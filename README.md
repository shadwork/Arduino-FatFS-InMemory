
# Arduino-FatFS-InMemory - An Arduino Library for In-Memory FatFS  

## Overview  

`Arduino-FatFS-InMemory` is an Arduino library that provides access to the FatFS library, with added functionality for handling in-memory file system images. This library enables the creation and manipulation of a virtual FAT12 filesystem stored entirely in memory.  

## Features  
- Full access to FatFS features.  
- Added support for an in-memory DISKIO implementation:  
  - Manipulate in-memory file system content with byte-level access.  
  - Easily create and use virtual FAT12 disk images.  

## Installation  

1. Clone or download the repository:  
   ```bash
   git clone https://github.com/shadwork/Arduino-FatFS-InMemory.git
   ```  
2. Copy the folder to your Arduino `libraries` directory.  
3. Include the library in your sketch:  
   ```cpp
    #include <diskio.h>
    #include <fathelper.h>
    #include <ff.h>
    #include <ffconf.h>
   ```

## API  

### In-Memory DiskIO Functions  
- **`BYTE* get_disk_content_raw()`**  
  - Retrieves a pointer to the raw content of the in-memory disk.  
- **`int get_disk_content_raw_length()`**  
  - Returns the length of the in-memory disk content in bytes.  
- **`void set_disk_content_raw(BYTE* content, int length)`**  
  - Replaces the in-memory disk content with a new byte array.  

### FAT12 Disk Image Helper  
- **`void* create_fat12_image(const char* volume_label, int total_sectors)`**  
  - Creates a FAT12-formatted disk image with the specified volume label and number of sectors.  
  - Returns a pointer to the disk image for use with `set_disk_content_raw`.  

## Example Usage  

Examples of library usage are available in the examples folder. After installation, they can be accessed in the Arduino IDE under the File → Examples menu.

## License  

This library includes and builds upon the original **FatFS** library by ChaN, adhering to its licensing terms:  

```
FatFs - Generic FAT Filesystem Module  
Copyright (C) 2023, ChaN  
Redistribution and use in source and binary forms, with or without modification,  
are permitted provided that the following conditions are met:  
```

## Credits  

- **Original FatFS Library**: ChaN ([FatFS Project](http://elm-chan.org/fsw/ff/00index_e.html))