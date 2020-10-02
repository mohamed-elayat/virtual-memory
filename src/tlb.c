

#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#include "tlb.h"
#include "conf.h"

struct tlb_entry
{
  unsigned int page_number;
  int frame_number;             /* Invalide si négatif.  */
  bool readonly : 1;    //todo: is this useless?

  int counter;

};

static FILE *tlb_log = NULL;
static struct tlb_entry tlb_entries[TLB_NUM_ENTRIES]; 

static unsigned int tlb_hit_count = 0;
static unsigned int tlb_miss_count = 0;
static unsigned int tlb_mod_count = 0;

/* Initialise le TLB, et indique où envoyer le log des accès.  */
void tlb_init (FILE *log)
{
    for (int i = 0; i < TLB_NUM_ENTRIES; i++) {
          tlb_entries[i].frame_number = -1;
    }
    tlb_log = log;
}

/******************** ¡ NE RIEN CHANGER CI-DESSUS !  ******************/

/* Recherche dans le TLB.
 * Renvoie le `frame_number`, si trouvé, ou un nombre négatif sinon.  */

static int tlb__lookup (unsigned int page_number, bool write) //todo: is the write useless?
{
    // TODO: COMPLÉTER CETTE FONCTION.

    // increments counters for all tlb entries.
    for (int i = 0; i < TLB_NUM_ENTRIES; i++){
        if(tlb_entries[i].counter != INT_MAX)
            tlb_entries[i].counter++;
    }


    // if parameter page is found, reset its counter
    // to 0 and return its frame_number.
    for (int i = 0; i < TLB_NUM_ENTRIES; i++){

        if(tlb_entries[i].page_number == page_number) {

            if(tlb_entries[i].frame_number >= 0) {
                tlb_entries[i].counter = 0;
                return tlb_entries[i].frame_number;
            }
        }
    }

    return -1;
}



/* Ajoute dans le TLB une entrée qui associe `frame_number` à
 * `page_number`.  */
static void tlb__add_entry (unsigned int page_number,
                            unsigned int frame_number, bool readonly)
{
    // TODO: COMPLÉTER CETTE FONCTION.
    //todo: should I increment counters here as well?

    // if there's a free tlb entry, insert
    // the new entry in there.
    for (int i = 0; i < TLB_NUM_ENTRIES; i++){
        if(tlb_entries[i].frame_number < 0){
            tlb_entries[i].page_number = page_number;
            tlb_entries[i].frame_number = (int)frame_number;
            tlb_entries[i].readonly = readonly;
            tlb_entries[i].counter = 0;
            return;
        }
    }

    // overwrite the tlb entry with the highest
    // counter value

    int max = 0;
    struct tlb_entry* t = NULL;

    for (int i = 0; i < TLB_NUM_ENTRIES; i++){
        if(tlb_entries[i].counter > max) {
            t = &tlb_entries[i];
            max = tlb_entries[i].counter;
        }
    }

    // t will never be null, because I incremented the counters at the
    // beginning of the function

    t->page_number = page_number;
    t->frame_number = (int)frame_number;
    t->readonly = readonly;
    t->counter = 0;

}

// if exists, mark the entry whose page
// number is the same as param page number
// as invalid
void tlb_unset_entry(unsigned int page_number){
    for (int i = 0; i < TLB_NUM_ENTRIES; i++){
        if(tlb_entries[i].page_number == page_number){
            tlb_entries[i].frame_number = -1;
            return;
        }
    }
}

// if exists, modify the entry whose page
// number is the same as param page number
// and set its readonly to readonly
void tlb_set_readonly (unsigned int page_number, bool readonly){
    for (int i = 0; i < TLB_NUM_ENTRIES; i++){
        if(tlb_entries[i].page_number == page_number){
            tlb_entries[i].readonly = readonly;
            return;
        }
    }
}



/******************** ¡ NE RIEN CHANGER CI-DESSOUS !  ******************/

void tlb_add_entry (unsigned int page_number,
                    unsigned int frame_number, bool readonly)
{
  tlb_mod_count++;
  tlb__add_entry (page_number, frame_number, readonly);
}

int tlb_lookup (unsigned int page_number, bool write)
{
  int fn = tlb__lookup (page_number, write);
  (*(fn < 0 ? &tlb_miss_count : &tlb_hit_count))++;
  return fn;
}

/* Imprime un sommaires des accès.  */
void tlb_clean (void)
{
  fprintf (stdout, "TLB misses   : %3u\n", tlb_miss_count);
  fprintf (stdout, "TLB hits     : %3u\n", tlb_hit_count);
  fprintf (stdout, "TLB changes  : %3u\n", tlb_mod_count);
  fprintf (stdout, "TLB miss rate: %.1f%%\n",
           100 * tlb_miss_count
           /* Ajoute 0.01 pour éviter la division par 0.  */
           / (0.01 + tlb_hit_count + tlb_miss_count));
}
