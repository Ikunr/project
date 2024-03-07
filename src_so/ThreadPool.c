#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> //锁
#include <unistd.h>
#include "ThreadPool.h"
#include "DoubleLinkList.h"
#include "Queue.h"
#include "StdThread.h"

#define true 1
#define false 0
#define Sleep_time 20

struct Task // 任务的结构体   一个线程函数 和线程函数的参数
{
    void *(*function)(void *); // 线程函数的函数指针
    void *arg;
};


typedef struct Task task;

task *CreateTask(void *(*func)(void *), void *arg) // 创任务 把任务的参数给他
{
    task *t = (task *)malloc(sizeof(task));
    if (t == NULL)
    {
        printf("CreateTask malloc error!\n");
        return NULL;
    }
    t->function = func;
    t->arg = arg;

    return t;
}

void FreeTask(task *t)
{
    free(t);
}

// 一个放线程的链表   任务队列
struct ThreadPool
{
    DLlist threads;    // 线程链表
    LQueue task_queue; // 任务队列 也不能无线的增长

    // 线程的约束
    int min_thrd_num; // 区间 最少的线程 具体业务来定  调你线程迟库来决定
    int max_thrd_num; // 最大线程

    // 任务队列的约束
    int max_queue_size; // 不能允许无限制的任务 避免有恶意刷服务器

    int busy_thrd_num; // 正在忙的线程数量
    int exit_thrd_num; // 应该退出的线程的数量

    pthread_mutex_t pool_mutex;      // 可能会出现俩个线程同时抢一个任务 所以要加个锁
    pthread_mutex_t busy_thrd_mutex; // 计算正在忙的锁

    // 条件变量锁
    pthread_cond_t queue_not_empty; // 队列任务不为空激发条件变量
    pthread_cond_t queue_not_full;  // 队列不满

    Thread *admin_thread; // 线程管理 时不时回来看一下线程的情况

    int shutdown; // 关闭
};

void *thread_worker(void *arg) // 拿任务 做任务  交任务  等任务
{
    ThreadP *p = (ThreadP *)arg;

    while (1) // 循环抢锁 没任务等待 有任务就做
    {
        pthread_mutex_lock(&p->pool_mutex); // 抢锁

        while (IsQEmty(&p->task_queue) == true && p->shutdown == false) // 判断队列任务是否为空 如果为空就一直等  如果我没抢到就一直等
        {

            pthread_cond_wait(&p->queue_not_empty, &p->pool_mutex); // 所以人都卡在条件变量  俩种情况 一是真的有任务 二是自杀

            if (p->exit_thrd_num > 0) // 如果大于0优先自杀
            {
                p->exit_thrd_num--;

                struct Node *TravelPoint = p->threads.head; // 每条线程的指针  遍历找要被杀掉的那个线程
                while (TravelPoint != NULL)
                {
                    Thread *t = (Thread *)TravelPoint->data;
                    if (GetThreadId(t) == pthread_self()) // 如果获得线程的ID号 和返回当前线程的线程号相同就把他删掉
                    {
                        RemoveByElement(&p->threads, t);
                        break;
                    }

                    TravelPoint = TravelPoint->next;
                }

                pthread_mutex_unlock(&p->pool_mutex);
                pthread_exit(NULL);
            }
        }

        if (p->shutdown == true) // 任务不做了
        {
            pthread_mutex_unlock(&p->pool_mutex); // 解锁
            pthread_exit(NULL);                   // 退线程
        }

        // 如果队列里有任务

        task *tk = (task *)(*QPop(&p->task_queue)); // 弹出来的是万能指针的的二级指针  先取值在强转
        // printf("len :%d\n",GetQueueLen(&p->task_queue));

        pthread_mutex_unlock(&p->pool_mutex); // 再争夺完公共资源 抢到任务的时候把锁放掉

        pthread_cond_broadcast(&p->queue_not_full); // 给下面增加的函数发个不满的信号

        pthread_mutex_lock(&p->busy_thrd_mutex); // 先加锁
        p->busy_thrd_num++;
        // printf(" num ++ :%d\n",p->busy_thrd_num);
        pthread_mutex_unlock(&p->busy_thrd_mutex); // 在解锁

        tk->function(tk->arg); // 任务的执行过程

        pthread_mutex_lock(&p->busy_thrd_mutex);
        p->busy_thrd_num--;
        // printf(" num  --:%d\n",p->busy_thrd_num);
        pthread_mutex_unlock(&p->busy_thrd_mutex);

        FreeTask(tk); // 任务做完放掉
    }
}

void *thread_manager(void *arg) // 管理者线程
{
    ThreadP *p = (ThreadP *)arg;
    while (p->shutdown != true) // 我想让他停的时候就停 比如销毁线程 怎么让系统停下来 加个标志位
    {
        sleep(Sleep_time); // 10秒钟视察一次
        pthread_mutex_lock(&p->pool_mutex);

        int queueLen = GetQueueLen(&p->task_queue);
        int thread_num = GetListLen(&p->threads);

        // 创线程 任务太多了 所以线程都在工作 还有任务在等待   一条思路 ：里面正在忙的数量 < 外面等待的任务
        if (p->busy_thrd_num < queueLen && thread_num < p->max_thrd_num)
        {
            int add = (queueLen / 2) > (p->max_thrd_num - thread_num) ? (p->max_thrd_num - thread_num) : (queueLen / 2); // 加一半的任务的 外面等待的除以2 线程
            for (int i = 0; i < add; i++)
            {
                Thread *t = InitThread(thread_worker, p);
                InsertTail(&p->threads, t);
            }
        }

        // 销毁线程
        thread_num = GetListLen(&p->threads); // 线程的数量比工作的数量的2倍还要多就就销毁
        if (thread_num > p->busy_thrd_num * 2 && thread_num > p->min_thrd_num)
        {
            int minus_thrd_num = (thread_num - p->min_thrd_num) / 2; // 总的线程 减去正在忙的除以2
            int minus = minus_thrd_num > (thread_num - p->busy_thrd_num) ? (thread_num - p->min_thrd_num) : minus_thrd_num;

            p->exit_thrd_num = minus; // 我要减的数量
            pthread_mutex_unlock(&p->pool_mutex);
            for (int i = 0; i < minus; i++)
            {
                pthread_cond_broadcast(&p->queue_not_empty); // 唤醒正在等待的进程 条件变量
            }
            continue;
        }

        pthread_mutex_unlock(&p->pool_mutex);
    }
    pthread_exit(NULL);
}

ThreadP *InitThreadPool(int max_thrd_num, int min_thrd_num, int max_queue_num)
{
    ThreadP *p = (ThreadP *)malloc(sizeof(ThreadP));
    if (p == NULL)
    {
        printf("InitThreadPool malloc error!\n");
        return NULL;
    }

    InitDLlist(&p->threads);
    InitQueue(&p->task_queue);

    p->max_thrd_num = max_thrd_num;
    p->min_thrd_num = min_thrd_num;
    p->max_queue_size = max_queue_num;
    p->busy_thrd_num = 0;
    p->exit_thrd_num = 0;

    pthread_mutex_init(&p->pool_mutex, NULL);      // 动态锁初始化
    pthread_mutex_init(&p->busy_thrd_mutex, NULL); // 忙的锁的初始化
    pthread_cond_init(&p->queue_not_empty, NULL);  // 动态
    pthread_cond_init(&p->queue_not_full, NULL);   // 动态

    p->shutdown = false;

    for (int i = 0; i < p->max_thrd_num; i++) // 创线程 按最大的创
    {
        Thread *t = InitThread(thread_worker, p); // 从线程池队列拿任务 把整个线程池的结构体传给他 要什么你自己拿
        InsertTail(&p->threads, t);               // 创完之后 放到链表里面
    }

    p->admin_thread = InitThread(thread_manager, p); // 把整个线程池给他

    return p;
}

void ThreadP_AddTask(ThreadP *p, void *(*func)(void *), void *arg)
{
    pthread_mutex_lock(&p->pool_mutex); // 是公共资源 一帮线程等队列 加锁
    // 任务队列满了不让你加了   任务队列满了你等着
    while (GetQueueLen(&p->task_queue) == p->max_queue_size) // 如果任务队列的长度和 最大允许的相等
    {
        pthread_cond_wait(&p->queue_not_full, &p->pool_mutex); // 让他在满的地方等着
    }

    if (p->shutdown == true)
    {
        pthread_mutex_unlock(&p->pool_mutex);
        return;
    }

    QPush(&p->task_queue, CreateTask(func, arg)); // 如果不相等 就 入任务队列
    pthread_cond_broadcast(&p->queue_not_empty);  // 有一堆线程在等 在上锁发个信号我有任务了 可以执行了 传线程等的锁
    pthread_mutex_unlock(&p->pool_mutex);         // 最后在解锁
}

void DestoryThreadPool(ThreadP *p)
{
    if (p == NULL)
    {
        return;
    }
    // 顺序  越晚 就先退
    p->shutdown = true;          // 为真的时候就把管理者线程就退出了
    JoinThread(p->admin_thread); // 回收资源
    free(p->admin_thread);       // 释放资源

    int len = GetListLen(&p->threads); // 把不干活的 死
    for (int i = 0; i < len; i++)
    {
        pthread_cond_broadcast(&p->queue_not_empty); // 唤醒那些不干活的 死
    }

    struct Node *travelPoint = p->threads.head; // 遍历还在干活的 干完就死
    while (travelPoint != NULL)
    {
        Thread *t = (Thread *)travelPoint->data;
        JoinThread(t);
        free(t);
        travelPoint = travelPoint->next;
    }

    FreeDLlist(&p->threads); // 所有线程里的都放完了 把线程放了

    while (IsQEmty(&p->task_queue) != true) // 把队列任务销毁
    {
        task *t = (task *)(*QPop(&p->task_queue));
        free(t);
    }
    FreeQueue(&p->task_queue);

    pthread_mutex_destroy(&p->pool_mutex);
    pthread_mutex_destroy(&p->busy_thrd_mutex);

    pthread_cond_destroy(&p->queue_not_empty);
    pthread_cond_destroy(&p->queue_not_full);
}
