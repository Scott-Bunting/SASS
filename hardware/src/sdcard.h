#ifndef sdcard_h
#define sdcard_h

#include <M5Stack.h>
// #include <time.h>
#include <Time.h>
#include <SD.h>

// Define pins for SD Card
#define SD_CS 4
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18

uint32_t startTime;

void listDir(const char *dirname, uint8_t levels);

void sdInit()
{
  File file = SD.open("/log.txt", FILE_APPEND);
  if (file) 
  {
    file.print(1);
    Serial.print("SIZE:  ");
    Serial.println(file.size());
    file.close();
  }
  else {
    Serial.println("Failed to open directory");
  }

  file = SD.open("/test.txt", FILE_APPEND);
  if (file)
  {
    file.print("testing");
    file.close();
  }
  else {
    Serial.println("Failed to open directory");
  }
}

void listDir(const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

#endif // sdcard_h