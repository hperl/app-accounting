#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <zmq.h>
#include <errno.h>
#include <yaml.h>

#include "output.h"
#include "symbols.h"

#ifndef app_accounting_server_h
#define app_accounting_server_h

void
xml_start();

void
xml_end();

void
xml_node(char *name);

void
xml_text(char *text);

void
xml_child_node(char *name, char *text);

#endif

