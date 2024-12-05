/*-----------------------------------------------------------------------/
/  FAT creation and manager helper          							 /
/-----------------------------------------------------------------------*/

#include "ff.h"	

#ifndef _FATHELPER_DEFINED
#define _FATHELPER_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

// Constants
#define SECTOR_SIZE 512
#define ROOT_DIR_ENTRIES 224
#define FAT_SECTORS 9
#define FAT_TABLE_COUNT 2

#pragma pack(push, 1)
// Boot sector structure
typedef struct {
    uint8_t jmp[3];               // Jump instruction
    char oem[8];                  // OEM Name
    uint16_t bytes_per_sector;    // Bytes per sector
    uint8_t sectors_per_cluster;  // Sectors per cluster
    uint16_t reserved_sectors;    // Reserved sectors
    uint8_t fat_count;            // Number of FAT tables
    uint16_t root_entries;        // Max number of root directory entries
    uint16_t total_sectors_short; // Total sectors (if fits in 16 bits)
    uint8_t media_descriptor;     // Media descriptor
    uint16_t sectors_per_fat;     // Sectors per FAT
    uint16_t sectors_per_track;   // Sectors per track
    uint16_t number_of_heads;     // Number of heads
    uint32_t hidden_sectors;      // Hidden sectors
    uint32_t total_sectors_long;  // Total sectors (if not in 16 bits)
    uint8_t drive_number;         // Drive number
    uint8_t reserved;             // Reserved (always 0)
    uint8_t boot_signature;       // Extended boot signature
    uint32_t volume_id;           // Volume ID
    char volume_label[11];        // Volume label
    char fs_type[8];              // File system type
    uint8_t boot_code[448];       // Boot code
    uint16_t boot_sector_signature; // Boot sector signature (0x55AA)
} BootSector;

// Directory entry structure
typedef struct {
    char name[8];          // File name (8 characters, space-padded)
    char extension[3];     // File extension (3 characters, space-padded)
    uint8_t attributes;    // File attributes (e.g., read-only, hidden)
    uint8_t reserved;      // Reserved (usually 0)
    uint8_t createTimeTenths; // Creation time in tenths of a second
    uint16_t createTime;   // Creation time (HH:MM:SS, packed)
    uint16_t createDate;   // Creation date (YYYY-MM-DD, packed)
    uint16_t lastAccessDate; // Last access date (YYYY-MM-DD, packed)
    uint16_t firstClusterHigh; // High word of first cluster (for FAT32, 0 in FAT12/16)
    uint16_t writeTime;    // Last write time (HH:MM:SS, packed)
    uint16_t writeDate;    // Last write date (YYYY-MM-DD, packed)
    uint16_t firstClusterLow;  // Low word of first cluster
    uint32_t fileSize;     // File size in bytes
} DirectoryEntry;

#pragma pack(pop)

// Convert a value to an 8-bit unsigned integer
uint8_t FAT12_U8(uint32_t value);

// Convert a value to a 16-bit packed FAT12 format
uint16_t FAT12_U16(uint32_t value);

// Convert a value to a 32-bit packed FAT12 format
uint32_t FAT12_U32(uint32_t value);

// Convert seconds and milliseconds into FAT12's tenths-of-a-second format
uint8_t FAT12_MS2B(uint8_t seconds, uint16_t milliseconds);

// Encode hours, minutes, and seconds into FAT12 packed time format
uint16_t FAT12_HMS2B(uint8_t hours, uint8_t minutes, uint8_t seconds);

// Encode year, month, and day into FAT12 packed date format
uint16_t FAT12_YMD2B(uint16_t year, uint8_t month, uint8_t day);

// Function to create FAT12 image
void* create_fat12_image(const char* volume_label, int total_sectors);

#ifdef __cplusplus
}
#endif

#endif
