#include "fathelper.h"
#include <stddef.h>

// Convert a value to an 8-bit unsigned integer
uint8_t FAT12_U8(uint32_t value) {
    return (uint8_t)(value & 0xFF);
}

// Convert a value to a 16-bit packed FAT12 format
uint16_t FAT12_U16(uint32_t value) {
    return (uint16_t)((value & 0xFFFF));
}

// Convert a value to a 32-bit packed FAT12 format
uint32_t FAT12_U32(uint32_t value) {
    return value; // Direct return for 32-bit value
}

// Convert seconds and milliseconds into FAT12's tenths-of-a-second format
uint8_t FAT12_MS2B(uint8_t seconds, uint16_t milliseconds) {
    return (uint8_t)((((seconds & 0x1) * 1000) + milliseconds) / 10);
}

// Encode hours, minutes, and seconds into FAT12 packed time format
uint16_t FAT12_HMS2B(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    return (uint16_t)(((hours & 0x1F) << 11) | ((minutes & 0x3F) << 5) | ((seconds / 2) & 0x1F));
}

// Encode year, month, and day into FAT12 packed date format
uint16_t FAT12_YMD2B(uint16_t year, uint8_t month, uint8_t day) {
    return (uint16_t)((((year - 1980) & 0x7F) << 9) | ((month & 0xF) << 5) | (day & 0x1F));
}

void* create_fat12_image(const char* volume_label, int total_sectors) {
    // Allocate memory for the entire disk image
    size_t image_size = total_sectors * SECTOR_SIZE;
    void* image = calloc(1, image_size);
    if (!image) {
        return NULL;
    }

    // Setup boot sector
    BootSector* boot = (BootSector*)image;
    memcpy(boot->jmp, "\xEB\x3C\x90", 3); // JMP instruction
    memcpy(boot->oem, "MSDOS5.0", 8);
    boot->bytes_per_sector = SECTOR_SIZE;
    boot->sectors_per_cluster = 1;
    boot->reserved_sectors = 1;
    boot->fat_count = FAT_TABLE_COUNT;
    boot->root_entries = ROOT_DIR_ENTRIES;
    boot->total_sectors_short = (total_sectors <= 0xFFFF) ? total_sectors : 0;
    boot->media_descriptor = 0xF0;
    boot->sectors_per_fat = FAT_SECTORS;
    boot->sectors_per_track = 18;
    boot->number_of_heads = 2;
    boot->hidden_sectors = 0;
    boot->total_sectors_long = (total_sectors > 0xFFFF) ? total_sectors : 0;
    boot->drive_number = 0x80;
    boot->reserved = 0;
    boot->boot_signature = 0x29;
    boot->volume_id = 0x12345678; // Random volume ID
    strncpy(boot->volume_label, volume_label, 11);
    memcpy(boot->fs_type, "FAT12   ", 8);
    boot->boot_sector_signature = 0xAA55;

    // Setup root directory
    size_t root_dir_offset = (1 + FAT_TABLE_COUNT * FAT_SECTORS) * SECTOR_SIZE;
    DirectoryEntry* root_dir = (DirectoryEntry*)((uint8_t*)image + root_dir_offset);

    // Add volume label as the first entry
    memset(root_dir, 0, sizeof(DirectoryEntry));
    strncpy(root_dir->name, volume_label, 11);
    root_dir->attributes = 0x08; // Volume label
    root_dir->writeTime = FAT12_HMS2B(12, 0, 0);
    root_dir->writeDate = FAT12_YMD2B(2024, 1, 1);

    return image;
}