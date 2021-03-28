const int sensorPin = A0; // Pin for photocell
const int ledPin = 3;  

int sensorVal = 0;  
int numVal = 0;

void setup() {
  // Set up LED pin as output
  pinMode(ledPin,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  sensorVal = analogRead(sensorPin);  // Store the value from photocell
  numVal = sensorVal % 255;   // Convert it to a corresponding num 0-254
  analogWrite(ledPin, numVal);  // Output the specified brightness
  delay(1000);
}
