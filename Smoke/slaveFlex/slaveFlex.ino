//Este codigo necesita de la library "RF24"
//Espera señales de un emisor y de despliega los datos por Serial.
//Tambien enciende un led

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"


//Indica en que pines del Arduino esta conectada la entrada CE y CS del rf24
//Set up nRF24L01 radio on SPI bus plus pins 9 & 10 en Arduino UNO (5 y 6 en Bitalino)
RF24 radio(9,10);

//
//Configura 2 direcciones de 8-bit para establecer coneccion (para este ejemplo solo se ocupa la primera);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

int data;
bool candado = true;

int buzzer = 6;

void setup(void)
{

  pinMode(9, OUTPUT);//PIN cspin
  pinMode(2, OUTPUT);//PIN del led
  Serial.begin(9600);

  // Setup and configure rf radio
  radio.begin();
  delay(500);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate( RF24_250KBPS );
  //optionally, increase the delay between retries & # of retries
  //radio.setRetries(15,15);
  //optionally, reduce the payload size.  seems to
  //improve reliability
  //radio.setPayloadSize(8);

  //Start listening
  radio.startListening();
  radio.openWritingPipe(pipes[1]); //Esto no se ocupa en este ejemplo
  radio.openReadingPipe(1,pipes[0]);
}

void loop(void){
  radio.startListening();
  bool alarma;
  while (radio.available()){
    // Fetch the payload, and see if this was the last one.
    radio.read( &alarma, sizeof(bool) );
    if(alarma){
      noTone(buzzer);
    }
  }
  
  //Serial.println(analogRead(A0));
  if(analogRead(A0) < 10 && candado){

    data = 10;
    
    radio.stopListening();
    Serial.print("Enviando  ");
    Serial.println(data);
    bool ok = radio.write( &data, sizeof(int) );
  
    if (ok){
      Serial.println("ok...");
      tone(buzzer, 440);
    }
    else{
      Serial.println("failed");
    }
    candado = false;
  }else if(analogRead(A0) > 15){
    candado = true;
  }

}
