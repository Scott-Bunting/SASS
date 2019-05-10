/********************************************************
*
* Project: MAXREFDES117#
* Filename: RD117_ARDUINO.ino
* Description: This module contains the Main application for the MAXREFDES117 example program.
*
* Revision History:
*\n 1-18-2016 Rev 01.00 GL Initial release.
*\n 12-22-2017 Rev 02.00 Significantlly modified by Robert Fraczkiewicz
*\n 08-22-2018 Rev 02.01 Added conditional compilation of the code related to ADALOGGER SD card operations
*
* --------------------------------------------------------------------
*
* This code follows the naming conventions:
*
* char              ch_pmod_value
* char (array)      s_pmod_s_string[16]
* float             f_pmod_value
* int32_t           n_pmod_value
* int32_t (array)   an_pmod_value[16]
* int16_t           w_pmod_value
* int16_t (array)   aw_pmod_value[16]
* uint16_t          uw_pmod_value
* uint16_t (array)  auw_pmod_value[16]
* uint8_t           uch_pmod_value
* uint8_t (array)   auch_pmod_buffer[16]
* uint32_t          un_pmod_value
* int32_t *         pn_pmod_value
*
* ------------------------------------------------------------------------- */

#ifndef ppg_h
#define ppg_h

#include <M5Stack.h>
#include <Wire.h>
#include <SPI.h>
#include "algorithm_by_RF.h"
#include "max30102.h"
#include "Deque.h"
#include "data.h"

Deque<int> ppgDeque;

// #define DEBUG_PPG // Uncomment for debug output to the Serial stream
// #define showRed

// Interrupt pin
const byte oxiInt = 5; // pin connected to MAX30102 INT
volatile bool ISRRan = false;
volatile int interruptCounter = 0;
int bufferIncrement = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

uint32_t elapsedTime, timeStart;
uint32_t aun_ir_buffer[BUFFER_SIZE];  //infrared LED sensor data
uint32_t aun_red_buffer[BUFFER_SIZE]; //red LED sensor data
float old_n_spo2;                     // Previous SPO2 value
uint8_t uch_dummy, k;
int bound = 1000;

// Variables from previous loop function
float n_spo2, ratio, correl; //SPO2 value
int8_t ch_spo2_valid;        //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate;        //heart rate value
int8_t ch_hr_valid;          //indicator to show if the heart rate calculation is valid
int32_t i;
char hr_str[10];
long graphPos;
float raw_ir, raw_red;

void ppgBufferProcess()
{
  // read from MAX30102 FIFO
  maxim_max30102_read_fifo((aun_red_buffer + bufferIncrement), (aun_ir_buffer + bufferIncrement));
  bufferIncrement = (bufferIncrement + 1) % BUFFER_SIZE;
  raw_ir = raw_ir_read(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &n_heart_rate);
  raw_red = raw_red_read(BUFFER_SIZE, aun_red_buffer, &n_spo2, &n_heart_rate);

#ifdef showRed
  if (raw_red < bound && raw_red > -bound) // If IR reflectance values are consistent with HR variance
  {
    graphPos = map(raw_red, -bound, bound, 200, 0);
    graphPos = floor(graphPos);
    ppgDeque.popHead();
    ppgDeque.pushTail(graphPos);
  }
#else  // showRed
  if (raw_ir < bound && raw_ir > -bound) // If IR reflectance values are consistent with HR range
  {
    graphPos = map(raw_ir, -bound, bound, 140, 0);
    graphPos = floor(graphPos);
    ppgDeque.popHead();
    ppgDeque.pushTail(graphPos + 30);
  }
#endif //showRed
#ifdef DEBUG_PPG
  Serial.println("Interrupt Loop");
  Serial.print(aun_red_buffer[i], DEC);
  Serial.print(F("\n"));
  Serial.println((float)(raw_ir));
  Serial.println((float)(raw_red));

  Serial.print(i, DEC);
  Serial.print(F("\t"));
  Serial.print(aun_red_buffer[i], DEC);
  Serial.print(F("\t"));
  Serial.print(aun_ir_buffer[i], DEC);
  Serial.println("");
#endif // DEBUG_PPG
}

/* 
 * Collect samples from MAX30102 when there is data to process.  
 * Heart rate and SpO2 are calculated every ST seconds.
 */
void ppgCollectSamples()
{

  if (interruptCounter > 0)
  {
    ppgBufferProcess();
    portENTER_CRITICAL(&mux);
    interruptCounter--;
    portEXIT_CRITICAL(&mux);

#ifdef DEBUG_PPG
    Serial.print("[DEBUG] interruptCounter: ");
    Serial.println((int)(interruptCounter));
    Serial.print("[DEBUG] bufferIncremet: ");
    Serial.println((int)(bufferIncrement));
#endif // DEBUG_PPG
  }
}

/* 
 * Interrupt handler to trigger reading of FIFO
 */
void IRAM_ATTR handleInterrupt()
{
  portENTER_CRITICAL(&mux);
  interruptCounter++;
  portEXIT_CRITICAL(&mux);
}

void ppgInit()
{
  pinMode(oxiInt, INPUT_PULLUP); //pin G5 connects to the interrupt output pin of the MAX30102
  maxim_max30102_reset();        //resets the MAX30102
  delay(1000);

  maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_dummy); //Reads/clears the interrupt status register
  maxim_max30102_init();                                  //initialize the MAX30102
  old_n_spo2 = 0.0;

  uch_dummy = Serial.read();

  Serial.print(F("Time[s]\tSpO2\tHR\tClock\tRatio\tCorr"));
  Serial.println("");

  attachInterrupt(digitalPinToInterrupt(oxiInt), handleInterrupt, FALLING);

  ppgDeque.setLimit(140);
  for (int i = 0; i < 140; i++)
    ppgDeque.pushTail(100);
}

void ppgCalc()
{
  //calculate heart rate and SpO2 after BUFFER_SIZE samples (ST seconds of samples) using Robert's method
  rf_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid, &ratio, &correl);

  // update globals
  if (n_heart_rate > 0)
  {
    if (DATA.heartRate == 0) DATA.heartRate = n_heart_rate;
    else if (DATA.heartRate < 60 && abs(n_heart_rate - DATA.heartRate) < 40) DATA.heartRate = n_heart_rate;
    else if (DATA.heartRate > 59 && DATA.heartRate < 99 && abs(n_heart_rate - DATA.heartRate) < 10) DATA.heartRate = n_heart_rate;
    else if (DATA.heartRate > 100 && abs(n_heart_rate - DATA.heartRate) < 40) DATA.heartRate = n_heart_rate; 
  }
  if (n_spo2 > 95) DATA.spo2 = n_spo2;

  // millis_to_hours(elapsedTime,hr_str); // Time in hh:mm:ss format
  elapsedTime = millis() - timeStart;
  elapsedTime /= 1000; // Time in seconds

#ifdef DEBUG_PPG
  Serial.println("--RF--");
  Serial.print(elapsedTime);
  Serial.print("\t");
  Serial.print(n_spo2);
  Serial.print("\t");
  Serial.print(n_heart_rate, DEC);
  Serial.print("\t");
  Serial.println(hr_str);
  Serial.println("------");
#endif // DEBUG_PPG

  if (ch_hr_valid && ch_spo2_valid)
  {
    Serial.print(elapsedTime);
    Serial.print("\t");
    Serial.print(n_spo2);
    Serial.print("\t");
    Serial.print(n_heart_rate, DEC);
    Serial.print("\t");

    Serial.print(hr_str);
    Serial.print("\t");
    Serial.print(ratio);
    Serial.print("\t");
    Serial.print(correl);
    Serial.println("");
  }
}

#endif // ppg_h