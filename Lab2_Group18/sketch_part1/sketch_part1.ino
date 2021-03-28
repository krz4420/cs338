int pressedState = 0;

void setup() {
  Serial.begin(9600);
  
  Serial.println("Got Here");
  DDRD &= ~(1 << PD2);  // Set Pin 2 as input
  PORTD = (1 << PD2);   // make Pin 2 high
  
}

void loop() {
  int reading = bitRead(PIND, 2); // Read the bit value on Pin 2
  
  // Debouncing
  if(reading == 1){
    delay(50);
    reading = bitRead(PIND, 2);
    if(reading == 0){
      pressedState = !pressedState;
      Serial.println(pressedState);
    }
  }
}
