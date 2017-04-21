#ifndef INC_FAT32_H_
#define INC_FAT32_H_

#include <abstract.h>
#include <fio.h>
#include <hw/ata.h>

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

struct FAT32DirInfo
{
	uint32_t cluster;
	uint32_t filesize;
	char name[MAX_PATH];
};

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

enum {
	FAT32_EOC = 0xFFFFFF8,
	DIR_BUF_NUM = SECTOR_SIZE / sizeof(DirectoryEntry) + 1,
	DIR_BUF_SIZE = DIR_BUF_NUM * sizeof(DirectoryEntry)
};

class FAT32Unit: public Unit
{
public:
	FAT32Unit(): Unit(UNIT_DRIVER, DRIVER_FAT32) {
		deps[deps_num][0] = UNIT_DRIVER;
		deps[deps_num][1] = DRIVER_ATA;
		deps_num++;
	}
	bool check_fs(ATADevice *dev);

	int open_file(const char *path, File *f);
	int read_file(File *f, int offset, int count, uint8_t *buf);

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);

	int test();
};

#endif /* INC_FAT32_H_ */
