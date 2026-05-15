#ifndef INITRD_H
#define INITRD_H

#include "vfs.h"

#define INITRD_MAX_FILES 16

/* One file entry in the initrd */
typedef struct {
    char     name[VFS_MAX_NAME];
    uint8_t  data[VFS_MAX_DATA];
    uint32_t size;
    uint32_t used;
} initrd_file_t;

void        initrd_init();
void        initrd_create(const char *name,
                          const char *data,
                          uint32_t size);
vfs_node_t *initrd_open(const char *name);
uint32_t    initrd_read(vfs_node_t *node,
                        uint8_t *buf,
                        uint32_t size);
uint32_t    initrd_write(vfs_node_t *node,
                         uint8_t *buf,
                         uint32_t size);
void        initrd_close(vfs_node_t *node);
void        initrd_list();
int         initrd_count();
initrd_file_t *initrd_get(int idx);

#endif