#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <SocketIoClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(D4,D8); //(cepin, cspin)

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

#define ID "Smoke-"
String kitID = ID + String(ESP.getChipId(), HEX) + String(ESP.getFlashChipId(), HEX);
String url = "quiet-journey-37928.herokuapp.com";

SocketIoClient webSocket;

int alarmPin = D1;
int sensorPin = A0;
int sensorValue = 0;
bool socketConnected = false;

bool lock = false;

//recibe desde la app que la alerta era verdader
void alertTrue(const char * payload, size_t length) {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.printf("got alert: %s\n", payload);
  digitalWrite(alarmPin, HIGH);
  //AQUI HACEMOS ALGO CUANDO ES VERDADERO
}

//recibe desde la app que la alerta era falsa
void alertFalse(const char * payload, size_t length) {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.printf("got alert: %s\n", payload);
  digitalWrite(alarmPin, LOW);
  lock = false;
}

//Envia id del kit cuando logra conectarse al servidor
void connectReady(const char * payload, size_t length) {
  socketConnected = true;
  webSocket.emit("loginsensorkit", "{\"kitID\":\"Smoke-1165061640e0\"}");
  webSocket.emit("kitupdatestatus",
                 "{\"Smoke-1165061640e0\": {\"kitName\": \"Nombre kit 1\",\"kitStatus\": \"bien\",\"sensor\": {\"k1000s1\": {\"nombre\": \"Sensor 1 del  kit 1\",\"status\": \"bien\"},\"k1000s2\": {\"nombre\": \"Sensor 2 del kit 1\",\"status\": \"bien\"}}}}");
  //sendAlert();
}

//Envia la alerta cuando se detecta mucho monoxido
void sendAlert() {
  webSocket.emit("alertfromsensor",
                 "{\"Smoke-1165061640e0\": {\"kitName\": \"Nombre kit 1\",\"kitStatus\": \"mal\",\"sensor\": {\"k1000s1\": {\"nombre\": \"Sensor 1 del  kit 1\",\"status\": \"mal\"},\"k1000s2\": {\"nombre\": \"Sensor 2 del kit 1\",\"status\": \"bien\"}}}}");
}

//Lee la señal analogica
void readAnalogSensor() {
  if ( millis() % 100 != 0 ) return;
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(alarmPin, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //digitalWrite(alarmPin, LOW);

  pinMode(D4, OUTPUT);//PIN cspin

  radio.begin();

  radio.startListening();
  //Open pipes to other nodes for communication
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  Serial.begin(115200);

  WiFiManager wifiManager;

  //reset saved settings
  //wifiManager.resetSettings();

  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(IPAddress(10, 0, 1, 1), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

  wifiManager.autoConnect("Smoke");
  Serial.println("connected to WiFi");
  delay(100);
  Serial.println("Tratando conectar socket...");

  //Escucha desde el servidor eventos
  webSocket.on("responsefromservertrue", alertTrue);
  webSocket.on("responsefromserverfalse", alertFalse);

  //emitId se ejecuta solo cuando logra conectarse de manera exitosa al servidor
  webSocket.on("connect", connectReady);

  webSocket.begin(url.c_str());

}

void loop() {
  //Permite mantener activo socket.io
  webSocket.loop();
  if (socketConnected) {
    readAnalogSensor();
  }

  
  //rango que determina si enviar alerta o no
  if(sensorValue >= 200 && !lock){
    sendAlert();
    lock = true;
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(alarmPin, HIGH);
  }

  int data;
  while (radio.available())
  {
    // Fetch the payload, and see if this was the last one.
    radio.read( &data, sizeof(int) );
    if(data == 10 && !lock){
      sendAlert();
      lock = true;
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(alarmPin, HIGH);
    }
  }

}
