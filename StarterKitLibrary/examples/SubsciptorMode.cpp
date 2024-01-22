#include <Arduino.h>
#include <StarterKitNB.h>

StarterKitNB sk;


// Definitions

String ClientIdTB = "clientID";
String usernameTB = "username";
String passwordTB = "password";

String apn_entel = "insertyourap.here.cl"; //private apn


void setup() {

  sk.Setup(true);   // Se setea la configuracion para la libreria
  delay(1000);
  sk.Connect(apn_entel);  // Se conecta a NB
  delay(1000);
  sk.ConnectBroker(ClientIdTB, usernameTB, passwordTB); 
  delay(1000);
  sk.SubscribeMQTT();
  delay(1000);
  
  
}

void loop() {
  sk.WaitMessageMQTT();

  if (!sk.ConnectionStatus()) // Si no hay conexion a NB
  {
    sk.Reconnect(apn_entel);  // Se intenta reconecta
  }
  
  if (!sk.SubscriptionStatus)
  {
    sk.SubscribeMQTT();
  }
  
}
