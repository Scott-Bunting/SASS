/*
    Central Data Structure for firmware
    Author: Benedict Greenberg (https://github.com/nebbles)
    Date: 28 February 2019
    
    Defines central data structure for processed data that is handled by BLE 
    module.
*/
#ifndef data_h
#define data_h
#include <M5Stack.h>

struct DataStruct
{
    String FIRMWARE_REVISION = "";
    // Body Sensor Location of PPG. 0=Other, 1=Chest, 2=Wrist, 3=Finger, 4=Hand, 5=Ear Lobe, 6=Foot.
    uint8_t ppgBSL = 0;
    // 8 bit value in beats per minute
    uint8_t heartRate = 0;
    // 16 bit value in milliseconds
    uint16_t interbeatInterval = 0;
    // 32 bit float as percentage
    float spo2 = 0.0;

    // Body Sensor Location of PPG. 0=Other, 1=Chest, 2=Wrist, 3=Finger, 4=Hand, 5=Ear Lobe, 6=Foot.
    uint8_t gsrBSL = 0;
    // Value 0-255 in ??units
    uint16_t scl = 0;
    // // Value 0-255 in ??units
    // uint8_t ns_scr = 0;
    // // Value 0-255 in ??units
    // uint8_t er_scr = 0;
};
extern struct DataStruct DATA; // global data object

void simulateDataChanges();

#endif