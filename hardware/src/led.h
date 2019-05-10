#ifndef led_h
#define led_h
#include <M5Stack.h>
#include "esp32_digital_led_lib.h"

strand_t m_sLeds;

void ledBar(int R, int G, int B, int M)
{
    if ((M < 0) || (M > 13))
        return;
    if (M == 11) // right
    {
        for (int i = 0; i < 5; i++)
        {
            m_sLeds.pixels[i] = pixelFromRGBW(R, G, B, 0);
        }
    }
    else if (M == 10) // left
    {
        for (int i = 5; i < 10; i++)
        {
            m_sLeds.pixels[i] = pixelFromRGBW(R, G, B, 0);
        }
    }
    else if (M == 12) // all
    {
        for (int i = 0; i < 10; i++)
        {
            m_sLeds.pixels[i] = pixelFromRGBW(R, G, B, 150);
        }
    }
    else
    {
        m_sLeds.pixels[M] = pixelFromRGBW(R, G, B, 0);
    }
    digitalLeds_updatePixels(&m_sLeds);
}

void ledInit()
{
    m_sLeds = {.rmtChannel = 0,
               .gpioNum = 15,
               .ledType = LED_WS2812B_V3,
               .brightLimit = 32,
               .numPixels = 10,
               .pixels = nullptr,
               ._stateVars = nullptr};

    pinMode(15, OUTPUT);   // LED GPIO
    digitalWrite(15, LOW); // set LED lows
    if (digitalLeds_initStrands(&m_sLeds, 1))
    {
        Serial.println("ERROR: Can't init LED driver()");
    }
    digitalLeds_resetPixels(&m_sLeds);
}

void ledsOff()
{
    ledBar(0, 0, 0, 12);
}

long timer;
long fadeTime = 50;
int ledVal = 0;
int fadeDir = 1;
void ledStartScreen()
{
    if (millis() > timer+fadeTime)
    {
        ledVal += fadeDir;
        if (ledVal >= 50) fadeDir = -fadeDir;
        if (ledVal <= 0) fadeDir = -fadeDir;
        
        ledBar(ledVal, 0, ledVal, 12);
        timer = millis(); // reset the timer
    }
}

int redVal = 20;
int fader = -1;
void ledBleNoConnection()
{
    fadeTime = 50;
    if (millis() > timer+fadeTime)
    {
        redVal += fader;
        if (redVal >= 20)
            fader = -fader;
        if (redVal <= 0)
            fader = -fader;

        // shift right side downwards (must update in reverse order)
        m_sLeds.pixels[4] = m_sLeds.pixels[3];
        m_sLeds.pixels[3] = m_sLeds.pixels[2];
        m_sLeds.pixels[2] = m_sLeds.pixels[1];
        m_sLeds.pixels[1] = m_sLeds.pixels[0];
        // shift left side downwards (must update in reverse order)
        m_sLeds.pixels[5] = m_sLeds.pixels[6];
        m_sLeds.pixels[6] = m_sLeds.pixels[7];
        m_sLeds.pixels[7] = m_sLeds.pixels[8];
        m_sLeds.pixels[8] = m_sLeds.pixels[9];
        // feed new value to top two
        m_sLeds.pixels[0] = pixelFromRGBW(redVal, 0, 0, 0);
        m_sLeds.pixels[9] = pixelFromRGBW(redVal, 0, 0, 0);
        digitalLeds_updatePixels(&m_sLeds);

        timer = millis(); // reset the timer
    }

    // M5.Lcd.setTextColor(WHITE, BLACK);
    // M5.Lcd.setTextSize(2);
    // M5.Lcd.setCursor(0, 220);
    // M5.Lcd.print(redVal);
}

int lLedVal = 10;
int rLedVal = 40;
int lFadeDir = 1;
int rFadeDir = -1;
void ledBleConnected()
{
    // ledBar(0, 20, 0, 12); // fixed value

    fadeTime = 50;
    if (millis() > timer + fadeTime)
    {
        lLedVal += lFadeDir;
        if (lLedVal >= 40)
            lFadeDir = -lFadeDir;
        if (lLedVal <= 10)
            lFadeDir = -lFadeDir;
        ledBar(0, lLedVal, 0, 11);
        
        rLedVal += rFadeDir;
        if (rLedVal >= 40)
            rFadeDir = -rFadeDir;
        if (rLedVal <= 10)
            rFadeDir = -rFadeDir;
        ledBar(0, rLedVal, 0, 10);

        timer = millis(); // reset the timer
    }
}

void ledRunTest()
{

    while (true)
    {
        // ledBar(num, 0, num, 12);
        // num = (num + 1) % 255;
        // delay(50);

        // ledStartScreen();
        ledBleConnected();

        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.setTextSize(4);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.print(lLedVal);

        // for (int l = 0; l < 10; l++)
        // {
        //     ledBar(255, 0, 0, l);
        //     delay(500);
        //     ledBar(0, 0, 0, l);
        //     delay(500);
        // }
    }
}

#endif