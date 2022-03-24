#ifndef SHMEM_H
#define SHMEM_H

#include "../emu_config.h"
#include "../../lib/type.h"

#define CMD_SLOT_NUM 256
#define EMU_SSD_SIZE (1 * GB)
#define HOST_BLOCK_SIZE (4 * KB)

#define SHM_READ_OPS 1
#define SHM_WRITE_OPS 2

typedef u32 shm_index;

#ifdef __i386__ // EMU is x32 programme
extern u32 shm_base;
#endif

typedef struct _shmem_list
{
    u32 next;
    u32 prev;
} __attribute__((packed)) shm_list;

/* cmd from fio stored in shmem*/
typedef struct shm_cmd
{
    u32 next;
    u32 prev;
    u32 ops;
    u32 lpn;
    u32 size;
} __attribute__((packed)) shm_cmd;

typedef struct shm_data
{
    char data[4 * KB];
} shm_data;

#define SHM_NAME "/shm-emussd"
#define SHM_BASE (shm_base)

#define SHM_DATA_BASE SHM_BASE
#define SHM_DATA_SIZE (CMD_SLOT_NUM * sizeof(shm_data))

#define SHM_CMD_BASE (SHM_DATA_BASE + SHM_DATA_SIZE)
#define SHM_CMD_SIZE (CMD_SLOT_NUM * sizeof(shm_cmd))

// list heads for free-list rdy-list proc-list

typedef enum LIST_TYPE
{
    FREE_LIST = 1,
    RDY_LIST,
    PROC_LIST,
} LIST_TYPE;
 
#define FREE_LIST_HEAD  ((volatile shm_list *)(SHM_CMD_BASE + SHM_CMD_SIZE))
#define RDY_LIST_HEAD   ((volatile shm_list *)(SHM_CMD_BASE + SHM_CMD_SIZE + sizeof(shm_list)))
#define PROC_LIST_HEAD  ((volatile shm_list *)(SHM_CMD_BASE + SHM_CMD_SIZE + 2 * sizeof(shm_list)))
#define CTRL_BYTE       ((int *)(SHM_CMD_BASE + SHM_CMD_SIZE + 3 * sizeof(shm_list)))

#define SHM_SIZE (SHM_DATA_SIZE + SHM_CMD_SIZE + 3 * sizeof(shm_list) + sizeof(int))
#define SHM_SLOT(n) ((n == CMD_SLOT_NUM) ? (NULL) : ((shm_cmd *)SHM_CMD_BASE + n))
#ifdef __x86_64__
#define SHM_INDEX(s) (((u64)(s)-SHM_CMD_BASE) / sizeof(shm_cmd))
#elif __i386__
#define SHM_INDEX(s) (((u32)(s)-SHM_CMD_BASE) / sizeof(shm_cmd))
#endif

#define SEM_NAME "sem_fio_emu"

/* TODO: shm操作x64和x32合并，然后改变参数LIST_TYPE,不需要使用switch并且可以扩展新的List_type */
#ifdef __i386__
/* 
 * cause we can't use pointer in shmem for different memspace in two process
 * so we need use index to replace
 */
static inline void shm_get()
{
    while (__sync_lock_test_and_set(CTRL_BYTE, 1));
}
static inline void shm_release()
{
    __sync_lock_release (CTRL_BYTE);
}
static inline int shm_list_empty(LIST_TYPE l_t)
{
    volatile shm_list *head;
    switch (l_t)
    {
    case FREE_LIST:
        head = FREE_LIST_HEAD;
        break;
    case RDY_LIST:
        head = RDY_LIST_HEAD;
        break;
    case PROC_LIST:
        head = PROC_LIST_HEAD;
        break;
    default:
        break;
    }

    if (head->next == CMD_SLOT_NUM)
    {
        return 1;
    }
    return 0;
}

static inline void shm_list_add(shm_index i, LIST_TYPE l_t)
{
    volatile shm_list *head;
    volatile shm_cmd *cmd;
    switch (l_t)
    {
    case FREE_LIST:
        head = FREE_LIST_HEAD;
        break;
    case RDY_LIST:
        head = RDY_LIST_HEAD;
        break;
    case PROC_LIST:
        head = PROC_LIST_HEAD;
        break;
    default:
        break;
    }
    volatile shm_cmd *slot = SHM_SLOT(i);

    if (shm_list_empty(l_t))
    {
        slot->next = head->next;
        head->next = i;

        slot->prev = head->prev;
        head->prev = i;
    }
    else
    {
        volatile shm_cmd *slot_tail = SHM_SLOT(head->prev);
        slot->prev = head->prev;
        head->prev = i;

        slot->next = slot_tail->next;
        slot_tail->next = i;
    }
}

static inline shm_index shm_list_remove(LIST_TYPE l_t)
{
    if (shm_list_empty(l_t))
    {
        return CMD_SLOT_NUM;
    }
    volatile shm_list *head;
    switch (l_t)
    {
    case FREE_LIST:
        head = FREE_LIST_HEAD;
        break;
    case RDY_LIST:
        head = RDY_LIST_HEAD;
        break;
    case PROC_LIST:
        head = PROC_LIST_HEAD;
        break;
    default:
        break;
    }

    volatile shm_index i = head->next;
    volatile shm_cmd *slot_remove = SHM_SLOT(i);
    // only one in list
    if (slot_remove->next == CMD_SLOT_NUM)
    {
        head->next = head->prev = CMD_SLOT_NUM;
    }
    else
    {
        volatile shm_cmd *slot_head = SHM_SLOT(slot_remove->next);

        slot_head->prev = slot_remove->prev;
        head->next = slot_remove->next;
    }

    return i;
}

void SHM_init();
void SHM_get_rdy_list(shm_index *index, shm_cmd *scmd);
void SHM_add_rdy_list(shm_index index, shm_cmd *scmd);
void SHM_add_free_list(shm_index index);
#elif __x86_64__
static inline void shm_get(u64 shm_base)
{
    while (__sync_lock_test_and_set(CTRL_BYTE, 1));
}
static inline void shm_release(u64 shm_base)
{
    __sync_lock_release (CTRL_BYTE);
}
static inline int shm_list_empty_x64(u64 shm_base, LIST_TYPE l_t)
{
    volatile shm_list *head;
    switch (l_t)
    {
    case FREE_LIST:
        head = FREE_LIST_HEAD;
        break;
    case RDY_LIST:
        head = RDY_LIST_HEAD;
        break;
    case PROC_LIST:
        head = PROC_LIST_HEAD;
        break;
    default:
        break;
    }

    if (head->next == CMD_SLOT_NUM)
    {
        return 1;
    }
    return 0;
}

static inline void shm_list_add_x64(u64 shm_base, shm_index i, LIST_TYPE l_t)
{
    volatile shm_list *head;
    volatile shm_cmd *slot;
    switch (l_t)
    {
    case FREE_LIST:
        head = FREE_LIST_HEAD;
        break;
    case RDY_LIST:
        head = RDY_LIST_HEAD;
        break;
    case PROC_LIST:
        head = PROC_LIST_HEAD;
        break;
    default:
        break;
    }
    slot = SHM_SLOT(i);

    if (shm_list_empty_x64(shm_base, l_t))
    {
        slot->next = head->next;
        head->next = i;

        slot->prev = head->prev;
        head->prev = i;
    }
    else
    {
        volatile shm_cmd *slot_tail = SHM_SLOT(head->prev);
        slot->prev = head->prev;
        head->prev = i;

        slot->next = slot_tail->next;
        slot_tail->next = i;
    }
}

static inline shm_index shm_list_remove_x64(u64 shm_base, LIST_TYPE l_t)
{
    volatile shm_list *head;
    volatile shm_cmd *slot_remove;
    volatile shm_index i;
    if (shm_list_empty_x64(shm_base, l_t))
    {
        return CMD_SLOT_NUM;
    }
    
    switch (l_t)
    {
    case FREE_LIST:
        head = FREE_LIST_HEAD;
        break;
    case RDY_LIST:
        head = RDY_LIST_HEAD;
        break;
    case PROC_LIST:
        head = PROC_LIST_HEAD;
        break;
    default:
        break;
    }

    i = head->next;
    slot_remove = SHM_SLOT(i);
    // only one in list
    if (slot_remove->next == CMD_SLOT_NUM)
    {
        head->next = head->prev = CMD_SLOT_NUM;
    }
    else
    {
        volatile shm_cmd *slot_head = SHM_SLOT(slot_remove->next);

        slot_head->prev = slot_remove->prev;
        head->next = slot_remove->next;
    }

    return i;
}
#endif
#endif