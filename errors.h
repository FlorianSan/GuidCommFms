#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <ctype.h>

#include "app_guid_comm.h"

void error(char* function, char* param, int nb_errors);

void acquisition(void *data, char** argv);

int testFormat(char* c, char* type);

#endif
