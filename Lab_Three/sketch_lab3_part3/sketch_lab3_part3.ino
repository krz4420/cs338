#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 128;

long lastEncoderValue = 0;
int lastTime = 0;


const int OLED_CS = 8;
const int OLED_DC = 9;
const int OLED_RST = 10;
const int OLED_SI = 11;
const int OLED_CLK = 12;
int x = 4;
int y = 4;
int hitBottom = 0;
int hitTop = 1;
int hitLeft = 1;
int hitRight = 0;

Adafruit_SSD1306 disp(128,64,OLED_SI,OLED_CLK,OLED_DC,OLED_RST,OLED_CS);

// 0-127 are the x-values 
// 0-63 are the y-values
// Bottom-right corner = (123, 59)
// Bottom-left corner = (4,59)
// Top-right corner = (123,4)
// Top-left corner = (4.4)

void setup() {
  Serial.begin(9600);
  //setting up encoder pins
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on  
  //Interupt for encoder
  attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder, CHANGE);

  disp.begin(SSD1306_SWITCHCAPVCC);
  disp.clearDisplay();
  disp.display();
    
}

void loop() {
  if(encoderValue == 128){ // ENCODER VALUE == 128 NO MOVEMENT OF BALL
    disp.drawCircle(x, y, 4, WHITE);
    disp.display();
  }else if(encoderValue > 128){ //ENCODER VALUE > 128 BALL MOVING "FORWARD"
    //Keep moving left if the ball has not hit the left wall
    if(hitLeft== 0){
      x -= 4;
    }
    //Keep moving right if the ball has not hit the right wall
    if(hitRight == 0){
      x += 4;
    }
    //Keep moving up if the ball has not hit the top wall
    if(hitBottom == 0){
      y += 4;
    }
    //Keep moving left if the ball has not hit the left wall
    if(hitTop == 0){
      y -= 4;
    }
    //Draw the current circle 
    disp.drawCircle(x, y, 4, WHITE);
    disp.display();
    int delayVal = 0;
    //Calculate the delay value based on the encoder value
    if(encoderValue < 255){
      delayVal = 255 % encoderValue;
    }else{
      delayVal = 1;
    }
    delay(delayVal);
    disp.fillScreen(BLACK);
    
    if(y >= 59){
      hitBottom = 1;
      hitTop = 0;
    }
    if(y <= 4){
      hitBottom = 0;
      hitTop = 1;
    }
    if(x >= 123){
      hitRight = 1;
      hitLeft = 0;
    }

    if(x <= 4){
      hitRight = 0;
      hitLeft = 1;
    }
 
  }else{ // ENCODER VALUE < 128 
     if(hitLeft== 0){
      x += 4;
    }

    if(hitRight == 0){
      x -= 4;
    }

    if(hitBottom == 0){
      y -= 4;
    }

    if(hitTop == 0){
      y += 4;
    }
    disp.drawCircle(x, y, 4, WHITE);
    disp.display();
    int delayVal = 0;
    if(encoderValue < 128 && encoderValue> 0){
      delayVal = encoderValue;
    }else{
      delayVal = 1;
    }
    delay(delayVal);
    //Draw the screen black to help make the ball look like its moving
    disp.fillScreen(BLACK);
    // If y is greater than or eaqual to 59 then it has hit the top move down
    if(y >= 59){
      hitBottom = 0;
      hitTop = 1;
    }
    //If y is less than or equal to 0 then it has hit the bottom move up
    if(y <= 4){
      hitBottom = 1;
      hitTop = 0;
    }
    //If x is greater than or equal to 123 hit the right wall go left
    if(x >= 123){
      hitRight = 0;
      hitLeft = 1;
    }
    //If x is less then or equal to 4 hit the left wall go right
    if(x <= 4){
      hitRight = 1;
      hitLeft = 0;
    }
  }
  
  //Serial.println(encoderValue);
}


void updateEncoder(){//HANDLER DEBOUNCING OF ROTARY ENCODER
  int currTime = millis();    
  if(currTime - lastTime > 100)
  {
    if(digitalRead(encoderPin2) == HIGH){
      encoderValue = encoderValue - 1;
    }else{
      encoderValue = encoderValue + 1;
    }
    lastTime = currTime;
    delay(50);
  }
}
