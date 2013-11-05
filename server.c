#include "server.h"
#include "symbols.h"

const int THREAD_NUM = 1;

void
process(char *msg, size_t size)
{
    yaml_parser_t p;
    yaml_document_t d;
    yaml_node_t *root;

    yaml_parser_initialize(&p);
    yaml_parser_set_input_string(&p, (unsigned char*)msg, strlen(msg));
    yaml_parser_load(&p, &d);
    root = yaml_document_get_root_node(&d);
    xml_start();
    if (root->type == YAML_MAPPING_NODE) {
        for (yaml_node_pair_t *pair = root->data.mapping.pairs.start;
             pair < root->data.mapping.pairs.top; pair++) {
            yaml_node_t *k = yaml_document_get_node(&d, pair->key);
            yaml_node_t *v = yaml_document_get_node(&d, pair->value);
            xml_node((char*)k->data.scalar.value);
            if (v->type == YAML_SCALAR_NODE) {
                xml_text((char*)v->data.scalar.value);
            } else if (v->type == YAML_MAPPING_NODE) {
                for (yaml_node_pair_t *c_pair = v->data.mapping.pairs.start;
                     c_pair < v->data.mapping.pairs.top; c_pair++) {
                    yaml_node_t *ck = yaml_document_get_node(&d, c_pair->key);
                    yaml_node_t *cv = yaml_document_get_node(&d, c_pair->value);
                    xml_child_node((char*)ck->data.scalar.value,
                                   (char*)cv->data.scalar.value);
                }
            }
        }
    }
    xml_end();
    yaml_parser_delete(&p);
}

int
main(int argc, char *argv[])
{
    zmq_msg_t msg;
    void *sock, *context;

    context = zmq_init(THREAD_NUM);
    sock = zmq_socket(context, ZMQ_PULL);
    zmq_bind(sock, "tcp://*:5555");
    while (true) {
        zmq_msg_init(&msg);
        if (zmq_recv(sock, &msg, 0) == -1) {
            printf("%s\n", zmq_strerror(errno));
            exit(EXIT_FAILURE);
        }
        process((char *)zmq_msg_data(&msg), zmq_msg_size(&msg));
        zmq_msg_close(&msg);
    }
    zmq_close(sock);
    zmq_term(context);
}

