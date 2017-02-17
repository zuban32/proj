#ifndef INC_FAT32_H_
#define INC_FAT32_H_

#include <inc/common.h>
#include <inc/ata.h>

typedef struct BiosParamBlock
{
	uint8_t jmp[3];
	char oem[8];
	uint16_t bytes_per_sect;
	uint8_t sects_per_cluster;
	uint16_t res_sectors;
	uint8_t fats_number;
	uint16_t dirs_number;
	uint16_t total_sects;
	uint8_t media_desc;
	uint16_t sects_per_fat; // obsolete in FAT32
	uint16_t sects_per_track;
	uint16_t heads;
	uint32_t start_lba;
	uint32_t total_sects_large;
} __attribute__ ((packed)) BiosParamBlock;

typedef struct ExtBootRecord
{
	uint32_t sects_per_fat;
	uint16_t flags;
	uint16_t fat_version;
	uint32_t root_cluster;
	uint16_t info_sect;
	uint16_t boot_backup;
	uint8_t res[12];
	uint8_t drive_no;
	uint8_t nt_flags;
	uint8_t signature;
	uint32_t serial;
	char label[11];
	char sys_id[8];
} __attribute__ ((packed)) ExtBootRecord;

typedef struct DirectoryEntry
{
	char name[8];
	char ext[3];
	uint8_t attrib;
	uint8_t userattrib;

	char undelete;
	uint16_t createtime;
	uint16_t createdate;
	uint16_t accessdate;
	uint16_t clusterhigh;

	uint16_t modifiedtime;
	uint16_t modifieddate;
	uint16_t clusterlow;
	uint32_t filesize;

} __attribute__ ((packed)) DirectoryEntry;

typedef struct LFNEntry
{
	uint8_t order;
	uint16_t name_low[5];
	uint8_t attr;
	uint8_t entry_type;
	uint8_t checksum;
	uint16_t name_mid[6];
	uint8_t res[2];
	uint16_t name_high[2];
} __attribute__ ((packed)) LFNEntry;

typedef struct FAT32FileInfo
{
	uint32_t start_cluster;
	uint32_t start_fat_entry;
} FAT32FileInfo;

enum {
	FAT32_EOC = 0xFFFFFF8,
	DIR_BUF_NUM = SECTOR_SIZE / sizeof(DirectoryEntry) + 1,
	DIR_BUF_SIZE = DIR_BUF_NUM * sizeof(DirectoryEntry)
};

int init_fat32(void);
int fat32_open_file(char *path, int *size, FAT32FileInfo *info);
int fat32_read(FAT32FileInfo *info, int offset, int count);

#endif /* INC_FAT32_H_ */
