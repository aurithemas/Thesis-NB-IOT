#include <Arduino.h>
#include <StarterKitNB.h>

StarterKitNB sk;

// Definitions
String msg;

void setup() {
  // put your setup code here, to run once:

  sk.Setup();
  delay(500);
  sk.StartGPS();
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  msg = sk.PositionGPS();
  delay(1000);
  


}