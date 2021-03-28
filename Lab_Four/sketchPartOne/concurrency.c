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

//Struct for state
struct process_state{
        unsigned int sp; /* stack pointer */
        struct process_state *next; /* link to next process */
   };
/*
 * Global variables for the current_process and the ready queue to hold the values of the processes that are ready to execute.
 */
process_t *current_process = NULL;
process_t *readyQueue = NULL;

//
/* Creates new Process
 * 
 * Initialize a variable to hold the value of the stack pointer.
 * Call process_init(); if the value of stackPointer is greater than 0 that mean space was given for stack.
 * malloc space for the process_t struct and add this process to the ready queue.
 */
int process_create (void (*f)(void), int n){
	// save value returned from process_init if successful

	int stackPointer = 0;
	stackPointer = process_init(f,n);
	if(stackPointer > 0){
		// create a new process state to hold value of this new process
		process_t *newState = malloc(sizeof(process_t));
		// malloc did not fail
		if(newState != NULL){
			newState->sp = stackPointer;
			newState->next = NULL;
			addState(newState);
			// SUCCESS
			return 0;
		}
	}
	// FAILED
	return -1;
}


/* 
 *  add the newState to the processState ready queue
 *  If the ready queue is NULL then make this process the head of ready queue
 *  Else use temp to traverse down the linked list and add this process to the end of the queue
*/
void addState(process_t *nextState){
	//If there are no current processes
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

/*
 * Begin processes
 */
void process_start (void){
  process_begin();
}


/* Process Select Function
 * 
 * If the cursp is greater than 0 then the current_process has not stopped executing.
 * Check that the current_process is not NULL. If it is not then update its sp value with the value of cursp.
 * Set its next value to NULL so that there are no hanging around pointers anywhere. Then add it to the end of the ready queue
 * to be exectued again when it is its turn.
 * 
 * Check that the ready queue is not NULL. If it is not then a process is ready to be executed. Save the value of that sp to be
 * returned. Set the current process to the value of head of the ready queue and update the value of the head of the ready queue.
 * return that saved stack pointer value sp.
 * 
 * If the ready queue is NULL then there are no processes ready to be executed. Update the value of current_process and return 0.
 */
unsigned int process_select (unsigned int cursp){
  if(cursp > 0){
    if(current_process != NULL){
      current_process->sp = cursp;
      current_process->next = NULL;
      addState(current_process);
    }
  }

  if(readyQueue != NULL){
    int returnVal = readyQueue->sp;
    current_process = readyQueue;
    readyQueue = readyQueue->next;
    return returnVal;
  }else{
    current_process = NULL;
    return 0;
  }

 
}
