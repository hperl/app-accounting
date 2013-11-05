#include "xml_output.h"
#include "json_output.h"

#ifndef app_accounting_output_h
#define app_accounting_output_h

#define OUTPUT_JSON 0
#define OUTPUT_XML  1

void
output_set_format(int format);

void
output_start();

void
output_end();

void
output_node(char *name);

void
output_text(char *text);

void
output_child_node(char *name, char *text);
#endif

