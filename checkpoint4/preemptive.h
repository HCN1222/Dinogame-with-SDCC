#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4  /* not including the scheduler */
/* the scheduler does not take up a thread of its own */

#define CNAME(s) _ ## s

// create a counting semaphore s that is initialized to n
#define SemaphoreCreate(s, n)\
            s = n

// do (busy-)wait() on semaphore s
#define SemaphoreWaitBody(s, label)\
    { __asm \
        label:            ;; top of while-loop \
        MOV A, CNAME(s)   ;; read value of _S into ACC (where S is semaphore) \
        JZ  label         ;; conditionally jump(s) back to label if ACC <= 0  \
        JB ACC.7, label \
                          ;; fall-through to drop out of while-loop\
        dec  CNAME(s) \
      __endasm; }

// signal() semaphore s
#define SemaphoreSignal(s)\
            __asm \
                INC CNAME(s)\
            __endasm;

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadExit(void);
void myTimer0Handler();

#endif // __PREEMPTIVE_H__