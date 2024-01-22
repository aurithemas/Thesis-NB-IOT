#include <Arduino.h>
#include <StarterKitNB.h>

StarterKitNB sk;


// Definitions

// Message
String object = "temperature";
String value = "37.8";
String msg = "{\"" + object + "\":" + value + "}";

// Thingsboard
String ClientIdTB = "yourclientID";
String usernameTB = "yourusername";
String passwordTB = "yourpassword";

String apn_entel = "insertyoutapn.here.cl"; //private apn
//String apn_entel = "auto"; //auto apn

int clientIdMQTT = 2;

void setup() {

  sk.Setup(true);   // Se setea la configuracion para la libreria
  delay(1000);
  sk.Connect(apn_entel);  // Se conecta a NB
  delay(1000);
}

void loop() {

  if (!sk.ConnectionStatus()) // Si no hay conexion a NB
  {
    sk.Reconnect(apn_entel);  // Se intenta reconecta
    delay(2000);
  }

  if (!sk.LastMessageStatus)  // Si no se ha enviado el ultimo mensaje
  {
    sk.ConnectBroker(ClientIdTB, usernameTB, passwordTB);  // Se conecta a ThingsBoard
    delay(2000);
  }

  sk.SendMessage(msg);    // Se envia el mensaje
  delay(2000);

}
