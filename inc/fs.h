#ifndef INC_FS_H_
#define INC_FS_H_

#include <inc/common.h>

enum {
	MAX_PATH = 80,
	MAX_OPEN_FILES = 32
};

enum FileMode {
	F_READ 	= 1 << 0,
	F_WRITE = 1 << 1
};

enum FSType {
	FAT12,
	FAT16,
	FAT32,
	NTFS,
	// ...
	// now only FAT32 is partly supported
};

typedef struct FSFileInfo
{
	uint32_t start;
	uint32_t desc_start;
} FSFileInfo;

typedef struct File
{
	char name[MAX_PATH + 1];
	enum FileMode mode;
	int size;
	enum FSType fs_type;

	uint32_t drive;
	FSFileInfo info;

	int (*read)(FSFileInfo *info, int offset, int count);
} File;

File *file_open(char *path, enum FileMode mode);
int file_read(File *f, int offset, int count);
int file_close(File *f);

#endif /* INC_FS_H_ */
