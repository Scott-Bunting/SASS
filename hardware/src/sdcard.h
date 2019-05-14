#ifndef sdcard_h
#define sdcard_h

#include <M5Stack.h>
// #include <time.h>
#include <Time.h>
#include <SD.h>
#include <string>

#define DEBUG_SD // Uncomment for SD Card debugging

// Define pins for SD Card
#define SD_CS 4
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18

uint32_t startTime;
bool firstLine;
int count;
int timeStartRecord;

void listDir(const char *dirname, uint8_t levels);

void sdInit()
{
  count = 0;
  firstLine = true;
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

  file = SD.open(sessionName, FILE_APPEND);
  if (file)
  {
    Serial.println(count);
    if (firstLine)
    {
      file.print("count, GSR, HR, IBI, SpO2, \n");
      firstLine = false;
    }
    else
    {
      char lineEntry[100];
      char countChar[10];
      itoa(count, countChar, 10);
      strcat(countChar, "\0");
      char gsrChar[5];
      itoa(DATA.scl, gsrChar, 10);
      char HRChar[4];
      itoa(DATA.heartRate, HRChar, 10);
      char IBIChar[5];
      itoa(DATA.interbeatInterval, IBIChar, 10);
      char SpO2Char[10];
      itoa(DATA.spo2, SpO2Char, 10);

      strcat(lineEntry, countChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, gsrChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, HRChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, IBIChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, SpO2Char);
      strcat(lineEntry, ",");
      strcat(lineEntry, " \n");

      file.print(lineEntry);
      file.close();

      #ifdef DEBUG_SD
      Serial.println(lineEntry);

      #endif // DEBUG
    }
  }
  else
  {
    Serial.println("No file created");
  }
  ++count;
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