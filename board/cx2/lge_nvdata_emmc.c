#include <lge_nvdata_emmc.h>

extern int mmc_init(int slot);
extern int mmc_read(int mmc_cont, unsigned int src, unsigned char *dst, int size);
extern int mmc_write(int mmc_cont, unsigned char *src, unsigned long dst, int size);

int lge_dynamic_nvdata_emmc_read(enum lge_dynamic_nvdata_offset offset,char* buf,int size)
{
	int pos = (int)offset;
	int start_sector_no;
	int pos_in_sector;
	char sector_buf[LGE_NVDATA_SECTOR_SIZE];

	if(size <= 0)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_SMALL;

	if(size > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_LARGE;

	start_sector_no = LGE_DYNAMIC_NVDATA_PARTITION_START_SECTOR + (int)(pos/LGE_NVDATA_SECTOR_SIZE);
	pos_in_sector = pos % LGE_NVDATA_SECTOR_SIZE;

	if(start_sector_no > LGE_DYNAMIC_NVDATA_PARTITION_LAST_SECTOR)
		return LGE_NVDATA_EMMC_ERR_SECTOR_MISMATCH;

	if(size+pos_in_sector > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_ALIGN_MISMATCH;
	
	//mmc_init(1);
	// read nvdata partition
	mmc_read(1,start_sector_no,sector_buf,LGE_NVDATA_SECTOR_SIZE);

	memcpy(buf,&(sector_buf[pos_in_sector]),size);

	return size;
}

int lge_dynamic_nvdata_emmc_write(enum lge_dynamic_nvdata_offset offset,char* buf,int size)
{
	int pos = (int)offset;
	int start_sector_no;
	int pos_in_sector;
	char sector_buf[LGE_NVDATA_SECTOR_SIZE];

	if(size <= 0)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_SMALL;

	if(size > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_LARGE;

	start_sector_no = LGE_DYNAMIC_NVDATA_PARTITION_START_SECTOR + (int)(pos/LGE_NVDATA_SECTOR_SIZE);
	pos_in_sector = pos % LGE_NVDATA_SECTOR_SIZE;

	if(start_sector_no > LGE_DYNAMIC_NVDATA_PARTITION_LAST_SECTOR)
		return LGE_NVDATA_EMMC_ERR_SECTOR_MISMATCH;

	if(size+pos_in_sector > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_ALIGN_MISMATCH;
	
	//mmc_init(1);
	// read nvdata partition
	mmc_read(1,start_sector_no,sector_buf,LGE_NVDATA_SECTOR_SIZE);

	memcpy(&(sector_buf[pos_in_sector]),buf,size);

	mmc_write(1,sector_buf,start_sector_no,LGE_NVDATA_SECTOR_SIZE);

	return size;
}

int lge_static_nvdata_emmc_read(enum lge_dynamic_nvdata_offset offset,char* buf,int size)
{
	int pos = (int)offset;
	int start_sector_no;
	int pos_in_sector;
	char sector_buf[LGE_NVDATA_SECTOR_SIZE];

	if(size <= 0)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_SMALL;

	if(size > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_LARGE;

	start_sector_no = LGE_STATIC_NVDATA_PARTITION_START_SECTOR + (int)(pos/LGE_NVDATA_SECTOR_SIZE);
	pos_in_sector = pos % LGE_NVDATA_SECTOR_SIZE;

	if(start_sector_no > LGE_STATIC_NVDATA_PARTITION_LAST_SECTOR)
		return LGE_NVDATA_EMMC_ERR_SECTOR_MISMATCH;

	if(size+pos_in_sector > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_ALIGN_MISMATCH;
	
	//mmc_init(1);
	// read nvdata partition
	mmc_read(1,start_sector_no,sector_buf,LGE_NVDATA_SECTOR_SIZE);

	memcpy(buf,&(sector_buf[pos_in_sector]),size);

	return size;
}

int lge_static_nvdata_emmc_write(enum lge_dynamic_nvdata_offset offset,char* buf,int size)
{
	int pos = (int)offset;
	int start_sector_no;
	int pos_in_sector;
	char sector_buf[LGE_NVDATA_SECTOR_SIZE];

	if(size <= 0)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_SMALL;

	if(size > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_SIZE_TOO_LARGE;

	start_sector_no = LGE_STATIC_NVDATA_PARTITION_START_SECTOR + (int)(pos/LGE_NVDATA_SECTOR_SIZE);
	pos_in_sector = pos % LGE_NVDATA_SECTOR_SIZE;

	if(start_sector_no > LGE_STATIC_NVDATA_PARTITION_LAST_SECTOR)
		return LGE_NVDATA_EMMC_ERR_SECTOR_MISMATCH;

	if(size+pos_in_sector > LGE_NVDATA_SECTOR_SIZE)
		return LGE_NVDATA_EMMC_ERR_ALIGN_MISMATCH;
	
	//mmc_init(1);
	// read nvdata partition
	mmc_read(1,start_sector_no,sector_buf,LGE_NVDATA_SECTOR_SIZE);

	memcpy(&(sector_buf[pos_in_sector]),buf,size);

	mmc_write(1,sector_buf,start_sector_no,LGE_NVDATA_SECTOR_SIZE);

	return size;
}

