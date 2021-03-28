#include "concurrency.h"

lock_t *lock = lock_create();

void p1 (void)
{
  lock_acquire(lock);
  
  /* ENTERING CRITICAL SECION */
  cli(); 
  Serial.println("Process 1");
  sei();
  delay(1000);
   /* EXITING CRITICAL SECTION */
  lock_release(lock);

  lock_acquire(lock);
  cli(); 
  Serial.println("Process 1.1");
  Serial.println("Process 1:2");
  sei();
  delay(1000);
   /* EXITING CRITICAL SECTION */
  lock_release(lock);

  lock_acquire(lock);
  cli(); 
  Serial.println("Process 1.1");
  Serial.println("Process 1:2");
  Serial.println("Process 1.3");
  sei();
  delay(1000);
   /* EXITING CRITICAL SECTION */
  lock_release(lock);
  
  
  
  return;
}

void p2 (void)
{ 
  lock_acquire(lock);
  /*ENTERING CRITICAL SECTION*/
  cli();
  Serial.println("Process 2");
  sei();
  delay(1000);
  lock_release(lock);
  
  
  lock_acquire(lock);
  cli(); 
  Serial.println("Process 2.1");
  Serial.println("Process 2.2");
  Serial.println("Process 2.3");
  sei();
  delay(1000);
  lock_release(lock);
  
  
  /* EXITING CRITICAL SECTION */
  
  lock_release(lock);
  return;
}

void p3 (void)
{ 
  lock_acquire(lock);
  /*ENTERING CRITICAL SECTION*/
  cli();
  Serial.println("Process 3");
  sei();
  delay(1000);
  lock_release(lock);

  cli();
  Serial.println("Process 3");
  Serial.println("Process 3.2");
  sei();
  delay(1000);
  lock_release(lock);
  
 
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
  if (process_create (p3, 64) < 0) {
    return;
  }
}

void loop()
{ 
  process_start();
  cli();
  Serial.println("Got here!");
  sei();
  delay(1000);
  /* if you get here, then all processes are either finished or
     there is deadlock */
  while (1) ;
}
