#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

struct ThreadPool;
typedef struct ThreadPool ThreadP;

ThreadP *InitThreadPool(int max_thrd_num, int min_thrd_num, int max_queue_num); // 线程池所需要的三个参数 结构体的俩个链表不需要初始化
void ThreadP_AddTask(ThreadP *p, void *(*func)(void *), void *arg);             // 塞任务
void DestoryThreadPool(ThreadP *p);                                             // 销毁线程迟


#endif