#include <inc/fat32.h>
#include <inc/ata.h>
#include <inc/string.h>
#include <inc/console.h>

static uint8_t vbr[512];

// dummy FAT32 test
int init_fat32(void)
{
	set_cur_ind(0);
	ata_request_readsector(0, 1, 1);
	while(is_bsy() || get_cur_ind() < 1);

	kmemcpy((char*)vbr, (char*)get_ata_buffer(), 512);

	BiosParamBlock *bpb = (BiosParamBlock *)vbr;
	ExtBootRecord *ext_br = (ExtBootRecord *)((uintptr_t)vbr + sizeof(*bpb));

	kprintf("FAT32 Volume: fats=%d, dirs=%d, root_cluster=%d\n", bpb->fats_number, bpb->dirs_number, ext_br->root_cluster);

	int first_usable_sector = bpb->res_sectors + bpb->fats_number * ext_br->sects_per_fat;
	kprintf("Sects per cluster = %d, bytes per sector = %d\n", bpb->sects_per_cluster, bpb->bytes_per_sect);
	kprintf("Res sects = %d\n", bpb->res_sectors);
	kprintf("Sects_per_FAT = %d, 1st sector = %d\n", ext_br->sects_per_fat, first_usable_sector);

	set_cur_ind(0);

	int cluster = ext_br->root_cluster;
	int fat_sect = bpb->res_sectors + (cluster * 4) / 512;
	int fat_off = cluster % 512;

	ata_request_readsector(fat_sect, 1, 1);
	while(is_bsy() || get_cur_ind() < 1);
	kprintf("fat entry = %x\n", *((uint32_t *)get_ata_buffer() + fat_off) & 0xfffffff);
	set_cur_ind(0);
	ata_request_readsector(first_usable_sector + (cluster-2)*bpb->sects_per_cluster, 1, 1);
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

	return 0;
}
