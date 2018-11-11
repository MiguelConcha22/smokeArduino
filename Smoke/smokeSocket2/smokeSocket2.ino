#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <SocketIoClient.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

SocketIoClient webSocket;

int sensorPin = A0;
int sensorValue = 0;

bool candado = false; //candado para enviar id
bool enviar = true;   //candado para enviar alerta

//unsigned long tiempoInicio;

//recibe mensaje de prueba del servidor
void test(const char * payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
}

/*void alert(const char * payload, size_t length) {
  char *str = const_cast<char *>(payload);
  if((str) == "verdadero"){
    digitalWrite(LED_BUILTIN, LOW);
    Serial.printf("VERDADERO\n");
  }else if((str) == "falso"){
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.printf("FALSO\n");
  }
  Serial.printf("got alert: %s\n", payload);
}*/

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
void emitId(const char * payload, size_t length){
  webSocket.emit("loginsensorkit", "{\"sensorid\":\"k1000\"}");
  sendAlert();
  //webSocket.emit("alertfromsensor", 
  //        "{\"k1000\": {\"kitName\": \"Nombre kit 1\",\"kitStatus\": \"mal\",\"sensor\": {\"k1000s1\": {\"nombre\": \"Sensor 1 del  kit 1\",\"status\": \"mal\"},\"k1000s2\": {\"nombre\": \"Sensor 2 del kit 1\",\"status\": \"bien\"}}}}");
  Serial.printf("got conection\n");
}

//Envia la alerta cuando se detecta mucho monoxido
void sendAlert(){
  webSocket.emit("alertfromsensor", 
          "{\"k1000\": {\"kitName\": \"Nombre kit 1\",\"kitStatus\": \"mal\",\"sensor\": {\"k1000s1\": {\"nombre\": \"Sensor 1 del  kit 1\",\"status\": \"mal\"},\"k1000s2\": {\"nombre\": \"Sensor 2 del kit 1\",\"status\": \"bien\"}}}}");
}

void setup() {
  //tiempoInicio = millis();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // put your setup code here, to run once:
  Serial.begin(115200);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  
  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("Smoke");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  delay(100);
  //simple verificacion de que sigue conectado
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Still Connected");
  }else{
    Serial.println("Error in WiFi connection");
  }    
  Serial.println("Tratando conectar socket...");

  //escucha desde el servidor mensajes
  webSocket.on("test", test);
  webSocket.on("responsefromservertrue", alertTrue);
  webSocket.on("responsefromserverfalse", alertFalse);
  
  //se ejecuta solo cuando logra conectarse
  //de manera exitosa al servidor
  webSocket.on("connect", emitId);
  
  //Comienza la conexion con el servidor
  webSocket.begin("quiet-journey-37928.herokuapp.com");
  
  //webSocket.begin("smokeserverfede.herokuapp.com");
  //webSocket.beginSSL("https://smokeserverfede.herokuapp.com", 443, "/socket.io/?transport=websocket", "");
  
  /*if(millis() - tiempoInicio >= 10000){
    if(candado == false){
      webSocket.emit("loginsensorkit", "{\"sensorid\":\"k1000\"}");
      candado = true;
    }
    delay(5000);
    if(enviar == true){
      webSocket.emit("alertfromsensor", 
        "{\"k1000\": {\"kitName\": \"Nombre kit 1\",\"kitStatus\": \"mal\",\"sensor\": {\"k1000s1\": {\"nombre\": \"Sensor 1 del  kit 1\",\"status\": \"mal\"},\"k1000s2\": {\"nombre\": \"Sensor 2 del kit 1\",\"status\": \"bien\"}}}}");
      enviar = false;
    }
  }*/
}

void loop() {
  //permite que lo relacionado con webSocket
  //en void setup se ejecute constantemente
  webSocket.loop();

  //sensorValue = analogRead(sensorPin);
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

  //prueba con POST y GET
  /*if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

      http.begin("http://quiet-journey-37928.herokuapp.com/arduino");
  
      int httpCode = http.GET();

      if(httpCode > 0) {
        String payload = http.getString();
        Serial.print("payload: ");
        Serial.println(payload);
      }
  
      http.end();
      
  }else{
    Serial.println("Error in WiFi connection");
  }
      
    delay(5000);*/
    
}
