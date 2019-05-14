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

File root;
// time_t timert;

void sdInit()
{
  while (!Serial)
  {
    // wait for serial port to connect
  }
  listDir("/", 0);

  File file = SD.open("/test.txt", FILE_WRITE);

  startTime = millis();

  Serial.println(startTime);
  if (file) 
  {
    file.print(startTime);
    file.close();
  }

  file = SD.open("/test.txt", FILE_APPEND);
  if (file)
  {
    file.print("appended");
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