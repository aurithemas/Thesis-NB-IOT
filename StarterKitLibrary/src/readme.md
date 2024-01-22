# Funciones de la librería StarterKitNB

## Constructores

### StarterKitNB()

    StarterKitNB::StarterKitNB()
    
Función que construye el objeto de StarterKitNB() para utilizarse en el código principal, permitiendo el acceso al resto de funciones

Ejemplo:

    #include <StarterKitNB.h>
    StarterKitNB sk;

### ~StarterKitNB()

    StarterKitNB::~StarterKitNB()

Función que desconstruye el objeto StarterKitNB.

Ejemplo:

    #include <StarterKitNB.h>
    StarterKitNB sk;
    sk = ~StarterKitNB();
    
## Inicialización del dispositivo e interfaz AT

### Setup()

    void StarterKitNB::Setup(bool LEDs)

Funcion que configura inicialmente todo lo necesario para usar la libreria.

Return -> None

Ejemplo:

    sk.Setup(true);   // Se setea la configuracion para la libreria.
    

### Reset()

    void StarterKitNB::Reset()

Funcion que aplica un reset a los comandos AT configurados (No aplica a todos)

Return -> None

Ejemplo:

    sk.Reset();
    

### bg77_at()

    String StarterKitNB::bg77_at(char *at, uint32_t timeout, bool print)
    

Funcion predeterminada que permite la comunicacion serial para utilizar comandos AT al modulo BG77

Return -> Respuesta del comando AT

Ejemplo:

    bg77_at((char *)"AT+QGPSCFG=\"priority\",1", 500);  // Da prioridad a la antena LTE
    delay(500);
    
### SerialComm()

    void StarterKitNB::SerialComm(uint32_t MaxTimeResponse)

Funcion que permite escribir en la terminal comandos AT

Return -> None

Ejemplo:

    sk.SerialComm(500); //maximo tiempo de respuesta 500 ms

## Conexión a NB

### Connect()

    void StarterKitNB::Connect(String apn, String band)
    
Funcion que permite conectar al dispositivo a NB

Return -> None

Ejemplo: 

    String apn = "insertyoutapn.here.cl"; //private apn

    void setup() {

      sk.Setup(true);   // Se setea la configuracion para la libreria
      delay(1000);
      sk.Connect(apn);  // Se conecta a NB
      delay(1000);
    }


### Reconnect()

    void StarterKitNB::Reconnect(String apn, String band)

Funcion que espera cierto tiempo para verificar si vuelta la conexion a NB, sino sucede se conecta nuevamente

Return -> None

Ejemplo:

    if (!sk.ConnectionStatus()) // Si no hay conexion a NB
    {
      sk.Reconnect(apn_entel);  // Se intenta reconecta
      delay(2000);
    }

### DeviceCheck()

    void StarterKitNB::DeviceCheck()
    
Funcion que entrega varia informacion sobre el estado actual del dispositivo

Return -> None

### ConnectionStatus()

    bool StarterKitNB::ConnectionStatus()

Funcion que verifica si se encuentra conectado el dispositivo a NB

Permite obtener el estado de la conexión y hacer uso de su valor booleano para su reconexión

Return -> true si se encuentra conectado, false si no

## Conectividad a MQTT/ThingsBoard

### ConnectBroker()

    void StarterKitNB::ConnectBroker(String clientIdBr, String usernameBr, String passwordBr, int clientIdMQTT, String broker, int port, bool SSL, String cacert, String clientCert, String clientKey, int MRT_OPEN, int MRT_CONN)

Funcion que abre la red para el cliente y lo conecta a ThingsBoard

Return -> None

    // Thingsboard
    String ClientIdTB = "yourclientID";
    String usernameTB = "yourusername";
    String passwordTB = "yourpassword";

    String apn = "insertyoutapn.here.cl"; //private apn
    
    int clientIdMQTT = 2;

    void setup() {

      sk.Setup(true);   // Se setea la configuracion para la libreria
      delay(1000);
      sk.Connect(apn);  // Se conecta a NB
      delay(1000);
    }

    void loop() {
      if (!sk.ConnectionStatus()) // Si no hay conexion a NB
      {
        sk.Reconnect(apn);  // Se intenta reconecta
        delay(2000);
      }
      if (!sk.LastMessageStatus)  // Si no se ha enviado el ultimo mensaje
      {
        sk.ConnectBroker(ClientIdTB, usernameTB, passwordTB);  // Se conecta a ThingsBoard
        delay(2000);
      }
      
### SendMessage()
    
    void StarterKitNB::SendMessage(String message, String topic, int clientIdMQTT, int msgID, int qos, int retain, int MRT_PUBEX)
    
Funcion que publica (envia) un mensaje en el broker

Return -> None

Ejemplo:

      // Message
      String object = "temperature";
      String value = "37.8";
      String msg = "{\"" + object + "\":" + value + "}";
      
      sk.SendMessage(msg);    // Se envia el mensaje
      delay(2000);
      
### DisconnectBroker()

    void StarterKitNB::DisconnectBroker(int clientIdMQTT, int MRT_DISC, int MRT_CLOSE)

Funcion que desconecta al cliente de TB y cierra la red para el cliente

Return -> None

Ejemplo:

    sk.DisconnectBroker(ClientIdTB);
    

### SubscribeMQTT()

    void StarterKitNB::SubscribeMQTT(int clientIdMQTT, int messageId, String topic, int qos)

Funcion que conecta a un cliente al modo subscriptor de MQTT

Return -> None

Ejemplo:

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
      
### WaitMessage()

    String StarterKitNB::WaitMessageMQTT(int WaitTime, String ReturnFormat)

Funcion que espera un mensaje MQTT cuando se está en modo subscriptor

Return -> String del mensaje recivido

Ejemplo:

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

## Conectividad a GPS

### StartGPS()

    void StarterKitNB::StartGPS()

Función que inicializa el GPS, desconectando la conexión WWAN

Return -> None

Ejemplo:

    void setup() {
      sk.Setup();
      delay(500);
      sk.StartGPS();
      delay(500);
    }

### PositionGPS()

    String StarterKitNB::PositionGPS()

Función que entrega una trama de posición GPS, de encontrarse calibrada la antena

Return -> String de trama GPS

Ejemplo:

    msg = sk.PositionGPS();
    delay(1000);
  
### EndGPS()

    void StarterKitNB::EndGPS()
    
Función que termina el uso de GPS, reasigna la prioridad a WWAN

Return -> None

## Manejo de archivos en BG77

### UploadFile()

    void StarterKitNB::UploadFile(String fileName, String content, int size,int timeout)

Funcion que permite subir un archivo a la memoria del modulo

Return -> None


### DeleteFile()

    void StarterKitNB::DeleteFile(String fileName){}

Funcion que permite eliminar un archivo de la memoria del modulo

Return -> None

Ejemplo:

      // Files info
      String nameCacert = "AmazonRootCA1.pem";
      String cacert = "content cacert";

      int lenCacert = cacert.length();

      void setup() {

        sk.Setup(true);   
        delay(1000);  

        sk.DeleteFile(nameCacert);
        sk.UploadFile(nameCacert, cacert, lenCacert, 10);
        }

### ListStorage()

    void StarterKitNB::ListStorage()

Funcion que muestra en el terminal la lista de archivos en el directorio de la memoria del modulo

Return -> None

## Power Saving Mode

### StartPSM()

    void StarterKitNB::StartPSM(String T3412, String T3324)

Funcion que inicializa PSM en el dispositivo

Ingresa timers T3412 (Requested extended periodic TAU value) y T3324 (Requested Active Time value) de PSM

Return -> None

### StopPSM()

    void StarterKitNB::StopPSM()

Funcion que termina el uso de PSM
Return -> None

