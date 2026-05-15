#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define VFS_MAX_FILES   32
#define VFS_MAX_NAME    32
#define VFS_MAX_DATA    512

/* File types */
#define VFS_FILE        1
#define VFS_DIRECTORY   2

/* A single file/directory node */
typedef struct vfs_node {
    char     name[VFS_MAX_NAME];
    uint32_t type;
    uint32_t size;
    uint8_t  data[VFS_MAX_DATA];
    uint32_t inode;
} vfs_node_t;

/* VFS function table (driver interface) */
typedef struct {
    uint32_t   (*read) (vfs_node_t *node, uint8_t *buf, uint32_t size);
    uint32_t   (*write)(vfs_node_t *node, uint8_t *buf, uint32_t size);
    vfs_node_t*(*open) (const char *name);
    void       (*close)(vfs_node_t *node);
} vfs_driver_t;

void        vfs_init();
vfs_node_t *vfs_open(const char *name);
uint32_t    vfs_read(vfs_node_t *node, uint8_t *buf, uint32_t size);
uint32_t    vfs_write(vfs_node_t *node, uint8_t *buf, uint32_t size);
void        vfs_close(vfs_node_t *node);
void        vfs_register(vfs_driver_t *driver);
void        vfs_list();

#endif