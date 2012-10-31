#include "server.h"

void
xml_start()
{
    puts("<?xml version=\"1.0\" encoding=\"UTF-8\"?><urf:UsageRecord xmlns:urf=\"http://eu-emi.eu/namespaces/2011/11/computerecord\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://eu-emi.eu/namespaces/2011/11/computerecord car_v1.0.xsd \">");
}

void
xml_end()
{
    puts("</urf:UsageRecord>");
}

void
xml_node(char *name, char *value)
{
    printf("<urf:%s>%s</urf:%s>", name, value, name);
}