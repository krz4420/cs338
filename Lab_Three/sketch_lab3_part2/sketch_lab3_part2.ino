const int sensorPin = A0;   // Pin for photocell

const int trigPin = 13;     // Trig pin for range finder
const int echoPin = 12;     // Echo pin for range boi

const int redPin = 3;
const int greenPin = 5;
const int brightnessPin = 6;

int sensorVal = 0;
int numVal = 0;
#include <math.h>

void setup() {
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(brightnessPin, OUTPUT);
  
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(brightnessPin, LOW);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  Serial.begin(9600);
}

void loop() {
  sensorVal = analogRead(sensorPin);    // Gets val from photo
  
  float duration, distance;
  numVal = sensorVal % 255;
  Serial.println(numVal);
  
  digitalWrite(trigPin, LOW);
  delay(2);
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin,LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;
  float twoBalls = 255;
  float newDist =  fmod(distance, twoBalls);
  
  if(newDist < 126){
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
    analogWrite(brightnessPin, numVal);
    delay(1000);
  }else{
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
    analogWrite(brightnessPin, numVal);
    delay(1000);
  }
}
