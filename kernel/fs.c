#include <inc/fs.h>
#include <inc/fat32.h>
#include <inc/string.h>
#include <inc/console.h>

static File file_table[MAX_OPEN_FILES];
static File *next_free_file = file_table;

File *file_open(char *path, enum FileMode mode)
{
	File *res = NULL;
	if(next_free_file >= file_table && next_free_file < file_table + MAX_OPEN_FILES) {
		res = next_free_file++;
	}

//	here must be(and will be) drive/partition info request from corresponding Driver
//	for now let it be FAT32 on ATA primary slave
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
