int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = LED_BUILTIN;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  if(sensorValue >= 750)
    digitalWrite(ledPin, LOW);
  else
    digitalWrite(ledPin, HIGH);
}
