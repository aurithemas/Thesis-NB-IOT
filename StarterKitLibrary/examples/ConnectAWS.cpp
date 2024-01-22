#include <Arduino.h>
#include <StarterKitNB.h>

StarterKitNB sk;


// Definitions

// Message
String object = "alarm";
String value = "0";
String msg = "{\"" + object + "\":" + value + "}";

// Broker info
String broker = "IPamazon.amazonaws.com";
int port = 8883;
String topic = "test";
String ClientIdTB = "StarterKitIoT";
String usernameTB = "";
String passwordTB = "";

String apn_entel = "insertyourapn.here.cl";

// Files info
String nameCacert = "AmazonRootCA1.pem";
String nameClientCert = "-certificate.pem.crt";
String nameClientKey = "-private.pem.key";

String cacert = "content cacert";
String clientcert = "content clientcert";
String clientkey = "content clientkey";

int lenCacert = cacert.length();
int lenClientcert = clientcert.length();
int lenClientkey = clientkey.length();


void setup() {

  sk.Setup(true);   
  delay(1000);  
  
  sk.DeleteFile(nameCacert);
  sk.UploadFile(nameCacert, cacert, lenCacert, 10);

  sk.DeleteFile(nameClientCert);
  sk.UploadFile(nameClientCert, clientcert, lenClientcert, 10);
  
  sk.DeleteFile(nameClientKey);
  sk.UploadFile(nameClientKey, clientkey, lenClientkey, 10);

  
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
    sk.ConnectBroker(ClientIdTB, usernameTB, passwordTB, 0, broker, port, true, nameCacert, nameClientCert, nameClientKey);
    delay(2000);
  }

  sk.SendMessage(msg, topic);
  delay(2000);

}