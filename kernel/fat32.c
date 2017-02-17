#include <inc/fat32.h>
#include <inc/ata.h>
#include <inc/string.h>
#include <inc/console.h>
#include <inc/fs.h>

static uint8_t vbr[512];
static uint8_t dir_buf[DIR_BUF_SIZE];
//static uint8_t fat_tbl_buf[SECTOR_SIZE];

static BiosParamBlock *bpb = (BiosParamBlock *)vbr;
static ExtBootRecord *ext_br = (ExtBootRecord *)((uintptr_t)vbr + sizeof(*bpb));
static int inited = 0;
static int first_usable_sector;

//static int is_dir(DirectoryEntry *dir)
//{
//	return dir->attrib == 0x10;
//}

static int cluster_to_lba(int cluster)
{
	return first_usable_sector + (cluster - 2) * bpb->sects_per_cluster;
}

//static int cluster_to_fatsect(int cluster)
//{
//	return bpb->res_sectors + (cluster * 4) / SECTOR_SIZE;
//}

static int fat32_directory_cluster(DirectoryEntry *dir)
{
	return dir->clusterlow | (dir->clusterhigh << 16);
}

static inline int fat32_valid(uint32_t fat32_entry)
{
	return fat32_entry > 1 && fat32_entry < FAT32_EOC;
}

//static int fat32_read_one_dir(DirectoryEntry *start)
//{
//	DirectoryEntry *dir = start;
//	if(dir->attrib == 0x0F) {
//	//		kprintf("LFN start found\n");
//			while(dir->attrib == 0x0f) {
//				dir++;
//			}
//	//		kprintf("LFN end found\n");
//			char cur_name[13];
//			kprintf("Dir name = ");
//			LFNEntry *lfn_cur = (LFNEntry *) dir-1;
//			do {
//				char *cur = cur_name;
//				for(int i = 0; i < 5; i++)
//					*cur++ = (char)lfn_cur->name_low[i];
//				for(int i = 0; i < 6; i++)
//					*cur++ = (char)lfn_cur->name_mid[i];
//				for(int i = 0; i < 2; i++)
//					*cur++ = (char)lfn_cur->name_high[i];
//
//				kprintf("%s", cur_name);
//			} while(!((lfn_cur--)->order & 0x40));
//			kprintf("\n");
//		} else {
//			kprintf("Dir name = %s\n", dir->name);
//		}
//
//	ata_read(cluster_to_fatsect(fat32_get_directory_cluster(start)), 1, 1, fat_tbl_buf);
//	uint32_t *fat_start = (uint32_t *)fat_tbl_buf;
//	while(fat32_valid(*fat_start)) {
//
//	}
//	return 0;
//}

static int fat32_read_dir(char *path)
{
	uint32_t dir_cluster = ext_br->root_cluster;

	char path_dir_name[MAX_PATH];
	char read_dir_name[MAX_PATH];
	char *cur_p = path;
	while(*cur_p) {
		char *cur_dname = path_dir_name;
		if(*cur_p != '/') {
			kprintf("Path read error: found [%c] instead of \\!\n", *cur_p);
			return -1;
		}
		cur_p++;
		while(*cur_p && *cur_p != '/') {
			*cur_dname++ = *cur_p++;
		}
		*cur_dname = 0;
		kprintf("Cur path dir name = %s\n", path_dir_name);

		ata_read(cluster_to_lba(dir_cluster), 1, 1, dir_buf);
		kprintf("Cur dir cluster = %d, LBA = %d\n", dir_cluster, cluster_to_lba(dir_cluster));
		DirectoryEntry *dir = (DirectoryEntry *)dir_buf, *cur_dir = dir;
		while(cur_dir->name[0]) {
			cur_dname = read_dir_name;

			if(cur_dir->name[0] == 0xE5 || cur_dir->name[0] == 0x05) {
//				skip this entry
				dir++;
				continue;
			}

			if(cur_dir->attrib == 0x0F) {
				kprintf("LFN start found\n");
				while(cur_dir->attrib == 0x0F && cur_dir < (DirectoryEntry *)dir_buf + DIR_BUF_NUM) {
					cur_dir++;
				}
				if(cur_dir >= (DirectoryEntry *)dir_buf + DIR_BUF_NUM) {
					// read the next directory cluster from FAT table
					// and then read its contents from disk
				}
				kprintf("LFN end found\n");
				LFNEntry *lfn_cur = (LFNEntry *) cur_dir-1;
				do {
					for(int i = 0; i < 5; i++)
						*cur_dname++ = (char)lfn_cur->name_low[i];
					for(int i = 0; i < 6; i++)
						*cur_dname++ = (char)lfn_cur->name_mid[i];
					for(int i = 0; i < 2; i++)
						*cur_dname++ = (char)lfn_cur->name_high[i];
				} while(!((lfn_cur--)->order & 0x40));
				*cur_dname++ = 0;
			} else {
				kmemcpy(read_dir_name, cur_dir->name, 8);
				*(read_dir_name + 8) = 0;
			}
			kprintf("Read dir name = %s\n", read_dir_name);

			if(!(cur_dir->attrib & 0x10)) {
				cur_dir++;
				kprintf("Not a dir - skip\n");
				continue;
			}

			if(kstrcmp(path_dir_name, read_dir_name) == 0) {
				kprintf("Folder [%s] found: [%s]\n", path_dir_name, read_dir_name);
				dir_cluster = fat32_directory_cluster(cur_dir);
//				kmemset(read_dir_name, 0, MAX_PATH);
//				kprintf("Next dir cluster: %d\n", dir_cluster);
				break;
			} else {
				kprintf("Wrong folder [%s]: wanna find [%s]\n", read_dir_name, path_dir_name);
				cur_dir++;
			}
		}

		if(!*cur_p) {
			if(cur_dir->name[0]) {
				kprintf("The end dir found - success\n");
			} else {
				kprintf("The end dir wasn't found - error\n");
			}
			break;
		}
	}
	return 0;
}

// dummy FAT32 test
int init_fat32(void)
{
	set_cur_ind(0);
	ata_request_readsector(0, 1, 1);
	while(is_bsy() || get_cur_ind() < 1);

	kmemcpy((char *)vbr, (char *)get_ata_buffer(), SECTOR_SIZE);

	kprintf("FAT32 Volume: fats=%d, dirs=%d, root_cluster=%d\n", bpb->fats_number, bpb->dirs_number, ext_br->root_cluster);

	first_usable_sector = bpb->res_sectors + bpb->fats_number * ext_br->sects_per_fat;
	kprintf("Sects per cluster = %d, bytes per sector = %d\n", bpb->sects_per_cluster, bpb->bytes_per_sect);
	kprintf("Res sects = %d\n", bpb->res_sectors);
	kprintf("Sects_per_FAT = %d, 1st sector = %d\n", ext_br->sects_per_fat, first_usable_sector);
	inited = 1;

	set_cur_ind(0);

//	int cluster = ext_br->root_cluster;
//	int fat_sect = cluster_to_fatsect(cluster);
//	int fat_off = cluster % 512;

//	fat32_read_dir("/test_dir/another_test_dir/zuban32_dir");
	fat32_read_dir("/test_dir/secret_folder/z32_folder");

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
