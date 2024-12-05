/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include <stddef.h>

/* Definitions of physical drive number for each drive */
#define DEV_RAM_RAW		0	/* Map Ramdisk to physical drive 0 */
#define DEV_RAM_SECTOR		1	/* Map Ramdisk to physical drive 1  */

BYTE* disk_content_raw = NULL;
int disk_content_raw_length = 0;
BYTE** disk_content_sector = NULL;
int disk_content_sector_length = 0;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
		case DEV_RAM_RAW :
			if(disk_content_raw != NULL){
				return 0;
			}
		case DEV_RAM_SECTOR :
			if(disk_content_sector != NULL){
				return 0;
			}
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
		case DEV_RAM_RAW :
			if(disk_content_raw != NULL){
				return 0;
			}
		case DEV_RAM_SECTOR :
			if(disk_content_sector != NULL){
				return 0;
			}
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	switch (pdrv) {
	case DEV_RAM_RAW :
		if((sector + count + 1) * FF_MIN_SS <= disk_content_raw_length){
			memcpy(buff, &disk_content_raw[sector * FF_MIN_SS], count * FF_MIN_SS);
			return RES_OK;
		}
	case DEV_RAM_SECTOR :
		return RES_OK;
	}
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	switch (pdrv) {
		case DEV_RAM_RAW :
			if((sector + count + 1) * FF_MIN_SS <= disk_content_raw_length){
				memcpy(&disk_content_raw[sector * FF_MIN_SS], buff, count * FF_MIN_SS);
				return RES_OK;
			}
		case DEV_RAM_SECTOR :
			return RES_OK;
		}
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) {
		case DEV_RAM_RAW :
			switch (cmd) {
				case CTRL_SYNC:
					return RES_OK;  // Nothing to synchronize
				case GET_SECTOR_COUNT:
					*(DWORD*)buff = disk_content_raw_length / FF_MIN_SS;  // Return the number of sectors
					return RES_OK;
				case GET_SECTOR_SIZE:
					*(WORD*)buff = FF_MIN_SS;  // Return the sector size
					return RES_OK;
				case GET_BLOCK_SIZE:
					*(DWORD*)buff = 1;  // Block size in sectors (not relevant for memory disk)
					return RES_OK;
				default:
					return RES_PARERR;  // Unsupported command
		}
	case DEV_RAM_SECTOR :

	}
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Ram disk management Functions                                         */
/*-----------------------------------------------------------------------*/

BYTE* get_disk_content_raw() {
    return disk_content_raw;
}

int get_disk_content_raw_length() {
    return disk_content_raw_length;
}

void set_disk_content_raw(BYTE* content, int length) {
	disk_content_raw = content;
	disk_content_raw_length = length;
}