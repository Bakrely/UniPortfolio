#include "paulvha_SCD30.h"
#include <esp_now.h>
#include <WiFi.h>
#include <KnxTpUart.h>

SCD30 airSensor;
boolean newbodyData = false;
float CO2 = 0;
float humidity = 0.0;
float temperature = 0.0;

typedef struct struct_message {
    int id;
    int heartRate;
    int SpO2;
    bool fallen;
    float bodyTemperature;
} struct_message;

struct_message bodyData;
KnxTpUart knx(&Serial, "1.1.25");
typedef enum {Idle= 0, getBodyData,sendBodyData,getRoomData,sendRoomData} State;
State currentState = Idle; 

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&bodyData, incomingData, sizeof(bodyData));
    newbodyData == true;
}

void setup() {
    SCD30WIRE.begin();
    knx.uartReset();
    WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station
    Serial.begin(9600);
    if (esp_now_init() != ESP_OK) {Serial.println("Error initializing ESP-NOW");while(1);} // Init ESP-NOW
    esp_now_register_recv_cb(OnDataRecv); // Once ESPNow is successfully Init, we will register for recv CB to get recv packer info
    if (! airSensor.begin(SCD30WIRE))
    {
        Serial.println(F("The SCD30 did not respond. Please check wiring."));while(1);
    }
    Serial.println("SCD30 Reading...");
    
}


void loop() {
    Automat();
}

void Automat(){
    switch (currentState)
    {
        case Idle:
            if (airSensor.dataAvailable())
                currentState = getRoomData;
            else if (newbodyData == true)
                currentState = getBodyData;
            break;
        case getRoomData:
            currentState = sendRoomData;
            break;
        case getBodyData:
            if(newbodyData == true)
            {
                newbodyData = false;
                currentState = sendBodyData;
            }
            else currentState = Idle;
            break;
        case sendRoomData:
            currentState = getBodyData;
            break;
        case sendBodyData:
            currentState = Idle;
            break;
        default:
            Serial.println("How does this even happen");
            break;
    }
    
    switch (currentState)
    {
        case Idle:
            break;
        case getRoomData:
            temperature = airSensor.getTemperature();
            humidity = airSensor.getHumidity();
            CO2 = airSensor.getCO2();
            break;
        case getBodyData:
            break;
        case sendRoomData:
            sendroomDatatoKNX();
            delay(500);
            break;
        case sendBodyData:
            sendbodyDatatoKNX();
            delay(500);
            break;
        default:
            Serial.println("How does this even happen");
            break;
    }
}

void sendroomDatatoKNX() {
    bool success = knx.groupWrite14ByteText("1/1/24","Room Data");
    success = knx.groupWrite2ByteFloat("1/1/25",CO2);
    success = knx.groupWrite2ByteFloat("1/1/26",humidity);
    success = knx.groupWrite2ByteFloat("1/1/27",temperature);
}

void sendbodyDatatoKNX() {
    bool success = knx.groupWrite14ByteText("1/1/28","Body Data");
    success = knx.groupWrite2ByteInt("1/1/29",id);
    success = knx.groupWrite2ByteInt("1/1/30",heartRate);
    success = knx.groupWrite2ByteInt("1/1/31",SpO2);
    success = knx.groupWriteBool("1/1/32",fallen);
    success = knx.groupWrite2ByteFloat("1/1/33",bodyTemperature);
}
