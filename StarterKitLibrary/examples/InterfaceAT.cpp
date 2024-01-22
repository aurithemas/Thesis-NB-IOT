#include <Arduino.h>
#include <StarterKitNB.h>

StarterKitNB sk;

// Definitions


void setup() {
  // put your setup code here, to run once:

  sk.Setup();

}

void loop() {
  // put your main code here, to run repeatedly:
  sk.SerialComm();
}