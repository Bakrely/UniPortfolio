#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Settings.h"
#include <WiFi.h>
#include "ESPNowW.h"
#include "time.h"
#include "heartRate.h"

bool setMessage ();
bool getTime();
bool fallen ();
void getSpO2andHR ();
void setOffset ();

bool getTime()
{
  struct tm timeInfo;
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin (SSIDTIME, PASSWORD);
  delay (5000);
    for (int i = 0; i<10; i++)
    {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      if(getLocalTime(&timeInfo))
      {
        Serial.println(&timeInfo, "%a, %B %d %Y %H:%M:%S");
        WiFi.disconnect(true);
        return true;
      }
      Serial.println("Obtaining online time"); 
      delay(200);
    }
    WiFi.disconnect(true);
    return false;
}


bool fallen ()
{
    accelandgyro.getMotion6(&acceleration[x],&acceleration[y],&acceleration[z],&gyroscope[x],&gyroscope[y],&gyroscope[z]); 

    acceleration[x]-=XACOFFSETVAL;acceleration[y]-=YACOFFSETVAL;acceleration[z]-=ZACOFFSETVAL; //Fine tuning of the acceleration data so they all are in the same range; easier to set threshhold for it
    
    if (abs(acceleration[x]) > FALLTHRESH || abs(acceleration[y]) > FALLTHRESH|| abs(acceleration [z]) >FALLTHRESH)
    {
        time(&refTimeMessage);
        return true;
    }
    else 
    {
        return setMessage();
    }
}

bool setMessage ()
{
  time(&currentTimeMessage);
  if ((currentTimeMessage - refTimeMessage) > SAVEDMESTIME && myData.fallen == true)
  {
    return false;
  }
  else if ((currentTimeMessage - refTimeMessage) < SAVEDMESTIME && myData.fallen == true)
  {
    return true;
  }
  else
  {
    return false; 
  }

}

void getSpO2andHR ()
{
  time(&startMeasureT);
  digitalWrite(ONBOARDLED,true);
  while ((currentMeasureT - startMeasureT)< MEASURETIME) 
   //buffer length of 100 stores 4 seconds of samples running at 25sps

  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();   
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
 
  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
  while (1)
  {
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }
    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample
    }
    
    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    time(&currentMeasureT);
  }
  myData.SpO2 = spo2;
  myData.heartRate = heartRate;
}

void setOffset ()
{
  accelandgyro.setXGyroOffset(XGYOFFSET);
  accelandgyro.setYGyroOffset(YGYOFFSET);               //sets the individual offsets for each Value
  accelandgyro.setZGyroOffset(ZGYOFFSET);
  accelandgyro.setXAccelOffset(XACOFFSET);  
  accelandgyro.setYAccelOffset(YACOFFSET);
  accelandgyro.setZAccelOffset(ZACOFFSET);
}

void setup()
{
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();  
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
  pinMode(ONBOARDLED, OUTPUT);
  WiFi.mode(WIFI_MODE_STA);
  //getTime();
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.add_peer(receiver_mac);
  Serial.println("Initializing I2C devices...");
  accelandgyro.initialize();
  while (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
  }
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
  particleSensor.enableDIETEMPRDY();
  Serial.println(accelandgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  setOffset();
}

void loop()
{
    time(&currentTimeMeasure);
    Serial.println(myData.fallen = fallen());
    getSpO2andHR ();
    ESPNow.send_message(receiver_mac, (uint8_t*)&myData,sizeof(myData));
    delay(500);
}


void Automat()
{
  time (&currentTimeMeasure);

switch (currentState)
{
case Idle:
  currentState = FallDetection;
  break;
case  FallDetection: 
  currentState = SendData;
  break;
case  SendData: 
  if ((currentTimeMeasure - refTimeMeasure) >  MEASUREINTERVAL)
  {
    currentState = RecordBodyData;
  }
  else
  {
    currentState = SendData;
  }
  break;
case SendFallData:

  break;
case  RecordBodyData: 
  
  currentState = SendData;
  break;
default:
  Serial.println("How does this even happen");
  break;
}

switch (currentState)
{
case Idle:
  break;
case  FallDetection: 
  myData.fallen = fallen();
  break;
case  SendData: 
  myData.temperature = particleSensor.readTemperature();
  ESPNow.send_message(receiver_mac, (uint8_t*)&myData,sizeof(myData));
  break;
case  RecordBodyData: 
  currentState = SendData;
  break;
case SendFallData:
  ESPNow.send_message(receiver_mac, (uint8_t*)&myData,sizeof(myData));
  break;
default:
  Serial.println("How does this even happen");
  break;
}



}
