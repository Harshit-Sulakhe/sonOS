#include "ata.h"
#include "io.h"

/* Wait until drive is not busy */
static int ata_wait_busy() {
    uint32_t timeout = 100000;
    while ((inb(ATA_STATUS) & ATA_SR_BSY) && timeout--)
        ;
    return (timeout == 0) ? -1 : 0;
}

/* Wait until drive is ready for data */
static int ata_wait_drq() {
    uint32_t timeout = 100000;
    while (!(inb(ATA_STATUS) & ATA_SR_DRQ) && timeout--) {
        uint8_t status = inb(ATA_STATUS);
        if (status & ATA_SR_ERR) return -1;
    }
    return (timeout == 0) ? -1 : 0;
}

/* Read a 16-bit word from ATA data port */
static inline uint16_t ata_read_word() {
    uint16_t val;
    asm volatile("inw %1, %0" : "=a"(val) : "Nd"((uint16_t)ATA_DATA));
    return val;
}

/* Write a 16-bit word to ATA data port */
static inline void ata_write_word(uint16_t val) {
    asm volatile("outw %0, %1" : : "a"(val), "Nd"((uint16_t)ATA_DATA));
}

int ata_init() {
    /* Select master drive on primary bus */
    outb(ATA_DRIVE_HEAD, 0xA0);

    /* Small delay — read status 4 times (400ns delay) */
    for (int i = 0; i < 4; i++)
        inb(ATA_STATUS);

    /* Send IDENTIFY command */
    outb(ATA_COMMAND, ATA_CMD_IDENTIFY);

    /* Check if drive exists */
    uint8_t status = inb(ATA_STATUS);
    if (status == 0) return -1; /* no drive */

    /* Wait for response */
    if (ata_wait_busy() < 0) return -1;

    /* Check mid/hi LBA ports — should be 0 for ATA */
    if (inb(ATA_LBA_MID) || inb(ATA_LBA_HI)) return -1;

    /* Wait for data */
    if (ata_wait_drq() < 0) return -1;

    /* Read 256 words of identify data (discard) */
    for (int i = 0; i < 256; i++)
        ata_read_word();

    return 0; /* drive found and ready */
}

int ata_read(uint32_t lba, uint8_t *buf, uint32_t sectors) {
    for (uint32_t s = 0; s < sectors; s++) {
        uint32_t cur_lba = lba + s;

        /* Wait until not busy */
        if (ata_wait_busy() < 0) return -1;

        /* Send LBA address and sector count */
        outb(ATA_DRIVE_HEAD,  0xE0 | ((cur_lba >> 24) & 0x0F));
        outb(ATA_SECTOR_CNT,  1);
        outb(ATA_LBA_LO,      cur_lba & 0xFF);
        outb(ATA_LBA_MID,     (cur_lba >> 8)  & 0xFF);
        outb(ATA_LBA_HI,      (cur_lba >> 16) & 0xFF);
        outb(ATA_COMMAND,     ATA_CMD_READ);

        /* Wait for data ready */
        if (ata_wait_drq() < 0) return -1;

        /* Read 256 words = 512 bytes into buffer */
        uint16_t *ptr = (uint16_t*)(buf + s * ATA_SECTOR_SIZE);
        for (int i = 0; i < 256; i++)
            ptr[i] = ata_read_word();
    }
    return 0;
}

int ata_write(uint32_t lba, uint8_t *buf, uint32_t sectors) {
    for (uint32_t s = 0; s < sectors; s++) {
        uint32_t cur_lba = lba + s;

        /* Wait until not busy */
        if (ata_wait_busy() < 0) return -1;

        /* Send LBA address and sector count */
        outb(ATA_DRIVE_HEAD,  0xE0 | ((cur_lba >> 24) & 0x0F));
        outb(ATA_SECTOR_CNT,  1);
        outb(ATA_LBA_LO,      cur_lba & 0xFF);
        outb(ATA_LBA_MID,     (cur_lba >> 8)  & 0xFF);
        outb(ATA_LBA_HI,      (cur_lba >> 16) & 0xFF);
        outb(ATA_COMMAND,     ATA_CMD_WRITE);

        /* Wait for drive ready to receive data */
        if (ata_wait_drq() < 0) return -1;

        /* Write 256 words = 512 bytes from buffer */
        uint16_t *ptr = (uint16_t*)(buf + s * ATA_SECTOR_SIZE);
        for (int i = 0; i < 256; i++)
            ata_write_word(ptr[i]);

        /* Flush write cache */
        outb(ATA_COMMAND, 0xE7);
        if (ata_wait_busy() < 0) return -1;
    }
    return 0;
}