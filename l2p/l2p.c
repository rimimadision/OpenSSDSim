#include"l2p.h"

#include"../config/config.h"
#include"../fcl/fcl.h"
#include"../gc/gc.h"
#include"../ftl/ftl_core0.h"
#include "../emu/emu_log.h"
#include "../lib/lock.h"

#include <string.h>
#include<stdlib.h>
//#include<_timeval.h>
#include<time.h>

void L2P_print_ppa(phy_page_addr ppa) {
	xil_printf("===================================\n");
	xil_printf("ch : %d\n",ppa.ch);
	xil_printf("ce : %d\n",ppa.ce);
	xil_printf("lun : %d\n",ppa.nand_flash_addr.slc_mode.lun);
	xil_printf("plane : %d\n",ppa.nand_flash_addr.slc_mode.plane);
	xil_printf("block : %d\n",ppa.nand_flash_addr.slc_mode.block);
	xil_printf("page : %d\n",ppa.nand_flash_addr.slc_mode.page);
	xil_printf("-----------------------------------\n");
}


void L2P_init()
{
	
}

void L2P_calc_obj_ppa(u32 obj_id, u64 obj_offset ,phy_page_addr * ppa)
{
	u32 circle_num = 0;// (u32)obj_offset;

	u32 normal_ch[TOTAL_CHANNEL] = NORMAL_CHANNEL;
	u32 ce;
	u32 ch;
	u32 plane;
	u32 page;

	for(page = 0; (page < PAGE_PER_BLOCK_SLC_MODE); page++)
	{
		for(plane = 0; (plane < PLANE_PER_CE); plane ++)
		{
			for(ce = 0; (ce < CE_PER_CH); ce++)
			{
				for(ch = 0; (ch < TOTAL_CHANNEL); ch++)
				{
					if(normal_ch[ch] == 1)
					{
						if(circle_num == obj_offset)
						{
							ppa->ch = ch;
							ppa->ce = ce;
							ppa->nand_flash_addr.slc_mode.block = obj_id;
							ppa->nand_flash_addr.slc_mode.lun = 0;
							ppa->nand_flash_addr.slc_mode.plane = plane;
							ppa->nand_flash_addr.slc_mode.page = page;

							// xil_printf("ch %d  ce %d \n", ch, ce);

							return;
						}
						circle_num++;
					}

				}
			}
		}
	}

}


void L2P_calc_ppa(u32 lpn, phy_page_addr *ppa)
{
	u32 circle_num = 0;
	u32 normal_ch[TOTAL_CHANNEL] = NORMAL_CHANNEL;

	u32 valid_ch_num = 0;
	for(u32 i = 0; i < TOTAL_CHANNEL; i++)
	{
		if(normal_ch[i] == 1)
		{
			valid_ch_num++;
		}
	}

	u32 ce;
	u32 ch;
	u32 plane;

	u32 row = lpn/(valid_ch_num*CE_PER_CH*PLANE_PER_LUN*PAGE_PER_BLOCK_SLC_MODE);
	u32 row_offset = lpn%(valid_ch_num*CE_PER_CH*PLANE_PER_LUN*PAGE_PER_BLOCK_SLC_MODE);
	u32 page = row_offset/(valid_ch_num*CE_PER_CH*PLANE_PER_LUN);
	u32 page_offset = row_offset%(valid_ch_num*CE_PER_CH*PLANE_PER_LUN);

	for(plane = 0; (plane < PLANE_PER_CE); plane ++)
	{
		for(ce = 0; (ce < CE_PER_CH); ce++)
		{
			for(ch = 0; (ch < TOTAL_CHANNEL); ch++)
			{
				if(normal_ch[ch] == 1)
				{
					if(circle_num == page_offset)
					{
						ppa->ch = ch;
						ppa->ce = ce;
						ppa->nand_flash_addr.slc_mode.block = row;
						ppa->nand_flash_addr.slc_mode.lun = 0;
						ppa->nand_flash_addr.slc_mode.plane = plane;
						ppa->nand_flash_addr.slc_mode.page = page;

						return;
					}
					circle_num++;
				}

				}
			}
	}

}



void FTL_erase_init(void)
{
    u32 ch, ce, lun, plane, block;
    u32 sq_id = INVALID_INDEX;
    nand_flash_addr_cmd phyaddr;
    u32 hcmd_id = 0;
    u32 buff_id = 0;
    u32 ret = FAIL;
    memset(&phyaddr, 0, sizeof(nand_flash_addr_cmd));

    for (block = 0; block < BLOCK_PER_PLANE; block++)
    {
        for (lun = 0; lun < LUN_PER_CE; lun++)
        {
            for (plane = 0; plane < PLANE_PER_LUN; plane++)
            {
                for (ce = 0; ce < CE_PER_CH; ce++)
                {
                    for (ch = 0; ch < CH_PER_BOARD; ch++)
                    {

                        while (sq_id == INVALID_INDEX)
                        {
                            sq_id = FCL_get_free_SQ_entry(0);
                        }

                        phyaddr.tlc_mode.lun = lun;
                        phyaddr.tlc_mode.plane = plane;
                        phyaddr.tlc_mode.block = block;
                        //FCL_set_SQ_entry(hcmd_id, sq_id, buff_id, 3, &phyaddr);

                        while (ret == FAIL)
                        {
                            //TO DO ADD TIME OUT CHECK
                            ret = FCL_send_SQ_entry(sq_id, ch, ce);
                        }
                    }
                }
            }
        }
    }
}