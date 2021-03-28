#include "concurrency.h"


void p1 (void)
{
  cli(); 
  Serial.println("Process 1");
  sei();
  delay(1000);
  cli(); 
  Serial.println("Process 1");
  sei();
  delay(1000);
  cli(); 
  Serial.println("Process 1");
  sei();

  delay(1000);
   cli(); 
  Serial.println("Process 1");
  sei();
  delay(1000);
  cli(); 

  Serial.println("Process 1");
  sei();

  delay(1000);
   cli(); 
  Serial.println("Process 1");
  sei();
  delay(1000);
  cli(); 
  Serial.println("Process 1");
  sei();
  delay(1000);
   cli(); 
  Serial.println("Process 1");
  sei();
  delay(1000);
  return;
}

void p2 (void)
{ 
  Serial.println("Process 2");
  sei();
  delay(1000);
  cli();
  Serial.println("Process 2");
  sei();
  delay(1000);
  cli(); 
  Serial.println("Process 2");
  sei();
  delay(1000);
  cli();
  Serial.println("Process 2");
  sei();
  delay(1000);
  return;
}

void setup()
{
  Serial.begin(9600);
  if (process_create (p1, 64) < 0) {
    return;
  }
  
  if (process_create (p2, 64) < 0) {
    return;
  }
  pinMode(13, OUTPUT);

}

void loop()
{ 
//  digitalWrite (13, HIGH);
//  delay(400);
//  digitalWrite (13, LOW);
//  delay(400);
  process_start();
  cli();
  Serial.println("Got here!");
  sei();
  delay(1000);
  /* if you get here, then all processes are either finished or
     there is deadlock */
  while (1) ;
}
