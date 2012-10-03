#include <common.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_MMC)

#ifdef CONFIG_LGE_FOTA_FEATURE
#include	"lge_fota_std.h"
#include	"oem_usd.h"
#endif

#include <mmc.h>
int mmc_flag[2] = {0, 0} ;

int do_mmc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong src_addr, dst_addr, size;
	char *cmd;
	
	int slot_no = 0, mmc_cont = 0;

	if (argc < 2) {
		goto mmc_cmd_usage;
	} else if (argc == 2) {
                if (strncmp(argv[0],"mmcinit",7) !=0) {
                        goto mmc_cmd_usage;
                } else {
			slot_no = simple_strtoul(argv[1], NULL, 16);
			if ((slot_no != 0) && (slot_no != 1))
				goto mmc_cmd_usage;
			if (mmc_init(slot_no) != 0) {
				printf("No MMC card found\n");
                                return 1;
			} else {
				mmc_flag[slot_no] = 1;
			}
                }
	} else {
		mmc_cont = simple_strtoul(argv[1], NULL, 16);
		if ((mmc_cont != 0) && (mmc_cont != 1))
			goto mmc_cmd_usage;

		if (!mmc_flag[mmc_cont]) {
			printf("Try to do init First b4 read/write\n");
			goto mmc_cmd_usage;
		}

		cmd = argv[2];
                if (strncmp(cmd, "read", 4) != 0 && 
                	strncmp(cmd, "write", 5) != 0 && 
#ifdef CONFIG_LGE_FOTA_FEATURE
			strncmp(cmd, "loadimage", 9) != 0 &&
#endif
                    strncmp(cmd, "erase", 5) != 0)


                goto mmc_cmd_usage;

                if (strcmp(cmd, "erase") == 0) {
			if (argc != 5) {
				goto mmc_cmd_usage;
			} else {
				src_addr = simple_strtoul(argv[3], NULL, 16);
				size = simple_strtoul(argv[4], NULL, 16);
				mmc_erase(mmc_cont, src_addr, size);
			}
		}
                if (strcmp(cmd, "read") == 0) {
			if (argc != 6) {
                                goto mmc_cmd_usage;
                        } else {
				src_addr = simple_strtoul(argv[3], NULL, 16);
				dst_addr = simple_strtoul(argv[4], NULL, 16);
				size = simple_strtoul(argv[5], NULL, 16);
				mmc_read(mmc_cont, src_addr,
					(unsigned char *)dst_addr, size);
			}
		}
		if (strcmp(cmd, "write") == 0) {
			if (argc != 6) {
				goto mmc_cmd_usage;
			} else {
				src_addr = simple_strtoul(argv[3], NULL, 16);
				dst_addr = simple_strtoul(argv[4], NULL, 16);
				size = simple_strtoul(argv[5], NULL, 16);
				mmc_write(mmc_cont, (unsigned char *)src_addr,
							dst_addr, size);
			}
		}

#ifdef CONFIG_LGE_FOTA_FEATURE
		if (strcmp(cmd, "loadimage") == 0)
		{
			sUpdateStatus	*mUSD;
			ua_u32			uiPartitionAddr;
			ua_u32			uiPartitionSize;

			FOTALOG_PRINT("Load Kernel Image from eMMC....\n");

			OemFlash_Initialize(mmc_cont, BOOT_CFG_KERNEL_LOAD_ADDR);
			OemUSD_Init(BOOT_CFG_KERNEL_LOAD_ADDR);
			mUSD = (sUpdateStatus*)BOOT_CFG_KERNEL_LOAD_ADDR;
			OemUSD_Load(mUSD);
			OemFlash_Release();

			if (((mUSD->enUpdateState == FOTA_STATE_FIRMWARE_UPDATE) ||
				 (mUSD->enUpdateState == FOTA_STATE_FILE_UPDATE)) &&
				 (mUSD->stFWInfo.uiCurrSection != (FOTA_FW_MAX_SECTION + FOTA_FW_AP_3_IMAGE)))
			{
				
				if (OemFlash_GetPartitionInfo(AP_3_PARTITION, &uiPartitionAddr, &uiPartitionSize) == ua_false)
				{
					FOTALOG_PRINT("OemFlash_GetPartitionInfo AP_3_PARTITION Error....\n");
					return 1;
				}

				FOTALOG_PRINT("Load [Recovery Kernel Image] from eMMC....\n");
			}
			else
			{
				
				if (OemFlash_GetPartitionInfo(AP_2_PARTITION, &uiPartitionAddr, &uiPartitionSize) == ua_false)
				{
					FOTALOG_PRINT("OemFlash_GetPartitionInfo AP_2_PARTITION Error....\n");
					return 1;
				}

				FOTALOG_PRINT("Load [Standard Kernel Image] from eMMC....\n");
				FOTALOG_PRINT("Load Address : %x, Size : %x\n", uiPartitionAddr, uiPartitionSize);
			}

			mmc_read(mmc_cont,
					 uiPartitionAddr/BOOT_CFG_LOAD_SECTOR_SIZE,
					 (unsigned char *)BOOT_CFG_KERNEL_LOAD_ADDR,
					 uiPartitionSize);
		}
#endif

	}
	return 0;

mmc_cmd_usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(mmcinit, 6, 1, do_mmc,
	"MMC sub system\n",
	" <controller[0/1]>\n"
	"mmc <controller[0/1]> read <src> <dst> <size>\n"
	"mmc <controller[0/1]> write <src> <dst> <size>\n"
	"mmc <controller[0/1]> erase <start> <size>\n"
#ifdef CONFIG_LGE_FOTA_FEATURE
	"mmc <controller[0/1]> loadimage\n"
#endif
	);
#endif
