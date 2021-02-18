#include"coroutine.h"

int coroutine_create(schedule_t &schedule,Func func,void *arg)
{
    /* 找到第一个可用的协程 */
    int id=0;
    for(;id<schedule.maxIndex;++id)
    {
        if(schedule.coroutines[id].state==FREE)
        {
            break;
        }
    }
    if(id==schedule.maxIndex)
    {
        schedule.maxIndex++;
    }
    /* 设置函数及参数 */
    coroutine_t &c=schedule.coroutines[id];
    c.func=func;
    c.arg=arg;
    c.state=RUNABLE;
    /* 返回协程id */
    return id;
}

void coroutine_start(schedule_t *ps)
{
    int id=ps->runId;
    if(id!=-1)
    {
        coroutine_t &c=ps->coroutines[id];
        c.func(c.arg);
        c.state=FREE;
        ps->runId=-1;
    }
}

int coroutine_resume(schedule_t &schedule,int id)
{
    if(id<0||id>schedule.maxIndex) return 1;
    coroutine_t &c=schedule.coroutines[id];
    switch (c.state)
    {
        case RUNABLE:
        {
            c.state=RUNING;
            getcontext(&(c.ctxt));
            c.ctxt.uc_link=&(schedule.main);
            c.ctxt.uc_stack.ss_flags=0;
            c.ctxt.uc_stack.ss_sp=c.stack;
            c.ctxt.uc_stack.ss_size=DEAFAULT_STACK_SIZE;
            schedule.runId=id;
            makecontext(&(c.ctxt),(void(*)())coroutine_start,1,&schedule);
        }
        case SUSPEND:
        {
            swapcontext(&(schedule.main),&(c.ctxt));
        }
        default:
        {
            return 1;
        }
    }
    return 0;
}

int coroutine_yield(schedule_t &schedule)
{
    if(schedule.runId!=-1)
    {
        coroutine_t &c=schedule.coroutines[schedule.runId];
        c.state=SUSPEND;
        schedule.runId=-1;
        swapcontext(&(c.ctxt),&(schedule.main));
    }
    return 0;
}

int is_schedule_finished(schedule_t &schedule)
{
    if(schedule.runId!=-1) return 0;
    for(int id=0;id<schedule.maxIndex;++id)
    {
        if(schedule.coroutines[id].state!=FREE)
        {
            return 0;
        }
    }
    return 1;
}