#include <unistd.h>
#include "fe.h"
#include "shmem.h"
#include "../emu_log.h"
#include "../../lib/list.h"

u32 shm_base;

void FE_init()
{
    SHM_init();
}

void *fe()
{
    EMU_log_println(LOG, "Fe start");
    static int c = 0;
    while (1)
    {
        usleep(1000);
        EMU_log_println(LOG, "fe");
        if(c == 500) return NULL;
        /* Add one test cmd */
        c++;
        shm_get();
        while(shm_list_empty(FREE_LIST))
        {
            shm_release();
            shm_get();
        }
        shm_index t_i = shm_list_remove(FREE_LIST);
        shm_cmd *t_cmd = SHM_SLOT(t_i);
        t_cmd->lpn = 20;
        t_cmd->ops = SHM_WRITE_OPS;//
        t_cmd->size = 16 * KB;
        shm_list_add(t_i, RDY_LIST);
        shm_release();
    }
}