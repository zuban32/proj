#include <fat32.h>
#include <hw/ata.h>
#include <util/math.h>
#include <util/string.h>
#include <console.h>
#include <fio.h>
#include <debug.h>

static uint8_t vbr[512];
static uint8_t dir_buf[DIR_BUF_SIZE];
static uint8_t fat_tbl_buf[SECTOR_SIZE];

static BiosParamBlock *bpb = (BiosParamBlock *) vbr;
static ExtBootRecord *ext_br =
        (ExtBootRecord *) ((uintptr_t) vbr + sizeof(*bpb));
static int first_usable_sector;

static FAT32Unit f32_unit;

static inline int cluster_to_lba(int cluster) {
    return first_usable_sector + (cluster - 2) * bpb->sects_per_cluster;
}

static inline int cluster_to_fatsect(int cluster) {
    return bpb->res_sectors + (cluster * 4) / SECTOR_SIZE;
}

static inline int fat32_directory_cluster(DirectoryEntry *dir) {
    return dir->clusterlow | (dir->clusterhigh << 16);
}

static inline int fat32_valid(uint32_t fat32_entry) {
    return fat32_entry > 1 && fat32_entry < FAT32_EOC;
}

static int fat32_read_dir(const char **cur_path, const int dir_cluster,
        ATADriver *ata, FAT32DirInfo *res) {
    char read_dir_name[MAX_PATH];
    char path_dir_name[MAX_PATH];
    int cluster = dir_cluster;

    char *cur_dname = path_dir_name;
    if (**cur_path != '/') {
        kprintf("Path read error: found [%c] instead of /!\n", **cur_path);
        return -1;
    }
    (*cur_path)++;
    while (**cur_path && **cur_path != '/') {
        *cur_dname++ = **cur_path;
        (*cur_path)++;
    }
    *cur_dname = 0;
//      kprintf("Cur path dir name = %s\n", path_dir_name);

    ata->read(cluster_to_lba(dir_cluster), 1, dir_buf);
//      kprintf("Cur dir cluster = %d, LBA = %d\n", cluster, cluster_to_lba(cluster));
    DirectoryEntry *dir = (DirectoryEntry *) dir_buf, *cur_dir = dir;
    while (cur_dir->name[0]) {
        cur_dname = read_dir_name;

        if (cur_dir->name[0] == 0xE5 || cur_dir->name[0] == 0x05) {
            // skip this entry
            dir++;
            continue;
        }

        if (cur_dir->attrib == 0x0F) {
//              kprintf("LFN start found\n");
            while (cur_dir->attrib == 0x0F) {
                cur_dir++;
                if (cur_dir >= (DirectoryEntry *) dir_buf + DIR_BUF_NUM) {
                    // read the next directory cluster from FAT table
                    // and then read its contents from disk
                    int fat_sect = cluster_to_fatsect(dir_cluster + 1);
                    ata->read(fat_sect, 1, fat_tbl_buf);
                    uint32_t *cur_entry = (uint32_t *) fat_tbl_buf
                            + fat_sect % SECTOR_SIZE;
                    if (fat32_valid(*cur_entry)) {
                        ata->read(cluster_to_lba(*cur_entry), 1, dir_buf);
                        cur_dir = (DirectoryEntry *) dir_buf;
                    } else {
                        return -1;
                    }
                }
            }
//              kprintf("LFN end found\n");
            LFNEntry *lfn_cur = (LFNEntry *) cur_dir - 1;
            do {
                for (int i = 0; i < 5; i++)
                    *cur_dname++ = (char) lfn_cur->name_low[i];
                for (int i = 0; i < 6; i++)
                    *cur_dname++ = (char) lfn_cur->name_mid[i];
                for (int i = 0; i < 2; i++)
                    *cur_dname++ = (char) lfn_cur->name_high[i];
            } while (!((lfn_cur--)->order & 0x40));
            *cur_dname++ = 0;
        } else {
            kmemcpy(read_dir_name, cur_dir->name, 8);
            *(read_dir_name + 8) = 0;
        }
//          kprintf("Read dir name = %s\n", read_dir_name);

//          if(!(cur_dir->attrib & 0x10)) {
//              cur_dir++;
//              kprintf("Not a dir - skip\n");
//              continue;
//          }

        if (kstrcmp(path_dir_name, read_dir_name) == 0) {
              kprintf("Folder [%s] found: [%s]\n", path_dir_name, read_dir_name);
            cluster = fat32_directory_cluster(cur_dir);
            kmemcpy(res->name, read_dir_name, MAX_PATH);
            res->cluster = fat32_directory_cluster(cur_dir);
            res->filesize = cur_dir->filesize;
//              kmemset(read_dir_name, 0, MAX_PATH);
//              kprintf("Next dir cluster: %d\n", dir_cluster);
            break;
        } else {
//              kprintf("Wrong folder [%s]: wanna find [%s]\n", read_dir_name, path_dir_name);
            cur_dir++;
        }
    }

    return cluster;
}

static int fat32_read_path(const char *path, FAT32DirInfo *out,
        ATADriver *ata) {
    if (!path || !out) {
        return -2;
    }
    uint32_t dir_cluster = ext_br->root_cluster;
    const char *cur_p = path;
    while (*cur_p) {
        FAT32DirInfo dir;
//        kprintf("Reading dir: %s\n", cur_p);
        dir_cluster = fat32_read_dir(&cur_p, dir_cluster, ata, &dir);
        if (!*cur_p) {
            if (dir.name[0]) {
                kprintf("The end dir found - success\n");
            } else {
                kprintf("The end dir wasn't found - error\n");
            }
            break;
        }
    }
    return 0;
}

int FAT32Unit::init(void) {
    ATADriver *ata = (ATADriver *) this->reg->unit_lookup(UNIT_DRIVER,
            DRIVER_ATA);
    int error = 0;
    if (ata->device_select(ATAId(0, 1))) {
        dprintf("Device select error\n");
        return 1;
    }

    if ((error = ata->read(0, 1, (unsigned char *) vbr))) {
        dprintf("Superblock read error %x\n", error);
        return 1;
    }

    dprintf("FAT32 Volume: fats=%d, dirs=%d, root_cluster=%d\n",
            bpb->fats_number, bpb->dirs_number, ext_br->root_cluster);

    first_usable_sector = bpb->res_sectors
            + bpb->fats_number * ext_br->sects_per_fat;
    dprintf("Sects per cluster = %d, bytes per sector = %d\n",
            bpb->sects_per_cluster, bpb->bytes_per_sect);
    dprintf("Res sects = %d\n", bpb->res_sectors);
    dprintf("Sects_per_FAT = %d, 1st sector = %d\n", ext_br->sects_per_fat,
            first_usable_sector);

    return 0;
}

// in current dev environment only primary slave contains fat32 fs
bool FAT32Unit::check_fs(ATADevice *dev) {
    return dev->get_id().bus == 0 && dev->get_id().slave == 1;
}

int FAT32Unit::open_file(const char *path, File *f) {
    ATADriver *ata = (ATADriver *) this->reg->unit_lookup(UNIT_DRIVER,
            DRIVER_ATA);
    ata->device_select(f->get_dev_id());
    int res = 0;
    FAT32DirInfo dir;
    if ((res = fat32_read_path(path, &dir, ata))) {
        kprintf("FAT32 read error: %d\n", res);
        return 1;
    } else {
        f->set_fsinfo(dir.cluster);
        f->set_size(dir.filesize);
        return 0;
    }
}

int FAT32Unit::read_file(File *f, int offset, int count, uint8_t *buf) {
    ATADriver *ata = (ATADriver *) this->reg->unit_lookup(UNIT_DRIVER,
            DRIVER_ATA);
    ata->device_select(f->get_dev_id());

    int cur_cluster = f->get_disk_start();
    int cur_off = 0, cur_read = 0;

    ata->read(cluster_to_fatsect(cur_cluster), 1, fat_tbl_buf);
    uint32_t *cur_entry = (uint32_t *) (fat_tbl_buf
            + (cur_cluster * 4) % SECTOR_SIZE);

    while (cur_off + SECTOR_SIZE < offset) {
        if ((unsigned) ((uint8_t *) cur_entry - fat_tbl_buf)
                >= SECTOR_SIZE / sizeof(*cur_entry)) {
            ata->read(cluster_to_fatsect(cur_cluster), 1, fat_tbl_buf);
            cur_entry = (uint32_t *) (fat_tbl_buf
                    + (cur_cluster * 4) % SECTOR_SIZE);
        }
        if (!fat32_valid(*cur_entry)) {
            kprintf("FAT32 file read: error (invalid FAT entry)!\n");
            return 1;
        }
        cur_cluster = *cur_entry & 0x0FFFFFFF;
        cur_off += SECTOR_SIZE;
    }

    cur_off = offset % SECTOR_SIZE;
    uint8_t file_buf[SECTOR_SIZE];
    while (cur_read < count) {
        ata->read(cluster_to_lba(cur_cluster), 1, file_buf);
        kmemcpy((char *) buf + cur_read, (char *) file_buf + cur_off,
                MIN(count - cur_read, SECTOR_SIZE));
        cur_read += MIN(count - cur_read, SECTOR_SIZE);

        if ((unsigned) ((uint8_t *) cur_entry - fat_tbl_buf)
                >= SECTOR_SIZE / sizeof(*cur_entry)) {
            ata->read(cluster_to_fatsect(cur_cluster), 1, fat_tbl_buf);
            cur_entry = (uint32_t *) (fat_tbl_buf
                    + (cur_cluster * 4) % SECTOR_SIZE);
        }
        if (!fat32_valid(*cur_entry)) {
            // cluster chain end
            break;
        }
        cur_cluster = *cur_entry & 0x0FFFFFFF;
        cur_off = 0;
    }

    return 0;
}

int FAT32Unit::handle(Event e, void *ret) {
    return 0;
}

int FAT32Unit::connect_from(Tunnel *t, int data) {
    return 0;
}

