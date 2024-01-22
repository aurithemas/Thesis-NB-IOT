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

#define PIN_VBAT WB_A0
Adafruit_ADS1115 ads;
const float multiplier = 0.1875F;


// Adafruit variables
String msg = "";
const char *broker = "io.adafruit.com";
const char *port = "1883";

const char *ClientId = "21475674566546";
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


StarterKitNB sk;

//Message NB
String banda = "";
String resp = "";
String Operador = "";

// NB connect
const char *apn = "m2m.entel.cl";
const char *user = "entelpcs";
const char *pass = "en  telpcs";
const char *band = "B28 LTE";


//hour for messages
String hora = "";
float temperature= 0;
float humidity = 0;

void setup()
{ 
  Serial.begin(115200);
  sk.Setup(false);
  Wire.begin();

  

  pinMode(WB_LED1, OUTPUT);
  pinMode(WB_LED2, OUTPUT);

  sk.UserAPN(apn, user, pass);
  sk.Connect(apn, band);

  ads.begin();
 
  ads.setDataRate(RATE_ADS1115_8SPS);
}
void loop()
{
  
  pinMode(WB_LED1, OUTPUT);
  pinMode(WB_LED2, OUTPUT);
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;
 
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  
  volts0 = ads.computeVolts(adc0);  
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);


  Serial.println("-----------------------------------------------------------");
  Serial.print("AIN0 nh3: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
  Serial.print("AIN1co: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println("V");
  Serial.print("AIN2nO2: "); Serial.print(adc2); Serial.print("  "); Serial.print(volts2); Serial.println("V");
  Serial.print("AIN not used 3: "); Serial.print(adc3); Serial.print("  "); Serial.print(volts3); Serial.println("V");
  Serial.println("Bvat" +  (String(analogRead(PIN_VBAT))));
}


