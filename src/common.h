

#ifndef COMMON_H
#define COMMON_H

/* Signale une erreure fatale.  Arguments comme `printf`.  */
void error (const char* fmt, ...);

// prints error and stop program
void error2 (const char* fmt);

//given an address (logical or physical),
// extracts its offset component
int get_offset(unsigned int address);

//given an address (logical or physical),
// extracts its page or frame number component
int get_pf_number(unsigned int address);

#endif

