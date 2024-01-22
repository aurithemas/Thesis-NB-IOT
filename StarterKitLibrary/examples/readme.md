# Códigos ejemplo del uso de librería StarterKitNB

La presente carpeta contiene distintos ejemplos del uso de la librería StarterKitNB en el contexto de Arduino/PlatformIO. Se espera a partir de estos generar una base en la cual crear nuevos códigos dependiendo de las necesidades del usuario.

Los ejemplos interactuan de forma modular con el dispositivo, mas no presentan el uso de dispositivos externos tales como sensores o actuadores. Sin embargo, son lo suficientemente flexibles para permitir su integración dentro de su código.

## ConnectAWS.cpp

Genera una conexión MQTT segura mediante SSL a Amazon Web Services. Para esto se necesita descargar los certificados correspondientes y subirlos a partir de los nombres asignados en el código para la variable, haciendo uso de los comandos de la librería.

## ConnectGPS.cpp

Inicializa el modo de prioridad GPS y forma una conexión con la red satelital GNSS a partir del módulo BG77, para así calibrar y tomar una traza de posición. Notese que el módulo es altamente sensible y de no encontrarse en condiciones adecuadas (cerca de una ventana/outdoor con la antena apuntando al cielo) no encontrará una traza adecuada.

## ConnectNB.cpp

Inicializa el dispositivo para generar una conexión a NB-IoT, asignandole los valores correspondientes en el código para la elección de APN, otorgados por el usuario. 
Para hacer uso de la función "auto" se debe haber conectado al menos a una APN viable previamente, pública o privada.

## ConnectTB.cpp

Genera la previamente vista conexión a NB-IoT, para posteriormente conectarse a la plataforma ThingsBoard a través de MQTT. Para realizar esta conexión se deben ajustar los parámetros correspondientes el broker MQTT de ThingsBoard, al igual que ingresar los valores de cliente, usuario y contraseña del cliente MQTT a utilizar.

Notese que este código no genera el cierre de la conexión, el cual se debe de agregar si se generan problemas con el uso de múltiples clientes.

## InterfaceAT.cpp

Inicializa el dispositivo y permite el ingreso de comandos a través de su puerto serial mediante una interfaz dentro del computador (monitores seriales en el puerto COM adecuado). Mediante este código se pueden probar comandos AT en tiempo real y programar el equipo para pruebas de código.

## SubsciptorMode.cpp

Genera la conexión MQTT del dispositivo para activar su modo de subscripción, el cual permite recibir datos del dispositivo MQTT. Se debe configurar el broker MQTT y como enviará los datos al dispositivo.
