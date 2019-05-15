/* 
  Project firmware for M5Stack Fire
  Authors: Ben Greenberg & Scott Bunting
  February 2019

  The project is available on GitHub: https://github.com/nebbles/MHML
  Mobile Healthcare & Machine Learning,
  Imperial College London

  Utilises PPG and GSR sensors on body to measure key metrics. Information 
  is hosted and transmitted from a BLE server running on device. Data is
  also displayed on device LCD for convenience.
*/
#include <M5Stack.h>
#include "data.h"
#include "ble.h"
#include "gsr.h"
#include "ppg.h"
#include "led.h"
#include "sdcard.h"

// #define DEBUG_MAIN // Uncomment whilst debugging for Serial debug stats.

enum class Modes
{
  FULL,
  BLE_ONLY
} MODE;

void M5off()
{
  ledsOff();
  M5.powerOFF();
}

void setup()
{
  count = (uint32_t)0;
  DATA.FIRMWARE_REVISION = "0.4.0";
  String DEVICE_NAME = "SASS M5 S";

  Serial.begin(115200);
  M5.begin();
  Wire.begin();
  M5.Speaker.mute();
  // pinMode(25, OUTPUT); // speaker set as output
  // dacWrite(25, 0);     // speaker drive low

  ledInit();

  // ledRunTest(); // for testing only

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(112, 50);
  M5.Lcd.print("SASS");

  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(64, 90);
  M5.Lcd.print(DEVICE_NAME + " v" + DATA.FIRMWARE_REVISION);

  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(22, 120);
  //M5.Lcd.print("github.com/nebbles/MHML");

  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(28, 170);
  M5.Lcd.print("Select Sensor Location");

  M5.Lcd.setTextColor(MAGENTA);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 220);
  M5.Lcd.print("   Proto   Other   Record  ");

  MODE = Modes::FULL;
  int button = 0;
  long loopStartTimer = millis();
  while (true)
  {
    if (millis() > loopStartTimer + 60000)
      M5off();

    if (millis() > loopStartTimer + 10000)
      ledStartScreen();

    if (M5.BtnB.isPressed() && M5.BtnC.isPressed())
    {
      MODE = Modes::BLE_ONLY;
      break;
    }
    else if (M5.BtnA.pressedFor(100))
    {
      button = 3; // BSL = Finger (see BLE spec)
      break;
    }
    else if (M5.BtnB.pressedFor(100))
    {
      button = 2; // BSL = Wrist (see BLE spec)
      break;
    }
    else if (M5.BtnC.pressedFor(100))
    {
      button = 0; // BSL = Other (see BLE spec)
      Serial.println("BtnC Pressed");
      sdInit();
      // while (true)
      // {
      //   if (M5.BtnC.pressedFor(100))
      //   {
      //     // sdRecord();
      //     Serial.println("[DEBUG] sdRecord has run");
      //     break;
      //   }
      // }
      break;
    }
    M5.update();
  }

  DATA.ppgBSL = 3; // button; // set PPG body sensor location (Fixed to 3 - fingers - due to broken buttons)
  DATA.gsrBSL = 3; // button; // set GSR body sensor location (Fixed to 3 - fingers - due to broken buttons)

  bleInit(DEVICE_NAME);

  // Reset Screen
  M5.Lcd.fillRect(0, 0, 320, 280, BLACK);

  // Write text to show that Measurement has Started
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print((String)DEVICE_NAME + " v" + DATA.FIRMWARE_REVISION);

  if (MODE == Modes::FULL)
  {
    ppgInit();
    gsrInit();

    // PPG Graph
    M5.Lcd.drawLine(10, 170, 150, 170, WHITE); // Draw x-axis for Graph
    M5.Lcd.drawLine(10, 30, 10, 170, WHITE);   // Draw y-axis for Graph

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(10, 175);
    M5.Lcd.print("PPG Graph");

    // GSR Graph
    M5.Lcd.drawLine(170, 170, 310, 170, WHITE); // Draw x-axis for Graph
    M5.Lcd.drawLine(170, 30, 170, 170, WHITE);  // Draw y-axis for Graph

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(170, 175);
    M5.Lcd.print("GSR Graph");

    timeStartGSR = millis();
    timeStartRecord = millis();
    gsrRun();
  }

#ifdef DEBUG_MAIN
  Serial.println("[DEBUG] Setup Loop Complete");
#endif //DEBUG_MAIN
}

/* 
 * Draw the raw PPG data to graph on LCD.  
 */
void drawGraphPPG()
{
  M5.Lcd.fillRect(11, 30, 140, 140, BLACK); // Clear and reset PPG Screen
  for (int i = 0; i < 280; i++)
  {
    int graphPos = ppgDeque[i];
    // Temp fix to prevent diagonal line from being drawn
    if (graphPos > 30 && graphPos < 170)
      M5.Lcd.drawPixel(i + 11, graphPos, RED);
  }
}

/* 
 * Draw the raw GSR data to graph on LCD.  
 */
void drawGraphGSR()
{
  M5.Lcd.fillRect(171, 30, 140, 140, BLACK); // Clear and reset GSR Graph
  for (int i = 0; i < 280; i++)              // Draw Graph
  {
    int graphPos = gsrDeque[i];
    // Temp fix to prevent diagonal line from being drawn
    if (graphPos > 30 && graphPos < 170)
      M5.Lcd.drawPixel(i + 171, graphPos, YELLOW);
  }
}

/* 
 * LCD space for displaying sensor values
 */
void drawLcdSensorValues()
{
  M5.Lcd.fillRect(0, 185, 320, 10, BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(1);

  M5.Lcd.setCursor(10, 185);
  M5.Lcd.print("HR:");
  M5.Lcd.setCursor(28, 185);
  M5.Lcd.print((int)(DATA.heartRate));

  M5.Lcd.setCursor(80, 185);
  M5.Lcd.print("SpO2:");
  M5.Lcd.setCursor(110, 185);
  M5.Lcd.print((int)(DATA.spo2));

  M5.Lcd.setCursor(170, 185);
  M5.Lcd.print("SCL:");
  M5.Lcd.setCursor(194, 185);
  M5.Lcd.print((int)(DATA.scl));
}

/*
 * LCD space for BLE info text. Chars are dx=6, dy=10.
 * x: 0   -> 320 - 'char width'
 * y: 195 -> 240 - 'char height'
 */
void drawLcdBleStatus()
{
  M5.Lcd.setTextColor(BLUE, BLACK);
  M5.Lcd.setCursor(10, 195);
  M5.Lcd.print("BLE:");
  M5.Lcd.setCursor(34, 195);
  if (deviceConnected)
  {
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.print("Device connected.");
  }
  else
  {
    M5.Lcd.fillRect(34, 195, 320, 10, BLACK);
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.print("No connection.");
  }

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 205);
  M5.Lcd.print("PPG BSL:");
  M5.Lcd.setCursor(58, 205);
  M5.Lcd.print(DATA.ppgBSL);
}

long dataSimulateTimer;
void loop()
{
  if (deviceConnected)
    ledBleConnected();
  else
    ledBleNoConnection();

  if (MODE == Modes::BLE_ONLY)
  {
    bleLCD(); // debug BLE information to LCD
    bleRun(); // general BLE activity
    if (deviceConnected && millis() > dataSimulateTimer + 800)
    {
      simulateDataChanges();
      dataSimulateTimer = millis();
    }
    return;
  }

  bleRun(); // general BLE activity

  ppgCollectSamples(); // collects samples from buffer if available
  ppgCalc();           // applies algorithm for heart rate and spO2
  drawGraphPPG();

  if (millis() - timeStartGSR > 200) // limits to 5 Hz
  {
    gsrRun();
    drawGraphGSR();
    timeStartGSR = millis(); // resets timer

#ifdef DEBUG_GSR
    Serial.print("[DEBUG] GSR was run: ");
    Serial.println((int)(timeStartGSR));
#endif // DEBUG_GSR
  }

  if (millis() - timeStartRecord > 100)
  {
    sdRecord();
    timeStartGSR = millis(); // resets timer
  }

  drawLcdSensorValues();
  drawLcdBleStatus();
}