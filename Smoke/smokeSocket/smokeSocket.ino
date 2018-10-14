#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <SocketIoClient.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

SocketIoClient webSocket;

int sensorPin = A0;
int sensorValue = 0;
bool socketConnected = false;

bool candado = false; //candado para enviar id
bool enviar = true;   //candado para enviar alerta

//recibe desde la app que la alerta era verdader
void alertTrue(const char * payload, size_t length) {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.printf("got alert: %s\n", payload);
}

//recibe desde la app que la alerta era falsa
void alertFalse(const char * payload, size_t length) {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.printf("got alert: %s\n", payload);
}

//Envia id del kit cuando logra conectarse al servidor
void connectReady(const char * payload, size_t length){
  socketConnected = true;
  webSocket.emit("loginsensorkit", "{\"sensorid\":\"k1000\"}");
  sendAlert();
}

//Envia la alerta cuando se detecta mucho monoxido
void sendAlert(){
  webSocket.emit("alertfromsensor", 
          "{\"k1000\": {\"kitName\": \"Nombre kit 1\",\"kitStatus\": \"mal\",\"sensor\": {\"k1000s1\": {\"nombre\": \"Sensor 1 del  kit 1\",\"status\": \"mal\"},\"k1000s2\": {\"nombre\": \"Sensor 2 del kit 1\",\"status\": \"bien\"}}}}");
}

//Lee la señal analogica
void readAnalogSensor() {
  if( millis() % 100 != 0 ) return;
  sensorValue = analogRead(A0);
  Serial.println(sensorValue);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.begin(115200);

  WiFiManager wifiManager;
  
  //reset saved settings
  //wifiManager.resetSettings();
  
  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  wifiManager.autoConnect("Smoke");
  Serial.println("connected to WiFi");
  delay(100);  
  Serial.println("Tratando conectar socket...");

  //Escucha desde el servidor eventos
  webSocket.on("responsefromservertrue", alertTrue);
  webSocket.on("responsefromserverfalse", alertFalse);
  
  //emitId se ejecuta solo cuando logra conectarse de manera exitosa al servidor
  webSocket.on("connect", connectReady);
  
  webSocket.begin("quiet-journey-37928.herokuapp.com");
}

void loop() {
  //Permite mantener activo socket.io
  webSocket.loop();
  if(socketConnected){
    readAnalogSensor();
  }
  
  //Serial.println(sensorValue);

  //rango que determina si enviar alerta o no
  /*if(sensorValue >= 750){
    if(enviar == true){
      Serial.println("se paso el valor");
      sendAlert();
      enviar = false;
    }
    digitalWrite(LED_BUILTIN, LOW);
  }*/
  //se abre el candado cuando el valor baja mas del limite
  //para evitar emitir la alerta mas de una vez
  /*else if(sensorValue <= 700){
    if(enviar == false){
      Serial.println("candado reiniciado");
      enviar = true;
    }
    digitalWrite(LED_BUILTIN, HIGH);
  }*/
    
}
