#include "data.h"

struct DataStruct DATA; // global data object
int hrDelta = 1;
int ibiDelta = 1;
float spo2Delta = 0.2;
int sclDelta = 45;
void simulateDataChanges()
{
    if (DATA.heartRate <= 60)
        hrDelta = -hrDelta;
    if (DATA.heartRate >= 70)
        hrDelta = -hrDelta;
    DATA.heartRate += hrDelta;
    DATA.heartRate = constrain(DATA.heartRate, 60, 70);

    if (DATA.interbeatInterval <= 70)
        ibiDelta = -ibiDelta;
    if (DATA.interbeatInterval >= 80)
        ibiDelta = -ibiDelta;
    DATA.interbeatInterval += ibiDelta;
    DATA.interbeatInterval = constrain(DATA.interbeatInterval, 70, 80);

    if (DATA.spo2 <= 97)
        spo2Delta = -spo2Delta;
    if (DATA.spo2 >= 99)
        spo2Delta = -spo2Delta;
    DATA.spo2 += spo2Delta;
    DATA.spo2 = constrain(DATA.spo2, 97, 99);

    if (DATA.scl <= 0)
        sclDelta = -sclDelta;
    if (DATA.scl >= 4095)
        sclDelta = -sclDelta;
    DATA.scl += sclDelta;
    DATA.scl = constrain(DATA.scl, 0, 4095);
}