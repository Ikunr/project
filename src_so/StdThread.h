#ifndef __STDTHREAD_H__
#define __STDTHREAD_H__

typedef struct StdThread Thread;

Thread *InitThread(void *(*func)(void *), void *arg);
unsigned long int GetThreadId(Thread *t);
void *JoinThread(Thread *t);
void DetachThread(Thread *t);
void CancelThread(Thread *t);
void ClearThread(Thread *t);


#endif