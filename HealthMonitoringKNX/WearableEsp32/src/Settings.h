#define XGYOFFSET 117
#define YGYOFFSET -26
#define ZGYOFFSET -10
#define XACOFFSET 655   //655
#define YACOFFSET -212 //-212
#define ZACOFFSET 793 //
#define XACOFFSETVAL -1000 //655
#define YACOFFSETVAL 760 //-212
#define ZACOFFSETVAL 16900 //
#define PATIENT_ID 100
#define MAX_BRIGHTNESS 255
#define FALLTHRESH 9000          //threshold value for fall detection
#define PASSWORD "fuckoff1"
#define SSIDTIME "Flo"
#define SAVEDMESTIME 120  //time until fallen message is deleted
#define MEASUREINTERVAL  900 //time in seconds between two SpO2 and HR measurements
#define MEASURETIME 60 //time for evaluation of HR and SpO2
#define ONBOARDLED 13

typedef enum {x=0,y,z}level_index;
typedef enum {Idle= 0, FallDetection,SendData, RecordBodyData, SendFallData} State;
typedef struct struct_message {
  int id = PATIENT_ID; 
  int heartRate;
  int SpO2;
  bool fallen= false;
  float temperature; 
} struct_message;

MAX30105 particleSensor;
MPU6050 accelandgyro;
struct_message myData;

                               
time_t  currentTimeMessage;                           //Time related Stuff
time_t  refTimeMessage;                     
time_t  currentTimeMeasure;
time_t  refTimeMeasure;
time_t  startMeasureT;
time_t currentMeasureT; 
const long  gmtOffset_sec = 3600;               //3600 for GMT +1
const int   daylightOffset_sec = 3600;          //3600 for daylight saving time offset
const char* ntpServer = "pool.ntp.org";

State currentState = Idle; 
int16_t acceleration [3] = {0};         //x,y,z Value for acceleration
int16_t gyroscope [3]  = {0};           //x,y,z Value for Orientation 
float temperature;
bool fallFlag = false; 
byte ledBrightness = 254; //Options: 0=Off to 255=50mA
byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200 change to 100
int pulseWidth = 411; //Options: 69, 118, 215, 411 change back to 411
int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
int32_t bufferLength = 100; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid
uint8_t receiver_mac[] = {0x30, 0xaE, 0xa4, 0x7B, 0x69, 0x78};


uint8_t oxygenData [60]; 
uint8_t oxygenMean; 
uint16_t heartrateData [60];
uint16_t heartrateMean; 
 

