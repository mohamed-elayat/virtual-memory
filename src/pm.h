

#ifndef PHYSICAL_MEMORY_H
#define PHYSICAL_MEMORY_H

#include <stdint.h>
#include <stdio.h>

/* Initialise la mémoire physique.  */
void pm_init (FILE *backing_store, FILE *log);

void pm_download_page (unsigned int page_number, unsigned int frame_number);
void pm_backup_page (unsigned int frame_number, unsigned int page_number);
char pm_read (unsigned int physical_address);
void pm_write (unsigned int physical_address, char);
void pm_clean (void);


// iterates over all frames and finds the frame number
// of the first free frame. If such a frame exists, it returns
// its frame number. If there's no free frames, it returns -1.
int get_free_frame();

// resets to 0 the frame counter associated with the
// given physical address.
void reset_frame_counter (unsigned int physical_address);

// iterates over all frames and returns the frame number
// of the frame whose frame counter is the highest.
int get_replacement_frame();

#endif
