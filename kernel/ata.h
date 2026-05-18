#ifndef ATA_H
#define ATA_H

#include <stdint.h>

/* ATA I/O ports (primary bus) */
#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECTOR_CNT  0x1F2
#define ATA_LBA_LO      0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HI      0x1F5
#define ATA_DRIVE_HEAD  0x1F6
#define ATA_STATUS      0x1F7
#define ATA_COMMAND     0x1F7

/* ATA status bits */
#define ATA_SR_BSY      0x80  /* busy                */
#define ATA_SR_DRDY     0x40  /* drive ready         */
#define ATA_SR_DRQ      0x08  /* data request ready  */
#define ATA_SR_ERR      0x01  /* error               */

/* ATA commands */
#define ATA_CMD_READ    0x20  /* read sectors        */
#define ATA_CMD_WRITE   0x30  /* write sectors       */
#define ATA_CMD_IDENTIFY 0xEC /* identify drive      */

/* Sector size */
#define ATA_SECTOR_SIZE 512

int  ata_init();
int  ata_read (uint32_t lba, uint8_t *buf, uint32_t sectors);
int  ata_write(uint32_t lba, uint8_t *buf, uint32_t sectors);

#endif