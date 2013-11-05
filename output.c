#include "output.h"

static int g_format = OUTPUT_JSON;

void
output_set_format(int format)
{
    g_format = format;
}

void
output_start()
{
    switch (g_format) {
    case OUTPUT_JSON:
        json_start();
        break;
    case OUTPUT_XML:
        xml_start();
        break;
    }
}

void
output_end()
{
    switch (g_format) {
    case OUTPUT_JSON:
        json_end();
        break;
    case OUTPUT_XML:
        xml_end();
        break;
    }
}

void
output_node(char *name)
{
    switch (g_format) {
    case OUTPUT_JSON:
        json_node(name);
        break;
    case OUTPUT_XML:
        xml_node(name);
        break;
    }
}

void
output_text(char *text)
{
    switch (g_format) {
    case OUTPUT_JSON:
        json_text(text);
        break;
    case OUTPUT_XML:
        xml_text(text);
        break;
    }
}

void
output_child_node(char *name, char *text)
{
    switch (g_format) {
    case OUTPUT_JSON:
        json_child_node(name, text);
        break;
    case OUTPUT_XML:
        xml_child_node(name, text);
        break;
    }
}

