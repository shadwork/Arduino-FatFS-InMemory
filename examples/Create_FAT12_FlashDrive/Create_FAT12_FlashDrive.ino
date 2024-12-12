#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

/*
  Create file system with 256 sector length and provide it as FlashDrive
  To store the changes you should ubmount device properly
  Tested with ESP32-S2 board
*/

#include <diskio.h>
#include <fathelper.h>
#include <ff.h>
#include <ffconf.h>

#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "USB.h"
#include "USBMSC.h"

static const uint32_t DISK_SECTOR_COUNT = 256;
static const uint16_t DISK_SECTOR_SIZE = 512;
static const char *IMAGE_FILE = "/image.img";

USBMSC MSC;
uint8_t *msc_disk;

void saveImage(){
  File file = LittleFS.open(IMAGE_FILE, FILE_WRITE,true);
  if (!file) {
      Serial.println("Failed to create file");
      return;
  }

  size_t written = file.write(msc_disk, DISK_SECTOR_COUNT * DISK_SECTOR_SIZE);
  if (written == DISK_SECTOR_COUNT * DISK_SECTOR_SIZE) {
      Serial.println("File created successfully");
  } else {
      Serial.println("Error writing to file");
      Serial.println(written);
      LittleFS.format();
      Serial.println("Formatted SPIFF partition...");
  }
  file.close();
}

void loadImage(){
  File file = LittleFS.open(IMAGE_FILE, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  size_t bytesRead = file.read(msc_disk, DISK_SECTOR_COUNT * DISK_SECTOR_SIZE);
  if (bytesRead == DISK_SECTOR_COUNT * DISK_SECTOR_SIZE) {
    Serial.println("File loaded into msc_disk successfully");
  } else {
    Serial.printf("Error: Expected to read %d bytes, but only read %d bytes.\n",
                  DISK_SECTOR_COUNT * DISK_SECTOR_SIZE, bytesRead);
  }
  file.close();
}

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
  //Serial.printf("MSC WRITE: lba: %lu, offset: %lu, bufsize: %lu\n", lba, offset, bufsize);
  memcpy(&msc_disk[lba * DISK_SECTOR_SIZE + offset], buffer, bufsize);
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
  //Serial.printf("MSC READ: lba: %lu, offset: %lu, bufsize: %lu\n", lba, offset, bufsize);
  memcpy(buffer, &msc_disk[lba * DISK_SECTOR_SIZE + offset], bufsize);
  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
  Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
  if(load_eject){
    saveImage();
    LittleFS.end();
    MSC.end();
  }
  return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
      case ARDUINO_USB_STARTED_EVENT: Serial.println("USB PLUGGED"); break;
      case ARDUINO_USB_STOPPED_EVENT: Serial.println("USB UNPLUGGED"); break;
      case ARDUINO_USB_SUSPEND_EVENT: Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en); break;
      case ARDUINO_USB_RESUME_EVENT:  Serial.println("USB RESUMED"); break;

      default: break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  while(!Serial);

  if (!LittleFS.begin(true)) {
      Serial.println("LittleFS Mount Failed");
      return;
  }

  // Uncomment this if something wrong with file in SPIFF partition
  // if (LittleFS.exists(IMAGE_FILE)) {
  //   if (LittleFS.remove(IMAGE_FILE)) {
  //     Serial.println("File removed successfully!");
  //   } else {
  //     Serial.println("Failed to remove file.");
  //     return;
  //   }
  // }

  if (!LittleFS.exists(IMAGE_FILE)) {
    Serial.println("File not found, creating...");
    msc_disk = (uint8_t*)create_fat12_image("ESP32DISK", DISK_SECTOR_COUNT);
    saveImage();
  } else {
    Serial.println("File already exists.");
    msc_disk = (uint8_t*)malloc(DISK_SECTOR_COUNT * DISK_SECTOR_SIZE);
    loadImage();
  }

  USB.onEvent(usbEventCallback);
  MSC.vendorID("ESP32");       //max 8 chars
  MSC.productID("USB_MSC");    //max 16 chars
  MSC.productRevision("1.0");  //max 4 chars
  MSC.onStartStop(onStartStop);
  MSC.onRead(onRead);
  MSC.onWrite(onWrite);

  MSC.mediaPresent(true);
  MSC.isWritable(true);  // true if writable, false if read-only

  MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
  USB.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}
#endif /* ARDUINO_USB_MODE */
