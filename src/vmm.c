

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

#include "conf.h"
#include "common.h"
#include "vmm.h"
#include "tlb.h"
#include "pt.h"
#include "pm.h"

static unsigned int read_count = 0;
static unsigned int write_count = 0;
static FILE* vmm_log;

void vmm_init (FILE *log)
{
  // Initialise le fichier de journal.
  vmm_log = log;
}


// NE PAS MODIFIER CETTE FONCTION
static void vmm_log_command (FILE *out,
                             const char *command,
                             unsigned int laddress, /* Logical address. */
                             unsigned int page,
                             unsigned int frame,
                             unsigned int offset,
                             unsigned int paddress, /* Physical address.  */
                             char c) /* Caractère lu ou écrit.  */
{
  if (out)
    fprintf (out, "%s[%c]@%05d: p=%d, o=%d, f=%d pa=%d\n", command, c, laddress,
	     page, offset, frame, paddress);
}

/* Effectue une lecture à l'adresse logique `laddress`.  */
char vmm_read (unsigned int laddress)
{

    //if address is invalid
    if(laddress >= NUM_PAGES * PAGE_FRAME_SIZE)
        error2("address out of range");

    char c = '!';
    read_count++;
    /* ¡ TODO: COMPLÉTER ! */

    int offset, p_num, f_num, p_num_to_overwrite;

    offset = get_offset(laddress);
    p_num = get_pf_number(laddress);

    f_num = tlb_lookup(p_num, 0);

    if(f_num >= 0) {
        c = pm_read(f_num * PAGE_FRAME_SIZE + offset);
        goto log_command;
    }

    f_num = pt_lookup(p_num);

    if(f_num >= 0) {
        c = pm_read(f_num * PAGE_FRAME_SIZE + offset);
        tlb_add_entry(p_num, f_num, 1);
        goto log_command;
    }

    f_num = get_free_frame();
    if(f_num >= 0) {
        goto update_tables;
    }

    f_num = get_replacement_frame();
    p_num_to_overwrite = pt_lookup_inverse(f_num); //todo: opt: look in tlb first

    if(  !pt_readonly_p(p_num_to_overwrite)  ) {
        pm_backup_page(f_num, p_num_to_overwrite);
    }

    pt_unset_entry(p_num_to_overwrite);
    tlb_unset_entry(p_num_to_overwrite);  // if exists in tlb

    update_tables:

    pm_download_page(p_num, f_num);
    pt_set_entry(p_num, f_num);
    tlb_add_entry(p_num, f_num, 1);

    // read count is decremented, so as
    // not to count the read twice
    read_count--;

    // since we just added p_num in tlb
    // and pt, this call will necessarily
    // be a tlb hit.
    return vmm_read(laddress);

    log_command:

    // TODO: Fournir les arguments manquants.

    vmm_log_command (
            stdout,
            "READING",
            laddress,
            p_num,
            f_num,
            offset,
            f_num*PAGE_FRAME_SIZE+offset,
            c);

    return c;
}

/* Effectue une écriture à l'adresse logique `laddress`.  */
void vmm_write (unsigned int laddress, char c)
{

    //if address is invalid
    if(laddress >= NUM_PAGES * PAGE_FRAME_SIZE)
        error2("address out of range");


    write_count++;


    /* ¡ TODO: COMPLÉTER ! */

    int offset, p_num, f_num, p_num_to_overwrite;

    offset = get_offset(laddress);
    p_num = get_pf_number(laddress);

    f_num = tlb_lookup(p_num, 0);

    if(f_num >= 0) {
        pm_write(f_num * PAGE_FRAME_SIZE + offset, c);
        pt_set_readonly(p_num, 0);
        tlb_set_readonly(p_num, 0);
        goto log_command;
    }

    f_num = pt_lookup(p_num);
    if(f_num >= 0) {

        tlb_add_entry(p_num, f_num, 1);

        pm_write(f_num * PAGE_FRAME_SIZE + offset, c);
        pt_set_readonly(p_num, 0);
        tlb_set_readonly(p_num, 0);

        goto log_command;
    }

    f_num = get_free_frame();
    if(f_num >= 0) {
        goto update_tables;
    }

    f_num = get_replacement_frame();
    p_num_to_overwrite = pt_lookup_inverse(f_num); //todo: opt: look in tlb first

    if(  !pt_readonly_p(p_num_to_overwrite)  ) {
        pm_backup_page(f_num, p_num_to_overwrite);
    }

    pt_unset_entry(p_num_to_overwrite);
    tlb_unset_entry(p_num_to_overwrite);

    update_tables:

    pm_download_page(p_num, f_num);
    pt_set_entry(p_num, f_num);
    tlb_add_entry(p_num, f_num, 1); // we will call vmm_write again, which will modify readonly to 0.
    write_count--;

    vmm_write(laddress, c);
    return;

    log_command:

    // TODO: Fournir les arguments manquants.
    vmm_log_command (
            stdout,
            "WRITING",
            laddress,
            p_num,
            f_num,
            offset,
            f_num * PAGE_FRAME_SIZE + offset,
            c);
}


// NE PAS MODIFIER CETTE FONCTION
void vmm_clean (void)
{

    for(int i = 0; i < NUM_FRAMES; i++){
        int p_num = pt_lookup_inverse(i);
        if(p_num >= 0){
            pm_backup_page(i, p_num);
        }
    }


  fprintf (stdout, "VM reads : %4u\n", read_count);
  fprintf (stdout, "VM writes: %4u\n", write_count);
}
