#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <Arduino.h>
#include "concurrency.h"

__attribute__((used)) unsigned char _orig_sp_hi, _orig_sp_lo;

__attribute__((used)) void process_begin ()
{
  asm volatile (
    "cli \n\t"
    "in r24,__SP_L__ \n\t"
    "sts _orig_sp_lo, r24\n\t"
    "in r25,__SP_H__ \n\t"
    "sts _orig_sp_hi, r25\n\t"
    "ldi r24, 0\n\t"
    "ldi r25, 0\n\t"
    "rjmp .dead_proc_entry\n\t"
    );
}

__attribute__((used)) void process_terminated ()
{
  asm volatile (
    "cli\n\t"
    "lds r25, _orig_sp_hi\n\t"
    "out __SP_H__, r25\n\t"
    "lds r24, _orig_sp_lo\n\t"
    "out __SP_L__, r24\n\t"
    "ldi r24, lo8(0)\n\t"
    "ldi r25, hi8(0)\n\t"
    "jmp .dead_proc_entry"
    );
}

void process_timer_interrupt ();
int process_create (void (*f)(void), int n);
void addState(process_t *nextState);
int printProcessList();
void process_start (void);

__attribute__((used)) void yield ()
{
  if (!current_process) return;
  asm volatile ("cli\n\t");
  asm volatile ("rjmp process_timer_interrupt\n\t");
}

__attribute__((used)) void process_timer_interrupt()
{
  asm volatile (
		"push r31\n\t"
		"push r30\n\t"
		"push r29\n\t"
		"push r28\n\t"
		"push r27\n\t"
		"push r26\n\t"
		"push r25\n\t"
		"push r24\n\t"
		"push r23\n\t"
		"push r22\n\t"
		"push r21\n\t"
		"push r20\n\t"
		"push r19\n\t"
		"push r18\n\t"
		"push r17\n\t"
		"push r16\n\t"
		"push r15\n\t"
		"push r14\n\t"
		"push r13\n\t"
		"push r12\n\t"
		"push r11\n\t"
		"push r10\n\t"
		"push r9\n\t"
		"push r8\n\t"
		"push r7\n\t"
		"push r6\n\t"
		"push r5\n\t"
		"push r4\n\t"
		"push r3\n\t"
		"push r2\n\t"
		"push r1\n\t"
		"push r0\n\t"
		"in r24, __SREG__\n\t"
		"push r24\n\t"
		"in r24, __SP_L__\n\t"
		"in r25, __SP_H__\n\t"
		".dead_proc_entry:\n\t"
		"rcall process_select\n\t"
		"eor r18,r18\n\t"
		"or r18,r24\n\t"
		"or r18,r25\n\t"
		"brne .label_resume\n\t"
		"lds r25, _orig_sp_hi\n\t"
		"out __SP_H__, r25\n\t"
		"lds r24, _orig_sp_lo\n\t"
		"out __SP_L__, r24\n\t"
		"ret\n\t"
		".label_resume:\n\t"
		"out __SP_L__, r24\n\t"
		"out __SP_H__, r25\n\t"
		"pop r0\n\t"
		"out  __SREG__, r0\n\t"
		"pop r0\n\t"
		"pop r1\n\t"
		"pop r2\n\t"
		"pop r3\n\t"
		"pop r4\n\t"
		"pop r5\n\t"
		"pop r6\n\t"
		"pop r7\n\t"
		"pop r8\n\t"
		"pop r9\n\t"
		"pop r10\n\t"
		"pop r11\n\t"
		"pop r12\n\t"
		"pop r13\n\t"
		"pop r14\n\t"
		"pop r15\n\t"
		"pop r16\n\t"
		"pop r17\n\t"
		"pop r18\n\t"
		"pop r19\n\t"
		"pop r20\n\t"
		"pop r21\n\t"
		"pop r22\n\t"
		"pop r23\n\t"
		"pop r24\n\t"
		"pop r25\n\t"
		"pop r26\n\t"
		"pop r27\n\t"
		"pop r28\n\t"
		"pop r29\n\t"
		"pop r30\n\t"
		"pop r31\n\t"
		"reti\n\t");
}


/*
 * Stack: save 32 regs, +2 for entry point +2 for ret address
 */
#define EXTRA_SPACE 37
#define EXTRA_PAD 4

unsigned int process_init (void (*f) (void), int n)
{
  unsigned long stk;
  int i;
  unsigned char *stkspace;

  /* Create a new process */
  n += EXTRA_SPACE + EXTRA_PAD;
  stkspace = (unsigned char *) malloc (n);

  if (stkspace == NULL) {
    /* failed! */
    return 0;
  }

  /* Create the "standard" stack, including entry point */
  for (i=0; i < n; i++) {
      stkspace[i] = 0;
  }

  n -= EXTRA_PAD;

  stkspace[n-1] = ( (unsigned int) process_terminated ) & 0xff;
  stkspace[n-2] = ( (unsigned int) process_terminated ) >> 8;
  stkspace[n-3] = ( (unsigned int) f ) & 0xff;
  stkspace[n-4] = ( (unsigned int) f ) >> 8;

  /* SREG */
  stkspace[n-EXTRA_SPACE] = SREG;

  stk = (unsigned int)stkspace + n - EXTRA_SPACE - 1;

  return stk;
}



// ____________Global Variables______________
process_t *current_process = NULL;

// Point to the first element in the waiting Q
process_t *readyQueue = NULL;

// _____________________________________________

// Struct for locks
struct lock_state {
        int locked; // Whether lock is locked or not
        process_t *waitingQ; // Queue for waiting processes on lock
};


// Struct for process state
struct process_state{
        unsigned int sp; /* stack pointer */
        struct process_state *next; /* link to next process */
        struct lock_state *lock; /*The lock that is being used by this currnet process */
   };
   


// Function to initialize lock data struct
void lock_init (lock_t *l){
    l->locked = 0; // 1 it is locked 0 it is not
    l->waitingQ = NULL; //Creating waiting queue for each lock struct
}


/* Function to acquire lock
 * 
 * If the lock is not locked then the current process wants the lock. Lock the lock and let the current process continue with the lock.
 * If the lock is locked then the current process wants the lock but another process has the lock. Add the current process to the waiting
 * queue for the lock. 
 */
void lock_acquire(lock_t *l){
  
  if(l->locked == 0){
    current_process->lock = l;
    l->locked = 1;
  }else{ //it is locked
    process_t *temp = l->waitingQ;
    //Brings to end of queue
    while(temp->next != NULL){
      temp = temp->next;
    }
    temp->next = current_process;
    
  }
}


 /* Function to release lock
  *  
  * If the waiting queue for the lock is not NULL then there are processes waiting for this lock.
  * Take the first waiting process of the waiting queue update the head of the waiting queue.
  * Then push the process that was just popped off the waiting queue on the the head of the ready queue
  * and call yield so that process_select() will be called and it will run select that new process to be executed.
  * 
  * If the waiting queue is empty then unlock the lock and call yield as other processes may be ready to execute
  * that are not waiting on this lock.
  */
void lock_release(lock_t *l){
  //Waiting queue is not NULL
  if(l->waitingQ != NULL){
    // Update head of waiting queue for this lock
    process_t *temp = l->waitingQ;
    l->waitingQ = l->waitingQ->next;

    //Push the newly popped process onto the head of readyQueue and call yield so it gets called
    if(readyQueue != NULL){
      temp->next = readyQueue;
      readyQueue = temp;
    }else{
      readyQueue = temp;
    }
    yield();
  }else{ // Waiting queue is empty
    l->locked = 0;
    // Still yield because there could be other processes on the ready queue
    yield();
  }
}

/*
 * Function to create lock in the .ino file
 */
lock_t* lock_create(){
  lock_t *lock = malloc(sizeof(lock_t));
  if (lock != NULL){
    lock_init(lock);
    return lock;
  }
 
}

// Function to Create a new Process and add it to Ready Q
int process_create (void (*f)(void), int n){
  
	// save value returned from process_init if successful
	int stackPointer = 0;
	stackPointer = process_init(f,n);
  
  // Check stack pointer value
	if(stackPointer > 0){
  
		// create a new process state to hold value of this new process
		process_t *newState = malloc(sizeof(process_t));
    
		// malloc did not fail
		if(newState != NULL){
     
			newState->sp = stackPointer;
			newState->next = NULL;
      newState->lock = NULL;
			addState(newState);
 			// SUCCESS
			return 0;
		}
	}
	// FAILED
	return -1;
}


/* 
add the newState to the processState linked list for later use
-go to current process
-go down the linked list until it is NULL
-set it to that processes next
DO NOT USE CURRENT PROCESS POINTER TO TRAVERSE THE LINKED LIST
*/
void addState(process_t *nextState){
  // The readyQueue is empty
	if(readyQueue == NULL){
		readyQueue = nextState;
    return;
	}

	// Create temp variable to go down linked list
	process_t *temp = readyQueue;
 
	// Go until find end of linked list
	while(temp->next != NULL){
		temp = temp->next;
	}
	temp->next = nextState;
  return;
}

void process_start (void){
  process_begin();
}


// Returns the val of the sp for the next ready process
unsigned int process_select (unsigned int cursp){
  
  if(cursp > 0){
    // If there is a current process then add to back of the Q
    if(current_process != NULL){
      current_process->sp = cursp;
      current_process->next = NULL;

      addState(current_process);
    }
  }
 
  // If there are processes in the ready Q 
  if(readyQueue != NULL){
    int returnVal = readyQueue->sp;     // return sp of next ready process
    current_process = readyQueue;       // Set current process equal to ready Q
    readyQueue = readyQueue->next;      // Go to next process in Q and load it 
    return returnVal;
  }else{
    current_process = NULL;
    return 0;
  }
}
