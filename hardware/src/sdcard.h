#ifndef sdcard_h
#define sdcard_h

#include <M5Stack.h>
// #include <time.h>
#include <Time.h>
#include <SD.h>
#include <string>

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
  else
  {
    Serial.println("Failed to open directory");
  }

  file = SD.open("/test.txt", FILE_APPEND);
  if (file)
  {
    file.print("testing");
    file.close();
  }
  else
  {
    Serial.println("Failed to open directory");
  }
}

void sdRecord()
{
  File file = SD.open("/log.txt");
  int sessionNumber = file.size(); //don't need to add 1 because sdInit is run before and adds 1
  file.close();

  int str_length = 15;
  char sessionName[str_length];
  sprintf(sessionName, "/session%d", sessionNumber);
  strcat(sessionName, ".csv");
  Serial.println(sessionName);

  file = SD.open(sessionName, FILE_WRITE);
  if (file)
  {
    file.print("new file creation test");
  }
  else
  {
    Serial.println("No file created");
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