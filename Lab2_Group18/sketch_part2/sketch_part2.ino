
// Pins from shift registers to Arduino
const int latchPin = 8;   // PB0 -- digital pin 8
const int clockPin = 12;  // PB4 -- digital pin 12
const int dataPin = 11;   // PB3 -- digital pin 11


byte cols[5] = {126, 191, 223, 239, 247}; // Array storing col sequence
                                          // Pins QA-QE used on second shift register
                                          // Pins QA-QG on first shift register for bit pattern for rows
// Array for bit pattern of numbers 0-9
byte numbers [10][5] = {{62,81,73,69,62},{0,66,127,64,0},{66,97,81,73,70},{33,97, 69, 75, 49},{24,20, 18, 127, 16},
                        {39, 69, 69, 69, 57},{60,74,73,73,48},{1,113,9,5,3},{54,73,73,73,54},{6, 73, 73,41,30}};

void setup() {
  DDRB = DDRB | B11001; // Sets Digital Pin 8,11 and 12 to output pins not affecting other pins 

  Serial.begin(9600);
  Serial.println("reset");
}

void loop() {
  registerWrite(0, HIGH); 
}

void registerWrite(int whichPin, int whichState){
 
  // Loop through the numbers 0-9 in the numbers array
  for(int j = 0; j < 10; j++)
  {
    int x = 0;  // used in loop to display the number long enough to be visible
    
    // Loop allows the number to be displayed long enough to be visible
    while(x < 10000){
      
      // Loop through the cols one at a time and turn on the LEDs that should be on for that row
      for(int i = 0; i < 5; i++){
        
        PORTB = PORTB & B0; // Set Latch (Digital Pin 8) to LOW

        // Shift bits out for col value and row value for specified number
        my_shiftOut(dataPin, clockPin, LSBFIRST,cols[i]);
        my_shiftOut(dataPin, clockPin, MSBFIRST, numbers[j][i]);
        
        PORTB = PORTB | B1; // Set Latch (Digital Pin 8) to HIGH
      }
      x++;  // Inc
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
      if( !!(val & (1 << i)) == 1){   // If the bit in i-th spot of the 8-bit number is 1
        PORTB = PORTB | B1000;  // Set data pin HIGH if the bit is 1
      }
      else{
        PORTB = PORTB & B0111;  // Set data pin LOW if the bit is 0
      }
    }
    else{
      if( !!(val & (1 << (7 - i))) == 1){ // If the bit in the i-th spot of the 8-bit number is 1
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
