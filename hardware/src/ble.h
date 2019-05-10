#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE2904.h>
#include "data.h"

#ifndef ble_h
#define ble_h

/*
 * UUIDs specified by the Bluetooth GATT Specification: 
 * https://www.bluetooth.com/specifications/gatt
 *
 * For custom UUIDs, see the following for generating: 
 * https://www.uuidgenerator.net/
 */
// Device Information Service (official)
#define SERVICE_UUID_DEVICE_INFORMATION "0000180A-0000-1000-8000-00805f9b34fb"
// Firmware Revision (official)
#define CHARACTERISTIC_UUID_FIRMWARE_REV "00002A26-0000-1000-8000-00805f9b34fb"

// PPG Sensor (custom)
#define SERVICE_UUID_PPG "1a632076-8702-41b9-bcff-ea119ae68a69"
// Body Sensor Location(official)
#define CHARACTERISTIC_UUID_BSL "00002A38-0000-1000-8000-00805f9b34fb"
// Heart Rate Measurement (official)
#define CHARACTERISTIC_UUID_HR "00002A37-0000-1000-8000-00805f9b34fb"
// Interbeat Interval (custom)
#define CHARACTERISTIC_UUID_IBI "847dc27a-00f2-4c99-aebf-5eacea5474b4"
// SpO2 level of blood (custom)
#define CHARACTERISTIC_UUID_SPO2 "ef4684bb-c958-40df-90be-5eaa65e07948"

// GSR sensor (custom)
#define SERVICE_UUID_GSR "720f8954-ace5-41f7-acec-113b274bc54f"
// Skin Conductance Level (custom)
#define CHARACTERISTIC_UUID_SCL "3f18d911-bffd-4236-b5fc-94c9bf27d0e8"
// Non-specific Skin Conductance Response (custom)
#define CHARACTERISTIC_UUID_NSSCR "6bb32e9e-41fd-4abc-8089-f24dbe18aa61"
// Event-related Skin Conductance Response (custom)
#define CHARACTERISTIC_UUID_ERSCR "12d786f3-8528-43e4-b5f9-f7115db16004"

// #define  "7d7ca8d5-e2b0-40f2-8d84-eb05a1773bfa"

BLEServer *pServer = NULL;
BLEService *pServicePPG = NULL;
BLEService *pServiceGSR = NULL;
BLEService *pServiceDevInfo = NULL;
BLECharacteristic *pCharacteristicPPG_HR = NULL;
BLECharacteristic *pCharacteristicPPG_IBI = NULL;
BLECharacteristic *pCharacteristicPPG_SPO2 = NULL;
BLECharacteristic *pCharacteristicPPG_BSL = NULL;
BLECharacteristic *pCharacteristicGSR_SCL = NULL;
BLECharacteristic *pCharacteristicGSR_BSL = NULL;
BLECharacteristic *pCharacteristicFR = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class customServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

/*
 * Initialises the BLE service and configures it appropriately. Call once.
 */
void bleInit(String deviceName)
{
    /* 
     * Create and name the BLE Device
     */
    BLEDevice::init(deviceName.c_str());

    /* 
     * Create the BLE Server and assign custom callbacks
     */
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new customServerCallbacks());

    /*
     * Create the BLE Service
     */
    pServicePPG = pServer->createService(SERVICE_UUID_PPG);
    pServiceGSR = pServer->createService(SERVICE_UUID_GSR);
    pServiceDevInfo = pServer->createService(SERVICE_UUID_DEVICE_INFORMATION);

    /*
     * Create a BLE Characteristics for each service
     */
    pCharacteristicPPG_HR = pServicePPG->createCharacteristic(
        CHARACTERISTIC_UUID_HR,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristicPPG_IBI = pServicePPG->createCharacteristic(
        CHARACTERISTIC_UUID_IBI,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristicPPG_SPO2 = pServicePPG->createCharacteristic(
        CHARACTERISTIC_UUID_SPO2,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristicPPG_BSL = pServicePPG->createCharacteristic(
        CHARACTERISTIC_UUID_BSL,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristicGSR_SCL = pServiceGSR->createCharacteristic(
        CHARACTERISTIC_UUID_SCL,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristicGSR_BSL = pServiceGSR->createCharacteristic(
        CHARACTERISTIC_UUID_BSL,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    pCharacteristicFR = pServiceDevInfo->createCharacteristic(
        CHARACTERISTIC_UUID_FIRMWARE_REV,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    /*
     * All characterstics with the the Notify property must also have the CCC 
     * (Client Characteristic Configuration) descriptor.
     * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
     */
    pCharacteristicPPG_HR->addDescriptor(new BLE2902());
    pCharacteristicPPG_IBI->addDescriptor(new BLE2902());
    pCharacteristicPPG_SPO2->addDescriptor(new BLE2902());
    pCharacteristicGSR_SCL->addDescriptor(new BLE2902());
    pCharacteristicPPG_BSL->addDescriptor(new BLE2902());
    pCharacteristicGSR_BSL->addDescriptor(new BLE2902());
    pCharacteristicFR->addDescriptor(new BLE2902());

    /* 
     * Assign any additional descriptors to characteristics
     */
    BLE2904 *cpfd = new BLE2904();
    cpfd->setFormat(6);
    pCharacteristicPPG_IBI->addDescriptor(cpfd);

    /* 
     * Set some initial characteristic values
     */
    pCharacteristicFR->setValue(DATA.FIRMWARE_REVISION.c_str());
    pCharacteristicPPG_BSL->setValue(&DATA.ppgBSL, 1); // 1 byte
    pCharacteristicGSR_BSL->setValue(&DATA.gsrBSL, 1);

    /* 
     * Start the service and then advertise
     */
    pServicePPG->start();
    pServiceGSR->start();
    pServiceDevInfo->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_PPG);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting for a client connection...");
}

int printState = 0;
/*
 * Displays debug information on screen for BLE.
 */
void bleLCD()
{
    M5.Lcd.setTextSize(2); // 20 px high
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.print("BLE Status:");

    if (deviceConnected && printState == 0)
    {
        M5.Lcd.fillRect(140, 30, 320, 20, BLACK);
        printState = 1;
    }
    if (!deviceConnected && printState == 1)
    {
        M5.Lcd.fillRect(140, 30, 320, 20, BLACK);
        printState = 0;
    }

    if (deviceConnected)
        M5.Lcd.fillRect(140, 50, 320, 20 * 4, BLACK);

    M5.Lcd.setCursor(140, 30);
    if (deviceConnected && printState == 1)
    {
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.print("Connected.");
    }
    if (!deviceConnected && printState == 0)
    {
        M5.Lcd.setTextColor(RED);
        M5.Lcd.print("No connection.");
    }

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.print("Heart rate: ");
    M5.Lcd.setTextColor(MAGENTA);
    M5.Lcd.setCursor(140, 50);
    M5.Lcd.print(DATA.heartRate);

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(0, 70);
    M5.Lcd.print("IBI: ");
    M5.Lcd.setTextColor(MAGENTA);
    M5.Lcd.setCursor(140, 70);
    M5.Lcd.print(DATA.interbeatInterval);

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(0, 90);
    M5.Lcd.print("SpO2: ");
    M5.Lcd.setTextColor(MAGENTA);
    M5.Lcd.setCursor(140, 90);
    M5.Lcd.print(DATA.spo2);

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(0, 110);
    M5.Lcd.print("SCL: ");
    M5.Lcd.setTextColor(MAGENTA);
    M5.Lcd.setCursor(140, 110);
    M5.Lcd.print(DATA.scl);

    M5.Lcd.setTextColor(RED);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 150);
    M5.Lcd.print("Note: this data is simulated.");
}

long lastStaticValueUpdate;
long lastDynamicValueUpdate;
/*
 * When connected, will send notifications of data to client.
 * Handles connecting/disconnect activity.
 */
void bleRun()
{
    if (deviceConnected)
    {
        if (millis() > lastStaticValueUpdate + 10000) // 10 sec update
        {
            // send notification for all static values
            pCharacteristicFR->notify();
            pCharacteristicPPG_BSL->notify();
            pCharacteristicGSR_BSL->notify();
            lastStaticValueUpdate = millis();
        }

        if (millis() > lastDynamicValueUpdate + 1000) // 1 sec update
        {
            pCharacteristicPPG_HR->setValue(&DATA.heartRate, 1);
            pCharacteristicPPG_IBI->setValue(DATA.interbeatInterval);
            pCharacteristicPPG_SPO2->setValue(DATA.spo2);
            pCharacteristicGSR_SCL->setValue(DATA.scl);

            pCharacteristicPPG_HR->notify();
            pCharacteristicPPG_IBI->notify();
            pCharacteristicPPG_SPO2->notify();
            pCharacteristicGSR_SCL->notify();

            lastDynamicValueUpdate = millis();
        }

        // bluetooth stack will go into congestion if too many packets are sent.
        delay(5); // ensures loop is delayed.
    }

    if (!deviceConnected && oldDeviceConnected) // disconnecting
    {
        // give the bluetooth stack the chance to get things ready
        delay(500);
        pServer->startAdvertising(); // restart advertising
        Serial.println("BLE: start advertising");
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected) // connecting
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

#endif