
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"
#include "conf.h"


void
error (const char *fmt, ...)
{
  va_list valist;

  va_start (valist, fmt);

  vfprintf (stderr, fmt, valist);
  exit (EXIT_FAILURE);

  va_end (valist);
}

void error2 (const char* fmt){
    fprintf(stderr, "error: %s", fmt);
    exit(EXIT_FAILURE);
}

int get_offset(unsigned int address){
    return (int)address % PAGE_FRAME_SIZE;
}

int get_pf_number(unsigned int address){
    return ((int)address - get_offset(address)) / PAGE_FRAME_SIZE;
}
