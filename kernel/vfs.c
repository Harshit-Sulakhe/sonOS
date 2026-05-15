#include "vfs.h"

static vfs_driver_t *current_driver = 0;

void vfs_init() {
    current_driver = 0;
}

void vfs_register(vfs_driver_t *driver) {
    current_driver = driver;
}

vfs_node_t *vfs_open(const char *name) {
    if (!current_driver) return 0;
    return current_driver->open(name);
}

uint32_t vfs_read(vfs_node_t *node, uint8_t *buf, uint32_t size) {
    if (!current_driver || !node) return 0;
    return current_driver->read(node, buf, size);
}

uint32_t vfs_write(vfs_node_t *node, uint8_t *buf, uint32_t size) {
    if (!current_driver || !node) return 0;
    return current_driver->write(node, buf, size);
}

void vfs_close(vfs_node_t *node) {
    if (!current_driver || !node) return;
    current_driver->close(node);
}

void vfs_list() {
    /* Will be implemented by initrd */
    if (current_driver) {
        /* List all files by trying to open known names */
    }
}