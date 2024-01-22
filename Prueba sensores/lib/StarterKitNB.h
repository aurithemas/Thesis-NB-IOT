#ifndef __STARTERKITNB_H__
#define __STARTERKITNB_H__


#include <Arduino.h>

class StarterKitNB // Clase de StarterKitNB
{
    public:

        // FORMATO PUBLIC: metodo();
        // Ingresar aquí

        StarterKitNB(); // Constructor
        ~StarterKitNB(); // Destructor 
        bool LEDs;
        bool LastMessageStatus;
        bool SubscriptionStatus;

        void Setup(bool LEDs = true); // Configuración

        void Reset(); // Resetea config

        String bg77_at(char *at, uint32_t timeout, bool print = true); // Escritor e interprete de comandos AT

        void SerialComm(uint32_t MaxTimeResponse = 500); // Interfaz de lectura y escritura de comandos AT

        // CONECTIVIDAD A NB
        void UserAPN(String apn, String user, String password);

        void Connect(String apn, String band = "B28 LTE, B2 LTE", String network = "NB"); // Conectar a la red WWAN mediante NB, despues hacerlo variable
        
        void Reconnect(String apn, String band = "B28 LTE, B2 LTE", String network = "NB"); // Reconectar a la red WWAN
        
        void DeviceCheck(); // Check del dispositivo

        bool ConnectionStatus(); // Estado de la conexión

        
        // CONECTIVIDAD A TB
        void ConnectBroker(String clientIdBr, String usernameBr, String passwordBr = "", int clientIdMQTT = 0, String broker = "thingsboard.cloud", int port = 1883, bool SSL = false, String cacert = "", String clientCert = "", String clientKey = "", int MRT_OPEN = 16000, int MRT_CONN = 16000); // Conecta a un cliente a ThingsBoard

        void SendMessage(String message, String topic = "v1/devices/me/telemetry", int clientIdMQTT = 0, int msgID = 0, int qos = 0, int retain = 0, int MRT_PUBEX = 16000);   // Envia un mensaje a ThingsBoard

        void DisconnectBroker(int clientIdMQTT = 0, int MRT_DISC = 20000); // Desconecta a un cliente de ThingsBoard

        void SubscribeMQTT(int clientIdMQTT = 0, int messageId = 1, String topic = "v1/devices/me/attributes", int qos = 0);

        String WaitMessageMQTT(int WaitTime = 10000, String ReturnFormat = "");

        
        // CONECTIVIDAD A GPS

        void StartGPS(); // Inicia GPS y le da prioridad

        String PositionGPS(); // Entrega una traza de posición y la guarda

        void EndGPS(); // Termina GPS y quita prioridad


        // ADMINISTRACION DE ARCHIVOS EN EL MODULO
        void UploadFile(String fileName, String content, int size,int timeout);

        void DeleteFile(String fileName);

        void ListStorage();


        // USO DE PSM

        void StartPSM(String T3412, String T3324); 

        void StopPSM();


    private:

        // FORMATO PRIVATE: _metodo;
        // Ingresar aquí valores privados usados en public que solo conoce el Kit por ejemplo    

    protected:

        // FORMATO PROTECTED: 
};


#endif // __STARTERKITNB_H__
