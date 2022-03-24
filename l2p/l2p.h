#ifndef L2P_H
#define L2P_H

#include"../lib/type.h"
#include"../fcl/fcl.h"
#include"../config/config.h"
#include"../config/mem.h"

extern u32* lpn_ppn;
extern u8* ppn_state;

void L2P_init();
void L2P_calc_obj_ppa(u32 obj_id, u64 obj_offset ,phy_page_addr *ppa);
void L2P_calc_ppa(u32 lpn, phy_page_addr *ppa);
void FTL_erase_init(void);

#endif
