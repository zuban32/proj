#include <inc/fat32.h>
#include <inc/ata.h>
#include <inc/string.h>
#include <inc/console.h>
#include <inc/fs.h>

static uint8_t vbr[512];

static BiosParamBlock *bpb = (BiosParamBlock *)vbr;
static ExtBootRecord *ext_br = (ExtBootRecord *)((uintptr_t)vbr + sizeof(*bpb));
static int inited = 0;
static int first_usable_sector;

static int cluster_to_lba(int cluster)
{
	return first_usable_sector + (cluster-2)*bpb->sects_per_cluster;
}

static int cluster_to_fatsect(int cluster)
{
	return bpb->res_sectors + (cluster * 4) / (SECTOR_SIZE * 2);
}

// dummy FAT32 test
int init_fat32(void)
{
	set_cur_ind(0);
	ata_request_readsector(0, 1, 1);
	while(is_bsy() || get_cur_ind() < 1);

	kmemcpy((char*)vbr, (char*)get_ata_buffer(), 512);


	kprintf("FAT32 Volume: fats=%d, dirs=%d, root_cluster=%d\n", bpb->fats_number, bpb->dirs_number, ext_br->root_cluster);

	first_usable_sector = bpb->res_sectors + bpb->fats_number * ext_br->sects_per_fat;
	kprintf("Sects per cluster = %d, bytes per sector = %d\n", bpb->sects_per_cluster, bpb->bytes_per_sect);
	kprintf("Res sects = %d\n", bpb->res_sectors);
	kprintf("Sects_per_FAT = %d, 1st sector = %d\n", ext_br->sects_per_fat, first_usable_sector);
	inited = 1;

	set_cur_ind(0);

	int cluster = ext_br->root_cluster;
	int fat_sect = cluster_to_fatsect(cluster);
	int fat_off = cluster % 512;

	ata_request_readsector(fat_sect, 1, 1);
	while(is_bsy() || get_cur_ind() < 1);
	kprintf("fat entry = %x\n", *((uint32_t *)get_ata_buffer() + fat_off) & 0xfffffff);
	set_cur_ind(0);
	ata_request_readsector(cluster_to_lba(cluster), 1, 1);
	while(is_bsy() || get_cur_ind() < 1);
	DirectoryEntry *dir = (DirectoryEntry *)get_ata_buffer();
	if(dir->attrib == 0x0F) {
//		kprintf("LFN start found\n");
		while(dir->attrib == 0x0f) {
			dir++;
		}
//		kprintf("LFN end found\n");
		char cur_name[13];
		kprintf("Dir name = ");
		LFNEntry *lfn_cur = (LFNEntry *) dir-1;
		do {
			char *cur = cur_name;
			for(int i = 0; i < 5; i++)
				*cur++ = (char)lfn_cur->name_low[i];
			for(int i = 0; i < 6; i++)
				*cur++ = (char)lfn_cur->name_mid[i];
			for(int i = 0; i < 2; i++)
				*cur++ = (char)lfn_cur->name_high[i];

			kprintf("%s", cur_name);
		} while(!((lfn_cur--)->order & 0x40));
		kprintf("\n");
	} else {
		kprintf("Dir name = %s\n", dir->name);
	}
	set_cur_ind(0);

	return 0;
}

int fat32_open_file(char *path, int *size, FAT32FileInfo *info)
{
//	yet every path should start with root '/'
	if(*path != '/') {
		return -1;
	}
	char dir_name[MAX_PATH];
	char *cur_p = path + 1;
	while(*cur_p) {
		char *cur_dname = dir_name;
		while(*cur_p && *cur_p != '/') {
			*cur_dname++ = *cur_p++;
		}
		*cur_dname = 0;
		kprintf("Cur dir name = %s\n", dir_name);


		if(!*cur_p) {
			break;
		} else {
			cur_p++;
		}
	}
	return 0;
}

int fat32_read(FAT32FileInfo *info, int offset, int count)
{
	return 0;
}
