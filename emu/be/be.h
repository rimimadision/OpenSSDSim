#ifndef BE_H
#define BE_H

#include <pthread.h>

#include "../../lib/type.h"
#include "../../lib/list.h"
#include "../../fcl/fcl.h"
#define FLASH_LOG_NAME "flash_log"

extern list_head register_list;
extern list_head complete_list;
extern pthread_mutex_t fcl_be_mutex;
extern pthread_cond_t list_cond;
typedef struct EMU_sq
{
    int ch;
    int ce;
    u32 sq_index;
    hw_queue_entry sq_entry;
    list_node node;
}EMU_sq;

void BE_init();
void *be();
void be_deal_register_sq(u32 sq_index, EMU_sq *sq);
void BE_send_sq(u32 sq_index, hw_queue_entry *sq_set, int ch, int ce);
void BE_write_log(u32 ch, u32 ce, u32 lun, u32 plane, u32 block, u32 page, u32 buf_addr);
void BE_read_log(u32 ch, u32 ce, u32 lun, u32 plane, u32 block, u32 page, u32 buf_addr);
#endif