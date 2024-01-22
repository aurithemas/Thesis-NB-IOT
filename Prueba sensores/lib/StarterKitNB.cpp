//
#include <StarterKitNB.h>


// Definitions
#define POWER WB_IO1
#define BG77_GPS_ENABLE WB_IO2

// Variables
char commandChar;
String commandString = "";

StarterKitNB::StarterKitNB()
{
    
}

StarterKitNB::~StarterKitNB()
{
    
}

// Funcion que configura inicialmente todo lo necesario para usar la libreria
// Return -> None
void StarterKitNB::Setup(bool LEDs)
{
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(POWER, OUTPUT);   // Para la comunicacion de comandos AT
  digitalWrite(POWER, LOW);

  pinMode(BG77_GPS_ENABLE, OUTPUT); // Para el uso de antena de GPS
  digitalWrite(BG77_GPS_ENABLE, 1);

  analogReadResolution(12);
  this -> LEDs = LEDs;    // Se incluye si el usuario desea el despliegue LEDs para la conexion
  this -> LastMessageStatus = false;    // Como no hay ultimo mensaje, se le asigna falso para empezar la conxeion a TB
  this -> SubscriptionStatus = false; 

  if (LEDs)   // Si se desea usar los leds para visualizar el estado de las conexiones
  {
    pinMode(WB_LED1, OUTPUT);  
    pinMode(WB_LED2, OUTPUT); 
  }

  while (bg77_at((char *)"AT", 500).indexOf("OK")<0) // Se espera hasta que responda la comunicacion serial
  {
    delay (1000);
  }
}

// Funcion que aplica un reset a los comandos AT configurados (No aplica a todos)
// Return -> None
void StarterKitNB::Reset()
{
  bg77_at((char *)"AT&F=0", 500); // Se envia el comando AT para volver la configuracion de fabrica
  delay(500);  
}

// Funcion predeterminada que permite la comunicacion serial para utilizar comandos AT al modulo BG77
// Return -> Respuesta del comando AT
String StarterKitNB::bg77_at(char *at, uint32_t timeout, bool print)
{

  String bg77_rsp = "";
  char tmp[256] = {0};
  int len = strlen(at);
  strncpy(tmp, at, len);
  uint32_t t = timeout;
  tmp[len] = '\r';
  Serial1.write(tmp);
  delay(10);
  while (t--)
  {
    if (Serial1.available())
    {
      bg77_rsp += char(Serial1.read());
    }
    delay(1);
  }

  if(print){
    Serial.println(bg77_rsp);
  }  

  return bg77_rsp;
}

// Funcion que permite escribir en la terminal comandos AT
// Return -> None
void StarterKitNB::SerialComm(uint32_t MaxTimeResponse)
{
  if (Serial.available())
  {
    commandChar = Serial.read();    // Guarda los caracteres escritos en el terminal
    commandString+=commandChar;     // Los une a un String
    if (commandChar == '\n')  // Si se presiona Enter, significa que se desea enviar el comando
    {
        commandString.remove(commandString.length()-1, 1);  // Se eliminar el Enter del String
        char commandBuf[commandString.length()];  // Se crea el buffer para el comando
        commandString.toCharArray(commandBuf, commandString.length());  // Se copia el String en el Buffer
        bg77_at(commandBuf, MaxTimeResponse);   // Se envia el comando
        commandString = "";   // Se resetea el Sring
        delay(200);
    }
  }
}

void StarterKitNB::UserAPN(String apn, String user, String password)
{

  String ingreso_apn = "AT+QICSGP=1,1,\""+apn+"\",\""+user+"\",\""+password+"\",3";   // Se crea el string del comando
  char ingreso_apn_char[ingreso_apn.length()+1];  // Se crea el buffer para el comando
  ingreso_apn.toCharArray(ingreso_apn_char, ingreso_apn.length()+1);    // Se copia el string en el buffer
  bg77_at(ingreso_apn_char, 500);   // Se envia el comando AT para la configuracion del APN
  delay(500);
  
}

// Funcion que permite conectar al dispositivo a NB
// Return -> None
void StarterKitNB::Connect(String apn, String band, String network)
{

  // Paso 1
  bg77_at((char *)"AT+QGPSCFG=\"priority\",1", 500);  // Da prioridad a la antena LTE
  delay(500);

  if (apn == "auto")  // Si se quiere configurar solo el APN
  {
    bg77_at((char *)"AT+CGDCONT=1,,", 500); // Si es vacio se configura automaticamente
    delay(500); //automatico
  }
  else    // Sino, se da un APN especifico
  {
    // bg77_at((char *)"AT+QICSGP=1,1,\"apn\",\"username\",\"username\",3", 500);
    String ingreso_apn = "AT+CGDCONT=1,\"IP\",\""+ String(apn) +"\"";   // Se crea el string del comando
    char ingreso_apn_char[ingreso_apn.length()+1];  // Se crea el buffer para el comando
    ingreso_apn.toCharArray(ingreso_apn_char, ingreso_apn.length()+1);    // Se copia el string en el buffer
    bg77_at(ingreso_apn_char, 500);   // Se envia el comando AT para la configuracion del APN
    delay(500);
    
  }

  if ((band.indexOf("B28 LTE")>= 0) && (band.indexOf("B2 LTE")>=0))
  {
    bg77_at((char *)"AT+QCFG=\"band\",F,8000002,8000002", 500); // Se configura la banda a la que trabajara 8000002
    delay(1000);
  }

  else if ((band.indexOf("B28 LTE")>= 0))
  {
    bg77_at((char *)"AT+QCFG=\"band\",F,8000000,8000000", 500);
    delay(1000);
  }

  else if ((band.indexOf("B2 LTE")>=0))
  {
    bg77_at((char *)"AT+QCFG=\"band\",F,2,2", 500);
    delay(1000);
  }

  else
  {
    bg77_at((char *)"AT+QCFG=\"band\",F,0,0", 500);
    delay(1000);
  }
  
  if (network.indexOf("NB") >= 0)
  {
    bg77_at((char *)"AT+QCFG=\"nwscanseq\",03", 500);
    delay(1000);
    bg77_at((char *)"AT+QCFG=\"iotopmode\",1", 500);
    delay(1000);
  }
  
  else if (network.indexOf("CAT") >= 0)
  {
    bg77_at((char *)"AT+QCFG=\"nwscanseq\",02", 500); 
    delay(1000);
    bg77_at((char *)"AT+QCFG=\"iotopmode\",0", 500);
    delay(1000);
  }
  
  else
  {
    bg77_at((char *)"AT+QCFG=\"nwscanseq\",0302", 500); 
    delay(1000);
    bg77_at((char *)"AT+QCFG=\"iotopmode\",2", 500);
    delay(1000);
  }
  
  

  bg77_at((char *)"AT+CFUN=1", 500);  // Se activa en maximo el nivel de funcionamiento del UE
  delay(1000);

  //Paso 2

  bg77_at((char *)"AT+COPS=0", 500);    // Selecciona el operador automaticamente
  delay(1000);

  bg77_at((char *)"AT+CEREG=4", 500);   // Permite que la conexion pueda aplicar PSM
  delay(1000);

  if (!ConnectionStatus()) // Si no hay conexion a NB
  {
    Reconnect(apn, band, network);  // Se intenta reconecta
  }
  
  

}

// Funcion que espera cierto tiempo para verificar si vuelta la conexion a NB, sino sucede se conecta nuevamente
// Return -> None
void StarterKitNB::Reconnect(String apn, String band, String network)
{
  int timeout = 90;   // Se consulta 90 veces si es que hay conexion a NB
  int delayBtimeout = 2000;   // 2 segundos entre cada consulta
  int i = 0;    // Contador para las consultas hechas
  while (bg77_at((char *)"AT+CEREG?",500).indexOf("+CEREG: 4,1") < 0) // Mientras no haya conexion
  {
    Serial.println("Checking connection...");

    if (i == timeout)   // Si se esta en el timeout
    {
      Serial.println("Trying connect...");
      bg77_at((char *)"AT+CFUN=0", 500);
      delay(10000);
      Connect(apn, band, network); // Se conecta nuevamente

      if (bg77_at((char *)"AT+CEREG?",500).indexOf("+CEREG: 4,1" < 0))  // Si no se logro conectar
      {
        Serial.println("Couldn't reconnect to network.");   // Se informa
        Serial.println("The device will be restart in 5 seconds.");
        delay(5000);
        ESP.restart();
      }
      else
      {
        Serial.println("Connected to network!");
        delay(1000);
        bg77_at((char*)"AT+CSQ?", 500);
        delay(1000);
        if (this -> LEDs)
        {
          digitalWrite(WB_LED1, HIGH); 
        }
        
        return ;
      }
      
    }
    
    else
    {
      delay(delayBtimeout);
      i++;
    }
  }
  Serial.println("This device is connected to network.");
}

// Funcion que entrega varia informacion sobre el estado actual del dispositivo
// Return -> None
void StarterKitNB::DeviceCheck()
{
  Serial.println("Checking Device Status");

  Serial.println("---------------------");
  Serial.println("---------------------");

  Serial.println("Device Information");
  Serial.println("Product Identification");
  delay(1);
  bg77_at((char *)"ATI", 500);
  delay(500);

  Serial.println("Manufacturer Information");
  delay(1);
  bg77_at((char *)"AT+GMI", 500);
  delay(500);

  Serial.println("Model Information");
  delay(1);
  bg77_at((char *)"AT+GMM", 500);
  delay(500);


  Serial.println("Firmware Version");
  delay(1);
  bg77_at((char *)"AT+GMR", 500);
  delay(500);

  Serial.println("IMEI Number");
  delay(1);
  bg77_at((char *)"AT+GSN", 500);
  delay(500);


  Serial.println("---------------------");
  Serial.println("---------------------");


  Serial.println("Connection Status");
  delay(1);
  bg77_at((char *)"AT+QCSCON?", 500);
  delay(500);


  Serial.println("APN Status");
  delay(1);
  bg77_at((char *)"AT+CGDCONT?", 500);
  delay(500);


  Serial.println("Operator, Band and Channel info");
  delay(1);
  bg77_at((char *)"AT+QNWINFO", 500);
  delay(500);

  Serial.println("---------------------");
  Serial.println("---------------------");
  Serial.println("Connection Check Finished");

}

// Funcion que verifica si se encuentra conectado el dispositivo a NB
// Return -> true si se encuentra conectado, false sino
bool StarterKitNB::ConnectionStatus()
{

  bool state;
  String response = bg77_at((char *)"AT+CEREG?", 500, false);

  if (response.indexOf("+CEREG: 4,1")>=0)
  {
    state = true;
    Serial.println("This device is connected to network.");
    if (this -> LEDs)
    {
      digitalWrite(WB_LED1, LOW);  
      delay(150);                      
      digitalWrite(WB_LED1, HIGH);   
      delay(150); 
      digitalWrite(WB_LED1, LOW);  
      delay(150);                      
      digitalWrite(WB_LED1, HIGH); 
    }
    
  }
  
  else
  {
    state = false;
    Serial.println("This device is not connected to network.");
    if (this -> LEDs)
    {
      digitalWrite(WB_LED1, LOW); 
    }
  }
  
  return state;
}

// Funcion que abre la red para el cliente y lo conecta a ThingsBoard
// Return -> None
void StarterKitNB::ConnectBroker(String clientIdBr, String usernameBr, String passwordBr, int clientIdMQTT, String broker, int port, bool SSL, String cacert, String clientCert, String clientKey, int MRT_OPEN, int MRT_CONN)
{

  if (SSL)
  {
    String atConfig = "AT+QMTCFG=\"ssl\","+String(clientIdMQTT)+",1,2";
    char atConfigBuf[atConfig.length()+1];
    atConfig.toCharArray(atConfigBuf, atConfig.length()+1);
    bg77_at(atConfigBuf, 500);

    String atcacert = "AT+QSSLCFG=\"cacert\",2,\""+cacert+"\"";
    char atcacertBuf[atcacert.length()+1];
    atcacert.toCharArray(atcacertBuf, atcacert.length()+1);
    bg77_at(atcacertBuf, 500);

    String atclientcert = "AT+QSSLCFG=\"clientcert\",2,\""+clientCert+"\"";
    char atclientcertBuf[atclientcert.length()+1];
    atclientcert.toCharArray(atclientcertBuf, atclientcert.length()+1);
    bg77_at(atclientcertBuf, 500);

    String atclientkey = "AT+QSSLCFG=\"clientkey\",2,\""+clientKey+"\"";
    char atclientkeyBuf[atclientkey.length()+1];
    atclientkey.toCharArray(atclientkeyBuf, atclientkey.length()+1);
    bg77_at(atclientkeyBuf, 500);
  
    bg77_at((char *)"AT+QSSLCFG=\"seclevel\",2,2", 500);
    bg77_at((char *)"AT+QSSLCFG=\"sslversion\",2,4", 500);
    bg77_at((char *)"AT+QSSLCFG=\"ciphersuite\",2,0XFFFF", 500);
    bg77_at((char *)"AT+QSSLCFG=\"ignorelocaltime\",2,1", 500);
  }

  else
  {
    bg77_at((char *)"AT+QMTCFG=\"ssl\",0,0", 500, false);
  }
  
  

  int i = 0;
  Serial.println("Opening connection to broker...");
  String open = "AT+QMTOPEN=" + String(clientIdMQTT) +",\"" + broker + "\"," + String(port);  //String de comando AT
  char open_frame[open.length()+1];   // crea el buffer
  open.toCharArray(open_frame, open.length()+1);    //copia el string en el buffer

  retryOpen:

    String opening_response = bg77_at(open_frame, MRT_OPEN+1000);  //utiliza la función para utilizar comando AT


    if (opening_response.indexOf("+QMTOPEN: "+String(clientIdMQTT)+",0") >= 0)  // Si se abrió la conexión para el cliente correctamente
    {
      Serial.println("Connection opened!"); // informa al usuario
        i=0;
        if (this -> LEDs)
        {
          digitalWrite(WB_LED2, HIGH);
          delay(150); 
          digitalWrite(WB_LED2, LOW);
          delay(150); 
          digitalWrite(WB_LED2, HIGH);
          delay(150); 
          digitalWrite(WB_LED2, LOW);
          delay(150); 
        }
        delay(2000);
    }

    else if (opening_response.indexOf("+QMTOPEN: "+String(clientIdMQTT)+",2") >= 0)
    {
      Serial.println("MQTT client identifier is occupied.");
      delay(10000);
      bg77_at((char *)"AT+QMTCLOSE=0",120000);
      delay(5000);
      goto retryOpen;
    }
    
    else    // Si no se abrió la conexión con el cliente, se informa el respectivo error
    {
      Serial.println("Opening connection wrong.");
      if (opening_response.indexOf("+QMTOPEN: "+String(clientIdMQTT)+",-1") >= 0)
      {
        Serial.println("Failed to open network.");
      }
      else if (opening_response.indexOf("+QMTOPEN: "+String(clientIdMQTT)+",1") >= 0)
      {
        Serial.println("Wrong parameter.");
      }
      else if (opening_response.indexOf("+QMTOPEN: "+String(clientIdMQTT)+",3") >= 0)
      {
        Serial.println("Failed to activate PDP.");
      }
      else if (opening_response.indexOf("+QMTOPEN: "+String(clientIdMQTT)+",4") >= 0)
      {
        Serial.println("Failed to parse domain name.");
        Serial.println("The device will be restart in 5 seconds.");
        delay(5000);
        ESP.restart();
      }
      else if (opening_response.indexOf("+QMTOPEN: "+String(clientIdMQTT)+",5") >= 0)
      {
        Serial.println("Network connection error.");
      }
      else
      {
        Serial.println("ERROR");
      }
      if (i == 3)
      {
        return;
      }
      Serial.println("Trying again...");
      i++;
      goto retryOpen;
      
    }
  
  Serial.println("Connecting to broker...");    
  String connection = "AT+QMTCONN=" + String(clientIdMQTT) +",\"" + clientIdBr + "\",\"" + usernameBr + "\",\"" + passwordBr + "\"";  //String de comando AT
  char connection_frame[connection.length()+1];   // crea el buffer
  connection.toCharArray(connection_frame, connection.length()+1);    //copia el string en el buffer

  retryConnect:

    String connecting_response = bg77_at(connection_frame, MRT_CONN+1000);    //utiliza la función para utilizar comando AT
  
    if (connecting_response.indexOf("+QMTCONN: "+String(clientIdMQTT)+",0,0") >= 0 && !(connecting_response.indexOf("+QMTSTAT") >= 0))
    {
      Serial.println("Connected to broker!");
      if (this -> LEDs)
      {
        digitalWrite(WB_LED2, HIGH);
        delay(150); 
        digitalWrite(WB_LED2, LOW);
        delay(150); 
        digitalWrite(WB_LED2, HIGH);
        delay(150); 
        digitalWrite(WB_LED2, LOW);
        delay(150); 
      }
      delay(2000);
    }
    else
    {
      Serial.println("Could not connect to broker.");
      if (connecting_response.indexOf("+QMTCONN: "+String(clientIdMQTT)+",1,0") >= 0)
      {
          Serial.println("Packet retransmission.");
      }
      else if (connecting_response.indexOf("+QMTCONN: "+String(clientIdMQTT)+",2,0") >= 0)
      {
          Serial.println("Failed to send packet.");
      }
      else if (connecting_response.indexOf("+QMTSTAT") >= 0)
      {
        Serial.println("STAT Error.");
        delay(1000);
        return ConnectBroker(clientIdBr, usernameBr, passwordBr, clientIdMQTT, broker, port, SSL, cacert, clientCert, clientKey, MRT_OPEN, MRT_CONN);
      }
      else
      {
        Serial.println("Unknown error.");
        delay(1000);
      }
      
      if (i == 3)
      {
        return;
      }
      Serial.println("Trying again...");
      delay(2000);
      i++;
      goto retryConnect;
    }
}


// Funcion que publica (envia) un mensaje en el broker
// Return -> None
void StarterKitNB::SendMessage(String message, String topic, int clientIdMQTT, int msgID, int qos, int retain, int MRT_PUBEX)
{

  Serial.println("Sending message...");
  String envio = "AT+QMTPUBEX="+String(clientIdMQTT)+","+String(msgID)+","+String(qos)+","+String(retain)+",\""+topic+"\",\""+message+"\"";
  char envio_char[envio.length()+1];
  envio.toCharArray(envio_char, envio.length()+1);
  String resp_envio = bg77_at(envio_char, MRT_PUBEX);

  if (resp_envio.indexOf("+QMTSTAT") >= 0)
  {
    Serial.println("Message couldn't send.");
    this -> LastMessageStatus = false;
    delay(1500);
  }

  else if (resp_envio.indexOf("+QMTPUB: 0,0,0") >= 0)
  {
    Serial.println("Message sent!");
    this -> LastMessageStatus = true;
    if (this -> LEDs)
    {
      digitalWrite(WB_LED2, HIGH);
      delay(150); 
      digitalWrite(WB_LED2, LOW);
      delay(150); 
      digitalWrite(WB_LED2, HIGH);
      delay(150); 
      digitalWrite(WB_LED2, LOW);
      delay(150); 
      digitalWrite(WB_LED2, HIGH);
      delay(150); 
      digitalWrite(WB_LED2, LOW);
      delay(150); 
      digitalWrite(WB_LED2, HIGH);
      delay(150); 
      digitalWrite(WB_LED2, LOW); 
    }
  }
  
  else
  {
    this -> LastMessageStatus = false;
    Serial.println("Message couldn't send.");
    delay(1500);
  }
}

// Funcion que desconecta al cliente de TB y cierra la red para el cliente
// Return -> None
void StarterKitNB::DisconnectBroker(int clientIdMQTT, int MRT_DISC)
{
  int i = 0;
  Serial.println("Disconnect to broker...");
  String disc = "AT+QMTDISC="+String(clientIdMQTT);
  char disc_char[disc.length()+1];
  disc.toCharArray(disc_char, disc.length()+1);

  RetryDisconnect:
    String resp_disc = bg77_at(disc_char, MRT_DISC+1);

    if (resp_disc.indexOf("+QMTDISC: "+String(clientIdMQTT)+",0") >= 0)
    {
        Serial.println("Successful disconnection!");
        return;
        delay(2000);
    }

    else if (i < 3)
    {
      Serial.println("Could not disconnect. Trying again...");
      i++;
      goto RetryDisconnect;
    }
    
    else
    {
      Serial.println("Could not disconnect to broker.");
    }
}

// Funcion que conecta a un cliente al modo subscriptor de MQTT
// Return -> None
void StarterKitNB::SubscribeMQTT(int clientIdMQTT, int messageId, String topic, int qos)
{

  Serial.println("Client subscribing...");
  String atCommandStr = "AT+QMTSUB="+String(clientIdMQTT)+","+String(messageId)+",\""+topic+"\","+String(qos);
  char atCommandBuf[atCommandStr.length()+1];
  atCommandStr.toCharArray(atCommandBuf, atCommandStr.length()+1);
  String resp = bg77_at(atCommandBuf, 15000);
  if (resp.indexOf("+QMTSUB: "+String(clientIdMQTT)+","+String(messageId)+",0,0")>=0)
  {
    this -> SubscriptionStatus = true;
    Serial.println("The client has subscribed successfully.");
  }
  else
  {
    this -> SubscriptionStatus = false;
    Serial.println("The client couldn't subscribe, please try again.");
  }
  
}

// Funcion que espera un mensaje MQTT cuando se está en modo subscriptor
// Return -> String del mensaje recivido
String StarterKitNB::WaitMessageMQTT(int WaitTime, String ReturnFormat)
{
  
  if (!(this -> SubscriptionStatus))
  {
    Serial.println("The client is not in subscribe mode.");
    delay(2000);
    return "";
  }

  if ((ReturnFormat != "") && (ReturnFormat != "AT_RESPONSE"))
  {
    Serial.println("Check ReturnFormat input.");
    delay(2000);
    return "";
  }
  

  Serial.println("Waiting Message...");

  String bg77_rsp = "";
  String str = "AT+QMTRECV?";
  char at[str.length()+1];
  str.toCharArray(at, str.length()+1);
   
  char tmp[256] = {0};
  int len = strlen(at);
  strncpy(tmp, at, len);
  tmp[len] = '\r';
  delay(10);

  Serial1.write(tmp);
  while (WaitTime--)
  {
    if (Serial1.available())
    {
      bg77_rsp += char(Serial1.read());
    }
    delay(1);
  }

  // Serial.println(bg77_rsp);
  if (bg77_rsp.indexOf("QMTRECV: ")<0)
  {
    this -> SubscriptionStatus = false;
  }
  else
  {
    this -> SubscriptionStatus = true;
  }
  

  Serial.println("Timeout over");

  if (ReturnFormat == "AT_RESPONSE")
  {
    return bg77_rsp;
  }
  
  
  
  String result = "";
  int lastStart = bg77_rsp.indexOf("{");
  int lastEnd = bg77_rsp.indexOf("}");
  int StrCount = 0;

  while (lastStart >= 0)
  {
    result = result + bg77_rsp.substring(lastStart, lastEnd+1);
    lastStart = bg77_rsp.indexOf("{", lastStart+1);
    StrCount++;
    if (lastEnd+1 >= bg77_rsp.length())
    {
      break;
    }
    lastEnd = bg77_rsp.indexOf("}", lastEnd+1);
  }

  result = String(StrCount)+","+result;
  Serial.println("Number of received messages, messages: \n"+result+"\n");

  return result;
}

// Función que inicializa el GPS, desconectando la conexión WWAN
// Return -> None
void StarterKitNB::StartGPS()
{
  delay(500);
  bg77_at((char *)"AT+QGPSCFG=\"priority\",0", 500);  // Da prioridad a la antena GNSS
  delay(500);
  bg77_at((char *)"AT+QGPSEND", 500); // Termina el uso de GNSS previo
  delay(500);
  bg77_at((char *)"AT+CEREG=4", 500);   // Permite que la conexion pueda aplicar PSM
  delay(500);
  bg77_at((char *)"AT+QGPS=1", 500); // Inicia el uso de GNSS
  delay(500);
  bg77_at((char *)"AT+QGPSLOC=2", 500); // Genera la primera consulta GPS para calibrar la antena
  delay(500);
}

// Función que entrega una trama de posición GPS, de encontrarse calibrada la antena
// Return -> String de trama GPS
String StarterKitNB::PositionGPS()
{

  int attempts = 0;
  String GPS_fail = "Null"; 
  delay(500);

  while (attempts != 4)
  {
    if  (bg77_at((char *)"AT+QGPSLOC=2",500).indexOf("+CME ERROR: 516")  < 0  ) 
    {
      Serial.println("Correct GPS tracking");
      String GPS_response = bg77_at((char *)"AT+QGPSLOC=2", 500);
      attempts = 4;
      return GPS_response;
    }

    else
    {
      attempts += 1;
      Serial.println("Failed to connect");
      delay(4000);
    }
    
  }


  return GPS_fail;
}



// Función que termina el uso de GPS, reasigna la prioridad a WWAN
// Return -> None

void StarterKitNB::EndGPS()
{
  bg77_at((char *)"AT+QGPSEND", 500); // Termina el uso de GNSS
  delay(500);
  bg77_at((char *)"AT+QGPSCFG=\"priority\",1", 500);  // Da prioridad a la antena GNSS
  delay(500);
}

// Funcion que permite subir un archivo a la memoria del modulo
// Return -> None
void StarterKitNB::UploadFile(String fileName, String content, int size,int timeout){
  
  int initTime = millis();
  int count = 0;
  
  String at = "AT+QFUPL=\""+fileName+"\","+String(size)+","+String(timeout);
  char atBuf[at.length()+1];
  at.toCharArray(atBuf, at.length()+1);
  
  String bg77_rsp = bg77_at(atBuf, 500);
  while ((bg77_rsp.indexOf("+QFUPL:")<0) && ((millis() - initTime) < timeout*1000+2000))
  {
    if (Serial1.available())
    {
      bg77_rsp += char(Serial1.read());
    }
    
  
    if (bg77_rsp.indexOf("CONNECT")>=0 && count != size)
    {
      Serial1.write(byte(content[count]));
      Serial.print(content[count]);
      count++;
    }
  }
   
  Serial.println("\n");
  delay(10);
  Serial.println(bg77_rsp);
}

// Funcion que permite eliminar un archivo de la memoria del modulo
// Return -> None
void StarterKitNB::DeleteFile(String fileName){
  
  String at = "AT+QFDEL=\""+String(fileName)+"\"";
  char atBuf[at.length()+1];
  at.toCharArray(atBuf, at.length()+1);
  bg77_at(atBuf, 3000);
}

// Funcion que muestra en el terminal la lista de archivos en el directorio de la memoria del modulo
// Return -> None
void StarterKitNB::ListStorage(){
  
  bg77_at((char *)"AT+QFLST=\"*\"", 3000);
}



// Funcion que inicializa PSM en el dispositivo
// Ingresa timers T3412 (Requested extended periodic TAU value) y T3324 (Requested Active Time value) de PSM
// Return -> None
void StarterKitNB::StartPSM(String T3412, String T3324){

  String atpsm = "AT+QPSMS=1,,,\""+ T3412 + "\",\"" + T3324 + "\"";
  char atpsmBuf[atpsm.length()+1];
  atpsm.toCharArray(atpsmBuf, atpsm.length()+1);

  bg77_at((char *)"AT+QIACT=1", 500);
  delay(500);
  bg77_at((char *)"AT+CGATT=1", 500);
  delay(500);
  bg77_at(atpsmBuf, 500);

}

// Funcion que termina el uso de PSM
// Return -> None
void StarterKitNB::StopPSM(){

  bg77_at((char *)"AT+QPSMS=", 500);


}

