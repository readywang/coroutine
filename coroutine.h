/*
 * @Author: readywang
 * @Date: 2021-02-18 09:52:15
 * @LastEditTime: 2021-02-18 16:57:24
 * @LastEditors: Please set LastEditors
 * @Description: asymmetric coroutine by c++
 * @FilePath: /coroutine/coroutine.h
 */

#ifndef INCLUDE_COROUTINUE_H
#define INCLUDE_COROUTINUE_H

#include<ucontext.h>

/* 默认协程栈大小 */
#define DEAFAULT_STACK_SIZE 1024*128

/* 协程池数目 */
#define COROUTINE_NUM 1024

/* 协程状态 */
enum STATE{FREE,RUNABLE,RUNING,SUSPEND};

/* 函数指针 */
typedef void (*Func)(void *);

/* 协程结构体 */
typedef struct _coroutine_t
{
    /* 协程上下文 */
    ucontext_t ctxt; 
    /* 协程函数 */
    Func func;
    /* 函数参数 */
    void *arg;
    /* 协程状态 */
    STATE state;
    /* 协程栈 */
    char stack[DEAFAULT_STACK_SIZE];
}coroutine_t;

/* 协程调度结构体 */
typedef struct _schedule_t
{
    /* 主协程上下文 */
    ucontext_t main;
    /* 运行协程id */
    int runId;
    /* 协程池 */
    coroutine_t *coroutines;
    /* 最大下标 */
    int maxIndex;
    _schedule_t():runId(-1),maxIndex(0)
    {
        coroutines=new coroutine_t[COROUTINE_NUM];
        for(int i=0;i<COROUTINE_NUM;++i)
        {
            coroutines[i].state=FREE;
        }
    }
    ~_schedule_t()
    {
        delete coroutines;
    }
}schedule_t;

/* 协程创建函数 */
int coroutine_create(schedule_t &schedule,Func func,void *arg);

/* 协程启动工具函数 */
void coroutine_start(schedule_t *ps);

/* 协程运行/恢复函数 */
int coroutine_resume(schedule_t &schedule,int id);

/* 协程挂起函数 */
int coroutine_yield(schedule_t &schedule);

/* 判断所有协程是否执行完毕 */
int is_schedule_finished(schedule_t &schedule);

#endif //INCLUDE_COROUTINUE_H