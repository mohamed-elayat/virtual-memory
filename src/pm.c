


#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "conf.h"
#include "pm.h"
#include "common.h"


static FILE *pm_backing_store;
static FILE *pm_log;
static char pm_memory[PHYSICAL_MEMORY_SIZE];
static unsigned int download_count = 0;
static unsigned int backup_count = 0;
static unsigned int read_count = 0;
static unsigned int write_count = 0;

// counter used to implement LRU for pm and to check
// availability of frame. The higher the frame counter,
// the older the frame. If frame counter is -1, then
// that frame is available (i.e. pm isn't full yet).
static int counters[NUM_FRAMES];


// Initialise la mémoire physique
void pm_init (FILE *backing_store, FILE *log)
{
  pm_backing_store = backing_store;
  pm_log = log;
//  memset (pm_memory, '\0', sizeof (pm_memory));
  memset (pm_memory, '_', sizeof (pm_memory));

  for(int i = 0; i < NUM_FRAMES; i++){ //todo: could have used a memset?
      counters[i] = -1;
  }

}

// Charge la page demandée du backing store
void pm_download_page (unsigned int page_number, unsigned int frame_number)
{
    download_count++;
    /* ¡ TODO: COMPLÉTER ! */

    if( fseek(pm_backing_store, page_number * PAGE_FRAME_SIZE, SEEK_SET) )
        error2("seek failed");

    fread(pm_memory + frame_number * PAGE_FRAME_SIZE,
            PAGE_FRAME_SIZE * sizeof(char),
            1,
            pm_backing_store);

}

// Sauvegarde la frame spécifiée dans la page du backing store
void pm_backup_page (unsigned int frame_number, unsigned int page_number)
{
    backup_count++;
    /* ¡ TODO: COMPLÉTER ! */

    if( fseek(pm_backing_store, page_number * PAGE_FRAME_SIZE, SEEK_SET) )
        error2("seek failed");

    fwrite(pm_memory + frame_number * PAGE_FRAME_SIZE,
            PAGE_FRAME_SIZE * sizeof(char),
            1,
            pm_backing_store);

}

void reset_frame_counter (unsigned int physical_address){
    int f_num = get_pf_number(physical_address);
    counters[f_num] = 0;
}

int get_free_frame(){

    for(int i = 0; i < NUM_FRAMES; i++) {
        if(counters[i] < 0) {
            return i;
        }
    }
    return -1;
}

// frame, which is -1,
// will necessarily change, because the frames can't all
// have counters equal to 0, because I increment them
// before this function gets called.

int get_replacement_frame(){

    int max = 0;
    int frame = -1;
    for(int i = 0; i < NUM_FRAMES; i++){
        if(counters[i] > max){
            frame = i;
            max = counters[i];
        }
    }
    return frame;
}


// with every read, increment all valid
// frames' counters, and reset the
// frame counter of the parameter address.
char pm_read (unsigned int physical_address)
{
    read_count++;
    /* ¡ TODO: COMPLÉTER ! */

    for(int i = 0; i < NUM_FRAMES; i++) {
        if(counters[i] >= 0 && counters[i] != INT_MAX) {
            counters[i] = counters[i] + 1; //todo: use ++
        }
    }

    reset_frame_counter(physical_address);

    return pm_memory[physical_address];
}

// with every write, increment all valid
// frames' counters, and reset the
// frame counter of the parameter address.
void pm_write (unsigned int physical_address, char c)
{
    write_count++;
    /* ¡ TODO: COMPLÉTER ! */

    for(int i = 0; i < NUM_FRAMES; i++) {
        if(counters[i] >= 0 && counters[i] != INT_MAX) {
            counters[i] = counters[i] + 1; //todo: use ++
        }
    }

    reset_frame_counter(physical_address);

    pm_memory[physical_address] = c;

}


void pm_clean (void)
{
  // Enregistre l'état de la mémoire physique.
  if (pm_log) {
      for (unsigned int i = 0; i < PHYSICAL_MEMORY_SIZE; i++)
        {
          if (i % 80 == 0)
            fprintf (pm_log, "%c\n", pm_memory[i]);
          else
            fprintf (pm_log, "%c", pm_memory[i]);
        }
    }
  fprintf (stdout, "Page downloads: %2u\n", download_count);
  fprintf (stdout, "Page backups  : %2u\n", backup_count);
  fprintf (stdout, "PM reads : %4u\n", read_count);
  fprintf (stdout, "PM writes: %4u\n", write_count);
}
