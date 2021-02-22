/*
 * @Author: your name
 * @Date: 2021-02-19 17:43:31
 * @LastEditTime: 2021-02-19 19:29:29
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /coroutine/coroutine1.h
 */

#ifndef INCLUDE_COROUTINUE_1_H
#define INCLUDE_COROUTINUE_1_H

#include<ucontext.h>
#include<stddef.h>

/* 默认协程栈大小 */
#define DEAFAULT_STACK_SIZE 1024*1024

/* 协程池数目 */
#define COROUTINE_NUM 16

/* 协程状态 */
enum CoroutineState{COROUTINE_DEAD,COROUTINE_READY,COROUTINE_RUNING,COROUTINE_SUSPEND};

/* 调度器结构体 */
struct schedule_t;

/* 函数指针 */
typedef void (*CoroutineFunc)(schedule_t*,void *);

/* 协程结构体 */
typedef struct _coroutine_t
{
    /* 协程上下文 */
    ucontext_t ctxt; 
    /* 协程函数 */
    CoroutineFunc func;
    /* 函数参数 */
    void *arg;
    /* 协程所属调度器 */
    schedule_t *sch;
    /* 协程状态 */
    CoroutineState state;
    /* 协程栈 */
    char *stack;
    /* 已经分配的内存大小 */
    ptrdiff_t cap;
    /* 当前协程的运行时栈，保存起来时的大小 */
    ptrdiff_t size;
}coroutine_t;

/* 协程调度结构体 */
typedef struct schedule_t
{
    /* 协程共享栈 */
    char stack[DEAFAULT_STACK_SIZE];
    /* 主协程上下文 */
    ucontext_t main;
    /* 存活的协程个数 */
    int nco;
    /* 可管理的最大协程数目 */
    int cap;
    /* 运行协程id */
    int runId;
    /* 协程池 */
    coroutine_t **coroutines;
};

/* 协程调度器创建函数 */
schedule_t *schedule_open();

/* 协程调度器关闭函数 */
void schedule_close(schedule_t*sch);

/* 协程创建函数 */
int coroutine_create(schedule_t *sch,CoroutineFunc func,void *arg);

/* 协程运行/恢复函数 */
int coroutine_resume(schedule_t *sch,int id);

/* 协程挂起函数 */
int coroutine_yield(schedule_t *sch);

/* 获取协程状态 */
CoroutineState coroutine_state(schedule_t *sch,int id);

/* 获取运行协程id */
int coroutine_runId(schedule_t *sch);

#endif //INCLUDE_COROUTINUE_1_H