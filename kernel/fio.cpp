#include <fat32.h>
#include <util/string.h>
#include <console.h>
#include <fio.h>

static File file_table[MAX_OPEN_FILES];
static File *next_free_file = file_table;

File *file_open(char *path, enum FileMode mode)
{
	File *res = nullptr;
	if(next_free_file >= file_table && next_free_file < file_table + MAX_OPEN_FILES) {
		res = next_free_file++;
	}

//	here must be(and will be) drive/partition info request from corresponding Driver
//	now let it be FAT32 on ATA primary slave
	res->drive = 1;
	res->fs_type = FAT32;
	res->mode = mode; // now only reading is allowed
	kmemcpy(res->name, path, kstrlen(path));
	kprintf("fat32_open_file = %d\n", fat32_open_file(path, &res->size, (FAT32FileInfo *)&res->info));

	return res;
}

int file_read(File *f, int offset, int count)
{
	if(!f || count < 0 || count > f->size) {
		return -1;
	}
	return f->read(&f->info, offset, count);
}

int file_close(File *f)
{
	return 0;
}
