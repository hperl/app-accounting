#include "server.h"

static const int BUF_SIZE = 256;
static char g_previous_tag[BUF_SIZE] = "\0";
static bool g_node_had_children = false;

void
xml_node_close_previous()
{
    if (g_previous_tag[0]) {
        if (g_node_had_children) {
            printf("\t");
        }
        printf("</urf:%s>\n", g_previous_tag);
        g_previous_tag[0] = 0;
    }
}

void
xml_start()
{
    puts("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<urf:UsageRecord xmlns:urf=\"http://eu-emi.eu/namespaces/2011/11/computerecord\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://eu-emi.eu/namespaces/2011/11/computerecord car_v1.0.xsd \">");
}

void
xml_end()
{
    xml_node_close_previous();
    puts("</urf:UsageRecord>");
}

void
xml_node(char *name)
{
    xml_node_close_previous();
    g_node_had_children = false;
    printf("\t<urf:%s>", name);
    strncpy(g_previous_tag, name, BUF_SIZE);
}

void
xml_text(char *text)
{
    printf("%s", text);
}

void
xml_child_node(char *name, char *text)
{
    if (!g_node_had_children) {
        g_node_had_children = true;
        printf("\n");
    }
    g_node_had_children = true;
    printf("\t\t<urf:%s>%s</urf:%s>\n", name, text, name);
}

