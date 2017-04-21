#ifndef INC_FIO_H_
#define INC_FIO_H_

#include <abstract.h>
#include <hw/ata.h>

enum {
	MAX_PATH = 80,
	MAX_OPEN_FILES = 32,
	MAX_MOUNT_POINTS = 'z' - 'a' + 1,
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
	MAX_FS
	// ...
	// now only FAT32 is partly supported
};

class MountPoint
{
	char letter;
	FSType fs_type;
	ATAId dev_id;

public:
	inline char get_name()
	{
		return this->letter;
	}

	inline ATAId get_dev_id()
	{
		return this->dev_id;
	}

	inline FSType get_fs_type()
	{
		return this->fs_type;
	}

	int init(char l, FSType fs, ATAId id);
};

struct FSFileInfo
{
	uint32_t disk_start;
};

class File
{
	static int global_id;
	int id = global_id++;
	char name[MAX_PATH + 1];
	FileMode mode;
	int size;

	FSFileInfo info;
	MountPoint *mp = nullptr;

public:
	inline int get_id()
	{
		return this->id;
	}

	inline ATAId get_dev_id()
	{
		return this->mp->get_dev_id();
	}

	inline FSType get_fs()
	{
		return this->mp->get_fs_type();
	}

	inline int get_disk_start()
	{
		return this->info.disk_start;
	}

	inline int get_size()
	{
		return this->size;
	}

	inline void set_fsinfo(uint32_t val)
	{
		this->info.disk_start = val;
	}

	inline void set_size(uint32_t sz)
	{
		this->size = sz;
	}

	int init(const char *nm, FileMode md, int sz, MountPoint *m);
	bool name_valid();

};

class FileIO: public Unit
{
	File file_table[MAX_OPEN_FILES];
	File *next_free_file = file_table;
	MountPoint mp_table[MAX_MOUNT_POINTS];
	MountPoint *next_free_mp = mp_table;
	Unit *fs[MAX_FS] = {nullptr};

public:
	FileIO(): Unit(UNIT_SUBSYSTEM, SS_FILE) {
		deps[deps_num][0] = UNIT_DRIVER;
		deps[deps_num][1] = DRIVER_FAT32;
		deps_num++;
	}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);

	MountPoint *find_mp(const char *full_path);
	File *file_open(const char *path, FileMode mode);
	int file_read(File *f, int offset, int count, uint8_t *buf);
	int file_close(File *f);
};

File *fopen(const char *path);
int fread(File *f, int offset, int count, uint8_t *buf);

#endif /* INC_FIO_H_ */
