#include <diskio.h>
#include <fathelper.h>
#include <ff.h>
#include <ffconf.h>

/*
  Create file system with 512 sector length
*/

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  while(!Serial);
  int size_in_sectors = 512;
  // Create FAT12 image with 512 sector length and FAT12DISK volume name
  BYTE *image = (BYTE*)create_fat12_image("FAT12DISK", size_in_sectors);

  if (image == NULL) {
    Serial.println("Failed to create FAT12 disk image in memory.");
    return;
  }

  // Init FatFS library witn in-memory disk image 
  set_disk_content_raw(image,size_in_sectors * SECTOR_SIZE);

  // Initialize FatFs
  FATFS fs;
  FRESULT fr;
  
  // Mount image to FatFS library
  fr = f_mount(&fs, "", 0);
  if (fr != FR_OK) {
      Serial.print("Failed to mount FAT12 image with code:");
      Serial.println(fr);
      free(image);
      return;
  }

  // Create test file and write it to image
  Serial.println("Write the file");
  FIL fil;
  fr = f_open(&fil, "test.txt", FA_WRITE | FA_CREATE_ALWAYS);
  if (fr == FR_OK) {
      unsigned int data_writed; 
      f_write(&fil, "Hello, FAT12!", 13, &data_writed);
      f_close(&fil);
      Serial.print("File created successfully. Bytes writed:");
      Serial.println(data_writed);
  } else {
      Serial.print("Failed to create file with code:");
      Serial.println(fr);
      return;
  }

  // Read catalog or root directory and print content
  DIR dir;           // Directory object
  FILINFO fno;       // File information object

  // Open the root folder
  char *path = "/";
  fr = f_opendir(&dir, path);
  if (fr != FR_OK) {
      Serial.print("Failed to open folder with code:");
      Serial.println(fr);
      return;
  }

  Serial.println("");
  Serial.println("Folder content:");

  // Read directory entries
  for (;;) {
      fr = f_readdir(&dir, &fno); // Read an entry
      if (fr != FR_OK || fno.fname[0] == 0) break; // Break on error or end of directory

      if (fno.fattrib & AM_DIR) {
          Serial.print("[DIR ] ");
          Serial.println(fno.fname);
      } else {
          Serial.print("[FILE] ");
          Serial.println(fno.fname);
      }
  }

  // Close the directory
  f_closedir(&dir);

  // Read file content
  char buffer[64];  // Buffer to store file data
  UINT bytesRead;   // Bytes read from the file
  char *filePath = "/test.txt";

  // Open the file for reading
  fr = f_open(&fil, filePath, FA_READ);
  if (fr != FR_OK) {
      Serial.print("Failed to open file: ");
      Serial.println(filePath);
      return;
  }

  // Read the file and print its content
  Serial.println("File content is:");
  Serial.println("::::");  
  do {
      fr = f_read(&fil, buffer, sizeof(buffer) - 1, &bytesRead);
      if (fr != FR_OK) {
          Serial.print("Error reading file with code: ");
          Serial.println(fr);
          return;
      }

      buffer[bytesRead] = '\0';  // Null-terminate the buffer for safe printing
      Serial.print(buffer);     // Print the buffer content
  } while (bytesRead > 0);
  Serial.println("");
  Serial.println("::::"); 

  f_close(&fil);


  // Get free space
  FATFS *filesys;
  DWORD freeClusters;
  DWORD totalSectors, freeSectors;
  Serial.println("");

  fr = f_getfree("", &freeClusters, &filesys);
  if (fr == FR_OK) {
    // Calculate total and free sectors
    totalSectors = (filesys->n_fatent - 2) * filesys->csize;
    freeSectors = freeClusters * filesys->csize;

    // Print the results
    Serial.print("Total space (KB): ");
    Serial.println(totalSectors / 2);  // Each sector is 512 bytes
    Serial.print("Free space (KB): ");
    Serial.println(freeSectors / 2);
  } else {
    Serial.print("Error getting free space: ");
    Serial.println(fr);
  }

  // Free image data
  free(image);
}

// the loop function runs over and over again forever
void loop() {

}
