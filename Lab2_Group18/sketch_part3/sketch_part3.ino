volatile int num = 0;
int buttonState = 1;

// Pins from shift registers to Arduino
const int latchPin = 8;   // PB0 -- digital pin 8
const int clockPin = 12;  // PB4 -- digital pin 12
const int dataPin = 11;   // PB3 -- digital pin 11


byte cols[5] = {126, 191, 223, 239, 247}; // Array storing col sequence
                                          // Pins QA-QE on second shift register
                                          // Pins QA-QG on first shift register for bit value for rows
// Array for bit pattern of numbers 0-9
byte numbers [10][5] = {{62,81,73,69,62},{0,66,127,64,0},{66,97,81,73,70},{33,97, 69, 75, 49},{24,20, 18, 127, 16},
                        {39, 69, 69, 69, 57},{60,74,73,73,48},{1,113,9,5,3},{54,73,73,73,54},{6, 73, 73,41,30}};

void setup() {
  DDRB = DDRB | B11001; // Sets Digital Pin 8,11 and 12 to output pins

  DDRD &= ~(1 << PD2);  // Set Digital Pin 2 as an Input
  PORTD = (1 << PD2);  
  
  attachInterrupt(0, buttonPressed, RISING);
  Serial.begin(9600);
  registerWrite(0, HIGH); 
}

void loop() {
}

void registerWrite(int whichPin, int whichState){
    
   // Loop allows the number to be displayed long enough to be visible
   while(1){
     // Loop through the cols one at a time and turn on the LEDs that should be on for that row
     for(int i = 0; i < 5; i++){
        
      PORTB = PORTB & B0; // Set Latch (Digital Pin 8) to LOW

      // Shift bits out for col value and row value for specified number
      my_shiftOut(dataPin, clockPin, LSBFIRST,cols[i]);
      my_shiftOut(dataPin, clockPin, MSBFIRST, numbers[num][i]);
        
      PORTB = PORTB | B1; // Set Latch (Digital Pin 8) to HIGH
    }
  }
}

void my_shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
  uint8_t i;

  // Loop through each bit in the 8-bit number
  for(int i = 0; i < 8; i++){
    if(bitOrder == LSBFIRST)
    {
      if( !!(val & (1 << i)) == 1){ // Checks the i-th spot of the 8-bit number is 1
        PORTB = PORTB | B1000;  // Set data pin HIGH if the bit is 1
      }
      else{
        PORTB = PORTB & B0111;  // Set data pin LOW if the bit is 0
      }
    }
    else{
      if( !!(val & (1 << (7 - i))) == 1){ // // Checks the i-th spot of the 8-bit number is 1
         PORTB = PORTB | B1000; // Set data pin HIGH if the bit is 1
      }
      else{
        PORTB = PORTB & B0111;  // Set data pin LOW if the bit is 0
      }
    }

    PORTB = PORTB | B10000; // Set the clock pin HIGH
    PORTB = PORTB & B01111; // Set the clock pin LOW
  }
}

void buttonPressed(){
  long last_interrupt = 0;        // Stores when the last interrupt was
  long interrupt_time = millis(); // Stores the curr time
  int reading = bitRead(PIND, 2); // Reads the value of Pin 2
  delay(50);
  
  if(buttonState == reading)    // If the buttonState is still the same as the reading, then button was pushed and want to change
  {
      num++;
      if(num > 9){      // Handle wrap around
        num = 0;
      }
  }
  last_interrupt = last_interrupt;
   
}
