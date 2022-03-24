#ifndef FTL_NVME_H
#define FTL_NVME_H

#define MAX_POLLING_NUM (8)

void FTL_nvme_init(void);
void FTL_nvme_req_polling();

#include "../emu/emu_config.h"
#ifdef EMU
#include "../emu/fe/shmem.h"
void ftl_get_rdy_list(shm_index *index, shm_cmd *scmd);
void ftl_add_rdy_list(shm_index index, shm_cmd *scmd);
void ftl_add_free_list(shm_index index);

#endif

#endif
