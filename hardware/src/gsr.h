#ifndef gsr_h
#define gsr_h

#include <M5Stack.h>
#include "Deque.h"
#include "data.h"

#define DEBUG_GSR // use to enable debug functionality with GSR

const int GSR = 35;
int sensorValue = 0;
int gsr_average = 0;
int timeStartGSR;
int timeDelay;
int posInt;
Deque<int> gsrDeque;

/* 
 * Set up GSR related tasks.  
 */
void gsrInit()
{
    gsrDeque.setLimit(140);
    for (int i = 0; i < 140; i++) gsrDeque.pushTail(100);
}

/* 
 * Collect GSR data from sensor.  
 */
void gsrRun()
{
    long sum = 0;
    for (int i = 0; i < 10; i++) //Average the 10 measurements to remove the glitch
    {
        sensorValue = analogRead(GSR);
        sum += sensorValue;
        delay(5);
    }
    gsr_average = sum / 10;
    DATA.scl = gsr_average;

    #ifdef DEBUG_GSR
    Serial.print("[DEBUG] GSR Value: ");
    Serial.println((int)(gsr_average));
    #endif // DEBUG_GSR

    long pos = map(gsr_average, 0, 4095, 140, 0); // Mapping GSR values to screen height
    posInt = floor(pos);
    gsrDeque.popHead();
    gsrDeque.pushTail(posInt + 30);
}

#endif