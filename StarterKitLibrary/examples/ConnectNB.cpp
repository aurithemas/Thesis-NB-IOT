#include <Arduino.h>
#include <StarterKitNB.h>

StarterKitNB sk;

String apn_entel = "insertyourapn.here.cl"; 

void setup() {

  sk.Setup(true);   
  delay(1000);
  sk.Connect(apn_entel);  
  delay(1000);
  sk.DeviceCheck();   // Consulta sobre las caracteristicas actuales del dispositivo
  delay(1000);

}

void loop() {

  if (!sk.ConnectionStatus()) 
  {
    sk.Reconnect(apn_entel);  
    delay(2000);
  }
  delay(2000);

}
