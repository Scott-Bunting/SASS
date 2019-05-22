#ifndef sdcard_h
#define sdcard_h

#include <M5Stack.h>
#include <SD.h>
#include <string>

// #define DEBUG_SD // Uncomment for SD Card debugging

// Define pins for SD Card
#define SD_CS 4
#define SD_MOSI 23
#define SD_MISO 19
#define SD_CLK 18

bool firstLine;
bool sdCardAvailable;
bool stop;
bool paused;
uint16_t count;
int timeStartRecord;
int sessionNumber;
char sessionName[15];
String timeStamp;

void sdInit()
{
  count = 0;
  firstLine = true;

  File file = SD.open("/log.txt", FILE_APPEND);

  if (file)
  {
    sdCardAvailable = true;
    Serial.println("SD Card Mounted");

    file.print(1);
    sessionNumber = file.size();
    sprintf(sessionName, "/session%d", sessionNumber + 1);
    strcat(sessionName, ".csv");
    file.close();
  }
  else
  {
    Serial.println("Failed to open directory");
    sdCardAvailable = false;
  }

#ifdef DEBUG_SD
  Serial.print("SIZE:  ");
  Serial.println(sessionNumber);
  Serial.println(sessionName);
#endif // DEBUG
}

void sdRecord()
{
  File file = SD.open(sessionName, FILE_APPEND);
  if (file)
  {
    if (firstLine)
    {
      file.print(timeStamp);
      file.print("\n");
      file.print("count, GSR, HR, IBI, SpO2, \n");
      firstLine = false;
    }

    else
    {
      // initialising the empty Line Entry
      char lineEntry[100] = "\0";

      // Convert data into characters to create Line Entry for CSV
      char countChar[5];
      itoa(count, countChar, 10);
      char gsrChar[5];
      itoa(DATA.scl, gsrChar, 10);
      char HRChar[4];
      itoa(DATA.heartRate, HRChar, 10);
      char IBIChar[5];
      itoa(DATA.interbeatInterval, IBIChar, 10);
      char SpO2Char[10];
      itoa(DATA.spo2, SpO2Char, 10);

      // Creating Line Entry for CSV file
      strcat(lineEntry, countChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, gsrChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, HRChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, IBIChar);
      strcat(lineEntry, ",");
      strcat(lineEntry, SpO2Char);
      strcat(lineEntry, " \n");

      // add Line Entry to file and close
      file.print(lineEntry);
      file.close();

#ifdef DEBUG_SD
      Serial.println(lineEntry);
#endif // DEBUG_SD
    }
  }
  else
  {
    sdCardAvailable = false;

#ifdef DEBUG_SD
    Serial.println("SD Card Unavailable");
#endif // DEBUG_SD
  }
  ++count;
}

void sdWipe()
{
  File file = SD.open("/log.txt");
  int logSize = file.size();
  file.close();
  for (int i = 0; i < logSize; i++)
  {
    char fileName[15] = "\0";
    sprintf(fileName, "/session%d", i);
    strcat(fileName, ".csv");
    SD.remove(fileName);
  }
  SD.remove("/log.txt");
}

#endif // sdcard_h