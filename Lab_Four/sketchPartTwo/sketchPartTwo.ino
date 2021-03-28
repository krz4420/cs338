#include "concurrency.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
lock_t *lock = lock_create();
const int OLED_CS = 8;
const int OLED_DC = 9;
const int OLED_RST = 10;
const int OLED_SI = 11;
const int OLED_CLK = 12;

int x = 4;
int y = 4;

int xOne = 59;
int yOne = 123;

int hitBottom = 0;
int hitTop = 1;
int hitLeft = 1;
int hitRight = 0;

int hitBottomO = 1;
int hitTopO = 0;
int hitLeftO = 0;
int hitRightO = 1;


Adafruit_SSD1306 disp(128,64,OLED_SI,OLED_CLK,OLED_DC,OLED_RST,OLED_CS);


void p1 (void)
{ 
    while(1){
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
      
      lock_acquire(lock);
      disp.drawCircle(x, y, 4, WHITE);
      disp.display();
      delay(90);
      lock_release(lock);

      
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
}

void p2 (void)
{ 
 
    while(1){
      
      if(hitLeftO == 0){
        xOne += 4;
      }
  
      if(hitRightO == 0){
        xOne -= 4;
      }
  
      if(hitBottomO == 0){
        yOne -= 4;
      }
  
      if(hitTopO == 0){
        yOne += 4;
      }
      
      lock_acquire(lock);
      disp.drawCircle(xOne, yOne, 4, WHITE);
      disp.display();
      delay(90);
      lock_release(lock);
      
      
      //Draw the screen black to help make the balls look like they moving
      lock_acquire(lock);
      disp.fillScreen(BLACK);
      lock_release(lock);
      
      // If y is greater than or eaqual to 59 then it has hit the top move down
      if(yOne >= 59){
        hitBottomO = 0;
        hitTopO = 1;
      }
      //If y is less than or equal to 0 then it has hit the bottom move up
      if(yOne <= 4){
        hitBottomO = 1;
        hitTopO = 0;
      }
      //If x is greater than or equal to 123 hit the right wall go left
      if(xOne >= 123){
        hitRightO = 0;
        hitLeftO = 1;
      }
      //If x is less then or equal to 4 hit the left wall go right
      if(xOne <= 4){
        hitRightO = 1;
        hitLeftO = 0;
      }
    }
}

void setup()
{
  Serial.begin(9600);
  disp.begin(SSD1306_SWITCHCAPVCC);
  disp.clearDisplay();
  disp.display();
  
  if (process_create (p1, 64) < 0) {
    return;
  }

  if (process_create (p2, 64) < 0) {
    return;
  }
  
}

void loop()
{ 
  process_start();
 
  /* if you get here, then all processes are either finished or
     there is deadlock */
  while (1) ;
}
