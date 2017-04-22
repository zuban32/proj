#include <fat32.h>
#include <util/string.h>
#include <console.h>
#include <fio.h>
#include <debug.h>

static FileIO fio;

int MountPoint::init(char l, FSType fs, ATAId id)
{
	this->letter = l;
	this->fs_type = fs;
	this->dev_id = id;

	return 0;
}

int File::global_id = 0;

bool File::name_valid()
{
	return name[0]  == '/' && (name[1] >= 'a' && name[1] <= 'z') && name[2] == '/';
}

int File::init(const char *nm, FileMode md, int sz, MountPoint *m)
{
	kmemcpy(this->name, const_cast<char *>(nm), MAX_PATH);
	this->mode = md;
	this->size = sz;
	this->mp = m;

	return this->name_valid();
}

// in perfect it must check _ALL_ possible disk interfaces and FS types
// but now only ATA and fat32 are supported
int FileIO::init()
{
	ATADriver *ata = (ATADriver *)this->reg->unit_lookup(UNIT_DRIVER, DRIVER_ATA);
	FAT32Unit *fat_fs = (FAT32Unit *)this->reg->unit_lookup(UNIT_DRIVER, DRIVER_FAT32);

	if(!ata || !fat_fs) {
		dprintf("Init error: either no ATA or no FAT32 driver is found\n");
		return 1;
	}

	this->fs[FAT32] = fat_fs;

	dprintf("dev_num = %d\n", ata->get_device_num());
	for(int i = 0; i < ata->get_device_num(); i++) {
		ATADevice *dev = ata->get_device(i);
		if(dev) {
			if(fat_fs->check_fs(dev)) {
				this->next_free_mp->init('a' + i, FAT32, dev->get_id());
				dprintf("MP[%c] for FS [%d] created: dev (%d, %d)\n",
						this->next_free_mp->get_name(),
						this->next_free_mp->get_fs_type(),
						this->next_free_mp->get_dev_id().bus,
						this->next_free_mp->get_dev_id().slave);
				this->next_free_mp++;
			}
		} else {
			break;
		}
	}

	return 0;
}

int FileIO::connect_from(Tunnel *t, int data)
{
	return 0;
}

int FileIO::handle(Event e, void *ret)
{
	return 0;
}


MountPoint *FileIO::find_mp(const char *full_path)
{
	kprintf("Looking for MP: %s [%c]\n", full_path, full_path[1]);
	for(MountPoint *mp = this->mp_table; mp < this->next_free_mp; mp++) {
		if(mp->get_name() == full_path[1]) {
			return mp;
		}
	}
	return nullptr;
}

File *FileIO::file_open(const char *path, FileMode mode)
{
	MountPoint *mp = this->find_mp(path);
	if(!mp) {
		kprintf("Mount point for path [%s] not found!\n", path);
		return nullptr;
	}
	if(mp->get_fs_type() != FAT32) {
		kprintf("File open: Unsupported file system [%d]!\n", mp->get_fs_type());
		return nullptr;
	}
	File *res = nullptr;

	if(next_free_file >= file_table && next_free_file < file_table + MAX_OPEN_FILES) {
		res = next_free_file++;
	}

	res->init(path, mode, 0, this->find_mp(path));
	kprintf("fat32_open_file = %d\n", ((FAT32Unit *)this->fs[res->get_fs()])->open_file(path + 2, res));

	return res;
}

int FileIO::file_read(File *f, int offset, int count, uint8_t *buf)
{
	if(!f || count < 0 || count > f->get_size()) {
		return -1;
	}
	return ((FAT32Unit *)this->fs[f->get_fs()])->read_file(f, offset, count, buf);
}

int FileIO::file_close(File *f)
{
	return 0;
}

File *fopen(const char *path)
{
	return fio.file_open(path, F_READ);
}

int fread(File *f, int offset, int count, uint8_t *buf)
{
	return fio.file_read(f, offset, count, buf);
}

