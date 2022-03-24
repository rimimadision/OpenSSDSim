#ifndef FTL_TASK_QUEUE
#define FTL_TASK_QUEUE

#include"../lib/type.h"
#include"../hcl/hcl.h"

#define TASK_QUEUE_MAX (256)
#define FINISH

typedef struct _ftl_task_queue_entry
{
    u8 head;
    u8 tail;
    u16 cnt;
    // u8 resv;
    host_cmd_entry *queue[TASK_QUEUE_MAX];
    host_cmd_entry_status status;
    unsigned (*func)(host_cmd_entry *hcmd_entry);
} ftl_task_queue_entry;

//ftl_task_queue_entry *ftl_task_queue; //HCE_INIT = 6

u32 FTL_task_queue_add_tail(host_cmd_entry *hcmd_entry, host_cmd_entry_status status);
u32 FTL_get_taskqueue_cnt(host_cmd_entry_status status);
host_cmd_entry *FTL_task_queue_remove_head(host_cmd_entry_status status);
u32 FTL_sendhcmd(host_cmd_entry *hcmd, host_cmd_entry_status status);
void FTL_int_task_queue(void);
void FTL_core0task_polling(void);
void FTL_core1task_polling(void);

#endif
