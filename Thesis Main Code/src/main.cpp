#include <Arduino.h>
#include <StarterKitNB.h>
#include <time.h>
#include <WiFi.h>
#include <Wire.h>
#include "rak1901.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
//Calibracion Mics
#include "MICS6814.h"
#include "SPI.h"

#include "driver/adc.h"
#include <Adafruit_ADS1x15.h>


//BATERIA 


      // AO esta en el pin 36
#define PIN_VBAT WB_A0    ////Lee la bateria restante del pin Analogico A0, el cual no esta siento utilizado

#define VBAT_MV_PER_LSB (0.85449218705F) // 3.5V ADC range and 12 - bit ADC resolution = 3000mV / 4096
#define VBAT_DIVIDER_COMP (1.73)      // Compensation factor for the VBAT divider, depend on the board
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

//analog pins
Adafruit_ADS1115 ads;

const float multiplier = 0.1875F;

uint32_t vbat_pin = PIN_VBAT;

rak1901 rak1901;
int loopCounter = 0;  

RTC_DATA_ATTR int bootCount = 0;    //counter for boot counts

int16_t adc0, adc1, adc2, adc3;
float volts0, volts1, volts2, volts3;
 

//Libreria para los gases


//Valores sin calibracion

float NH3Value = 0;        // value read from the pot
float COValue = 0;        // value read from the pot
float NO2Value = 0;        // value read from the pot

//valores calibrados
float NH3ValueCal = 0;        // value read from the pot
float COValueCal = 0;        // value read from the pot
float  NO2ValueCal = 0;        // value read from the pot

float ABat = 0;

//Libreria temperatura declaracion

//MAX voltage
const float max_volts = 5.0;        // Maximum voltage of ADC
const float max_analog_steps = 4095.0;        // Maximum number of steps in ADC, for arduino are 10 bits =1023 for ESP32 are 12 bits = 4095;
//// ESP32 is not able to distinguish 3.3 V from 3.2 V. You’ll get the same value for both voltages: 4095.
//Counter   
const float maxstepsbit= 265536.0;

// Adafruit variables
String msg = "";
const char *broker = "io.adafruit.com";
const char *port = "1883";

int ClientId = 0;
const char *passwordBroker = "aio_tHor755XFRWVYdG10LElbA2cPD83";
const char *AIO_USERNAME = "ArumeQueen";

//Topic and messages for adafruit feeds.
//Kit1 (Santiago)

String topicbat1 = "ArumeQueen/feeds/kit-1.bateria-k1";
String topicco1 = "ArumeQueen/feeds/kit-1.cok1";
String topicnh31 = "ArumeQueen/feeds/kit-1.nh3k1";
String topicno21 = "ArumeQueen/feeds/kit-1.no2k1";
String topicg1 = "ArumeQueen/feeds/kit-1.general-data-k1";

//Kit2 (Las condes)

String topicbat2 = "ArumeQueen/feeds/kit-2.bateria-k2";
String topicco2 = "ArumeQueen/feeds/kit-2.cok2";
String topicnoh32 = "ArumeQueen/feeds/kit-2.nh3k2";
String topicno22 = "ArumeQueen/feeds/kit-2.no2k2 ";
String topicg2 = "ArumeQueen/feeds/kit-2.general-data-k2";

// Messages for kits
String bat= "";
String co = "";
String nh3 = "";
String no2 = "";
String general = "";



StarterKitNB sk;

//Message NB
String banda = "";
String resp = "";
String Operador = "";
String IDSTR = "";
// NB connect
const char *apn = "m2m.entel.cl";
const char *user = "entelpcs";
const char *pass = "en  telpcs";
const char *band = "B28 LTE";


//hour for messages
String hora = "";
float NOO2 = 0; 
float temperature= 0;
float humidity = 0;


// Variables de tiempo
const char *ssid = "invitado";
const char *password = "49852731";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * (-3);
const int daylightOffset_sec = 3600;
const int startTimes[] = {155,300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734, 735, 736, 737, 738, 739, 740, 741, 742, 743, 744, 745, 746, 747, 748, 749, 13*60+55,1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078, 
}; // Horas de inicio en formato   24 horas, ¡DEBEN IR EN ORDEN! ( 00:01 se escribe como 1, el formato es hora *60 + minutos)
const int duration = 150;                                   // Duración en minutos de funcionamiento del loop
const char *time_zone = "CET-3CEST, M3.5.9;M10.5.0/3";

                               // Tiempo de calibración en minutos
const int t_calentamiento = 25*60-25; //min*60);                      Calentamiento en minutos
// Declaracion de funciones
void setClock();
int calculateRemainingTime();
void printCurrentTime(struct tm timeinfo);

// Variables
int remainingTime;

// Funciones


float readVBAT(void)
{
    float raw;

    // Get the raw 12-bit, 0..3000mV ADC value
    raw = analogRead(vbat_pin);
    delay(100);
    Serial.println("raw "+String(raw));

    return raw*REAL_VBAT_MV_PER_LSB;

}

uint8_t mvToPercent(float mvolts)
{
    if (mvolts < 3300)
        return 0;

    if (mvolts < 3600)
    {
        mvolts -= 3300;
        return mvolts / 28;
    }

    mvolts -= 3600;
    return 10 + (mvolts * 0.11F); // thats mvolts /6.66666666
}



void connectWiFi()
{
  WiFi.begin(ssid, password);
  Serial.printf("Connecting to %s WiFi Network ", ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println();
  Serial.println("You are connected to WiFi!");
  setClock(); 
}

void disconnectWiFi()
{
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setClock()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println();
  Serial.print("Waiting for NTP time sync: ");
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    yield();
    nowSecs = time(nullptr);
  }
  Serial.println("");

  struct tm timeinfo;
  localtime_r(&nowSecs, &timeinfo); // Use localtime_r instead of gmtime_r
  Serial.println();
  Serial.print("Current time (UTC-3): ");
  Serial.print(asctime(&timeinfo));
}



void printCurrentTime(struct tm timeinfo)
{
  if (bootCount == 0)
  {
    Serial.println("Everything is setup correctly, Welcome!");
    Serial.println();
  }
  else
  {
    // Adjust for UTC-3 timezone
    time_t adjustedTime = mktime(&timeinfo) + gmtOffset_sec; // Subtract 3 hours
    localtime_r(&adjustedTime, &timeinfo);
    Serial.print("Current time of the device: ");
    Serial.println((&timeinfo)); // Print in the default format
  }
}

void setup()
{ 
  pinMode(WB_LED1, OUTPUT);
  pinMode(WB_LED2, OUTPUT);
  sk.Setup(false);

  void EndGPS();
  sk.StopPSM();

  

  sk.UserAPN(apn, user, pass);
  sk.Connect(apn, band);

  Wire.begin();
  Serial.printf("RAK1901 init %s\r\n", rak1901.init() ? "Success" : "Fail");
  delay(1000);
  
  struct tm timeinfo;
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
  printCurrentTime(timeinfo);
  delay(1000);

  if (bootCount == 0)
  {
    connectWiFi();
    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    printCurrentTime(timeinfo);
    disconnectWiFi();
    delay(1000);
    Serial.print("Wake up ");

    remainingTime = calculateRemainingTime();
    disconnectWiFi();

    esp_sleep_enable_timer_wakeup(remainingTime* 1000000ULL);
    Serial.println("");
    bootCount++;
    Serial.println("Going to sleep...");
    delay(1000);
    esp_deep_sleep_start();

  }

  void Reset();
  void EndGPS();
  sk.StopPSM();
 

  pinMode(WB_LED1, OUTPUT);
  pinMode(WB_LED2, OUTPUT);

  ads.begin();

  ads.setDataRate(RATE_ADS1115_16SPS);

  Serial.println("=====================================");
  Serial.println("RAK19001 Battery test");
  Serial.println("=====================================");
  Serial.println("");
  // Get a single ADC sample and throw it away
  readVBAT();
  
  
  delay(2000);
  
  

}


void loop()
{
  sk.StopPSM();
  
  
  if (!sk.ConnectionStatus()) // Si no hay conexion a NB
  {
    pinMode(WB_LED1, OUTPUT);
    pinMode(WB_LED2, OUTPUT);

    sk.Reconnect(apn, band);  // Se intenta reconecta
    delay(2000);

  }

  struct tm timeinfo;
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
  unsigned long startMillis = millis(); 
   // Get the current time
  Serial.print("Going to sleep in: ");
  Serial.print(duration -(millis()/ 60000));
  Serial.println(" minutes.");
  delay(000);
  Serial.println("Calentando sensores");
  delay(1000);


  MICS6814 gas(ads.readADC_SingleEnded(1), ads.readADC_SingleEnded(2) , ads.readADC_SingleEnded(0));

  

  esp_sleep_enable_timer_wakeup(t_calentamiento* 1000000ULL);
  Serial.println("Going to sleep...");
  delay(1000);
  esp_light_sleep_start();
  delay(10000);

  ClientId = 12342512;
  IDSTR = String(ClientId);

  sk.ConnectBroker(IDSTR,AIO_USERNAME,passwordBroker,0,broker,1883);

  ClientId ++;
  Serial.print("Calibrating Sensor MICS6814");
  gas.calibrate();
  delay(1000);
  Serial.println("");
  Serial.println("Sensor Calibrated!");
 
  delay(1000);

  while (millis() - startMillis < duration * 60 * 1000) {
    //Calentamiento Sensores
    

    //Chequea la conexion dentro del tiempo que deberia estar despierto
    if (!sk.ConnectionStatus()) // Si no hay conexion a NB
  {
    pinMode(WB_LED1, OUTPUT);
    pinMode(WB_LED2, OUTPUT);
    return setup();  // Se intenta reconecta
    
  }

    time(&now);  // Update the current time
    localtime_r(&now, &timeinfo);
    printCurrentTime(timeinfo);
    

    
    //SENSORES
    // read the analog in value:
    float NH3Value = ads.readADC_SingleEnded(0);
    delay(100);
    float COValue  = ads.readADC_SingleEnded(1);
    delay(100);
    float NO2Value = ads.readADC_SingleEnded(2);;
    delay(100);
    
    

    //print the results to the Serial Monitor:
    Serial.print("Latest reading in volts, NH3 (a0): ");
    Serial.print(NH3Value *( 10.0 / 65536));
    Serial.print(" CO (a1): ");
    Serial.print(COValue *(10.0 / 65536));
    Serial.print(" NO2 (a2): ");
    Serial.print(NO2Value * (10.0 / 65536));
    Serial.println("");
    
    delay(100);


    hora = String(timeinfo.tm_hour)+":"+String(timeinfo.tm_min)+":"+String(timeinfo.tm_sec);

    //Datos NB
    resp = sk.bg77_at((char *)"AT+QNWINFO", 500, false);
    
    
    resp = sk.bg77_at((char *)"AT+COPS?", 500, false);
    Operador = resp.substring(resp.indexOf("\""), resp.indexOf("\"",resp.indexOf("\"")+1)+1);     // ENTEL PCS
    
    
    delay(190);  
    
    
    //Meassures after calibration 
    float NH3ValueCal = gas.measure(NH3);
  
    delay(100);
    float COValueCal = gas.measure(CO);
  
    delay(100);
    float NO2ValueCal = gas.measure(NO2);
    delay(100);

    float vbat_mv = readVBAT();
    
    // Convert from raw mv to percentage (based on LIPO chemistry)
    uint8_t vbat_per = mvToPercent(vbat_mv);

    // Display the results

    Serial.print("LIPO = ");
    Serial.print(vbat_mv);
    Serial.print(" mV (");
    Serial.print(vbat_per);
   
    ABat=analogRead(vbat_pin);

    delay(100);

    
    Serial.printf("===============timer:%d===============\r\n", loopCounter);
    delay(1000);
    //Check if sensor Rak1901 update data is work
    //And print the data
    if (rak1901.update()) {
        Serial.printf("Temperature = %.2f。C\r\n", rak1901.temperature());
        Serial.printf("Humidity = %.2f%%\r\n", rak1901.humidity());
    } else {
        Serial.println("Please plug in the sensor RAK1901 and Reboot");
    }
    
    
    
    temperature = (rak1901.temperature());
    humidity = (rak1901.humidity());
    
    
    volts0 = ads.computeVolts(NH3Value);
    
    delay(1000);
    volts1 = ads.computeVolts(COValue);
    
    delay(1000);
    volts2 = ads.computeVolts(NO2Value);
    delay(1000);
    

    


    // General msge 
    general= "las condes:"
    +String(volts2)
    +":"+String(volts1)
    +":"+String(volts0)
    +":"+String(ABat+(max_volts / max_analog_steps))
    +":"+String(temperature)
    +":"+String(humidity)
    +":"+String(NO2Value)
    +":"+String(COValue)
    +":"+String(NH3Value);

    Serial.println(general);
    sk.SendMessage(general,topicg2);
    

     //msje bateria
    
    Serial.println(vbat_per);
    sk.SendMessage(String(vbat_per),topicbat2);
   

    
    //msje CO
    co = COValueCal;
    Serial.println(co);
    sk.SendMessage(co,topicco2);

    
    
    
    //msje NH3
    nh3 = NH3ValueCal;
    Serial.println(nh3);
    sk.SendMessage(nh3,topicnoh32);
    
    

    
  
    //msje NO2
    no2 = (NO2ValueCal);
    Serial.println(no2);
    sk.SendMessage(no2,topicno22);
    
    
   
    
    // print the results to the Serial Monitor:
 

    
    //Meassures every 5 second
    
    delay(200); 
  }

  sk.DisconnectBroker();
  time(&now);  // Update the current time
  localtime_r(&now, &timeinfo);
  printCurrentTime(timeinfo);
  delay(1000);
  remainingTime = calculateRemainingTime();
  int hours = remainingTime / 3600;
  int minutes = (remainingTime % 3600) / 60;
  Serial.println("Going to sleep...");
  Serial.print("Wake up: ");
  Serial.print(hours);
  Serial.print("h ");
  Serial.print(minutes);
  Serial.print("m ");
  delay(1000);


  esp_sleep_enable_timer_wakeup(remainingTime * 1000000ULL);
  esp_deep_sleep_start();

}



int calculateRemainingTime()
{
  
  struct tm timeinfo;
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
  if (bootCount == 0){
    
    int currentTimeInMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    int closestStartTime = startTimes[0];
    int nextStartTime = startTimes[0];

    for (int i = 0; i < sizeof(startTimes) / sizeof(startTimes[0]); i++)
    {
      int currentStartTime = startTimes[i];
      int nextStartTimeCandidate = (i < sizeof(startTimes) / sizeof(startTimes[0]) - 1) ? startTimes[i + 1] : startTimes[0] + 24 * 60;

      if (currentStartTime <= currentTimeInMinutes && nextStartTimeCandidate > currentTimeInMinutes)
      {
        // The current time is within the range of the current start time and the next start time
        closestStartTime = currentStartTime;
        nextStartTime = nextStartTimeCandidate;
        break;
      }
    }

    int remainingTimeInMinutes = (currentTimeInMinutes < closestStartTime) ? closestStartTime - currentTimeInMinutes : nextStartTime - currentTimeInMinutes;

    int remainingTimeInSeconds = remainingTimeInMinutes * 60;

    Serial.print("remaining time: ");
    Serial.print(remainingTimeInSeconds / 3600);
    Serial.print("h ");
    Serial.print((remainingTimeInSeconds % 3600) / 60);
    Serial.print("m ");
    Serial.print(remainingTimeInSeconds % 60);
    Serial.println("s");

    return remainingTimeInSeconds;
  }
  else {

  int currentTimeInMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  int closestStartTime = startTimes[0] - gmtOffset_sec / 60;
  int nextStartTime = startTimes[0] - gmtOffset_sec / 60;
  

    for (int i = 0; i < sizeof(startTimes) / sizeof(startTimes[0]); i++)
    {
      int currentStartTime = startTimes[i] - gmtOffset_sec / 60;
      int nextStartTimeCandidate = (i < sizeof(startTimes) / sizeof(startTimes[0]) - 1) ? startTimes[i + 1] - gmtOffset_sec / 60 : startTimes[0] + 24 * 60;

      if (currentStartTime <= currentTimeInMinutes && nextStartTimeCandidate > currentTimeInMinutes)
      {
        closestStartTime = currentStartTime;
        nextStartTime = nextStartTimeCandidate;
        break;
      }
    }
  

    int remainingTimeInMinutes = (currentTimeInMinutes < closestStartTime) ? closestStartTime - currentTimeInMinutes : nextStartTime - currentTimeInMinutes;
    


    if ((nextStartTime - startTimes[0]) % 1440 == 0)
    {

      // Add 3 hours (3 * 3600 seconds) to the remaining time
      remainingTimeInMinutes += 180;
    }

    int remainingTimeInSeconds = remainingTimeInMinutes * 60;
    Serial.println("next start time fuera del if");
    delay(1000);
    Serial.println(nextStartTime);
    delay(1000);
    Serial.print("remaining time: ");
    Serial.print(remainingTimeInSeconds / 3600);
    Serial.print("h ");
    Serial.print((remainingTimeInSeconds % 3600) / 60);
    Serial.print("m ");
    Serial.print(remainingTimeInSeconds % 60);
    Serial.println("s");
    delay(1000);
    return remainingTimeInSeconds;
  }

} 