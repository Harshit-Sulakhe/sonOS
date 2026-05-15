#include "initrd.h"
#include "vfs.h"

static initrd_file_t files[INITRD_MAX_FILES];
static vfs_node_t    node_buf;
static int           file_count = 0;

static vfs_driver_t initrd_driver = {
    .read  = initrd_read,
    .write = initrd_write,
    .open  = initrd_open,
    .close = initrd_close
};

void initrd_init() {
    file_count = 0;
    for (int i = 0; i < INITRD_MAX_FILES; i++)
        files[i].used = 0;
    vfs_register(&initrd_driver);
}

void initrd_create(const char *name,
                   const char *data,
                   uint32_t size) {
    if (file_count >= INITRD_MAX_FILES) return;

    initrd_file_t *f = &files[file_count];

    /* Copy name */
    int i = 0;
    while (name[i] && i < VFS_MAX_NAME - 1) {
        f->name[i] = name[i]; i++;
    }
    f->name[i] = '\0';

    /* Copy data */
    uint32_t len = size < VFS_MAX_DATA ? size : VFS_MAX_DATA;
    for (uint32_t j = 0; j < len; j++)
        f->data[j] = data[j];

    f->size = len;
    f->used = 1;
    file_count++;
}

vfs_node_t *initrd_open(const char *name) {
    for (int i = 0; i < INITRD_MAX_FILES; i++) {
        if (!files[i].used) continue;

        /* Compare names */
        int match = 1;
        for (int j = 0; j < VFS_MAX_NAME; j++) {
            if (files[i].name[j] != name[j]) {
                match = 0; break;
            }
            if (files[i].name[j] == '\0') break;
        }
        if (match) {
            /* Fill node buffer */
            for (int j = 0; j < VFS_MAX_NAME; j++)
                node_buf.name[j] = files[i].name[j];
            for (uint32_t j = 0; j < files[i].size; j++)
                node_buf.data[j] = files[i].data[j];
            node_buf.size  = files[i].size;
            node_buf.type  = VFS_FILE;
            node_buf.inode = i;
            return &node_buf;
        }
    }
    return 0; /* not found */
}

uint32_t initrd_read(vfs_node_t *node,
                     uint8_t *buf,
                     uint32_t size) {
    uint32_t len = size < node->size ? size : node->size;
    for (uint32_t i = 0; i < len; i++)
        buf[i] = node->data[i];
    return len;
}

uint32_t initrd_write(vfs_node_t *node,
                      uint8_t *buf,
                      uint32_t size) {
    if (node->inode >= INITRD_MAX_FILES) return 0;
    initrd_file_t *f = &files[node->inode];
    uint32_t len = size < VFS_MAX_DATA ? size : VFS_MAX_DATA;
    for (uint32_t i = 0; i < len; i++)
        f->data[i] = buf[i];
    f->size = len;
    node->size = len;
    return len;
}

void initrd_close(vfs_node_t *node) {
    (void)node; /* nothing to do */
}

void initrd_list() {
    for (int i = 0; i < INITRD_MAX_FILES; i++) {
        if (files[i].used) {
            /* caller will print the name */
            (void)files[i].name;
        }
    }
}

int initrd_count() {
    return file_count;
}

/* Get file by index for listing */
initrd_file_t *initrd_get(int idx) {
    if (idx < 0 || idx >= INITRD_MAX_FILES) return 0;
    if (!files[idx].used) return 0;
    return &files[idx];
}