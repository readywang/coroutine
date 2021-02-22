
#include"coroutine1.h"
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<stdint.h>

/* 协程创建工具函数 */
coroutine_t *_co_new(schedule_t *sch,CoroutineFunc func,void *arg)
{
    coroutine_t *co=new coroutine_t;
    co->func=func;
    co->arg=arg;
    co->sch=sch;
    co->cap=0;
    co->size=0;
    co->stack=NULL;
    co->state=COROUTINE_READY;
    return co;
}

/* 协程删除工具函数 */
void _co_delete(coroutine_t *co)
{
    free(co->stack);
    delete co;
}

schedule_t *schedule_open()
{
    schedule_t *sch=new schedule_t;
    sch->nco=0;
    sch->cap=COROUTINE_NUM;
    sch->runId=-1;
    sch->coroutines=(coroutine_t **)malloc(sch->cap*sizeof(coroutine_t *));
    memset(sch->coroutines,0,sch->cap*sizeof(coroutine_t *));
    return sch;
}

void schedule_close(schedule_t*sch)
{
    for(int i=0;i<sch->cap;++i)
    {
        coroutine_t *co=sch->coroutines[i];
        if(co)
        {
            _co_delete(co);
        }
    }
    free(sch->coroutines);
    sch->coroutines=NULL;
    delete sch;
}

int coroutine_create(schedule_t *sch,CoroutineFunc func,void *arg)
{
    /* 创建一个协程 */
    coroutine_t *co=_co_new(sch,func,arg);
    if(sch->nco>=sch->cap)
    {
        /* 进行扩容 */
        int id=sch->cap;
        sch->coroutines=(coroutine_t **)realloc(sch->coroutines,sizeof(coroutine_t *)*sch->cap*2);
        memset(sch->coroutines+sch->cap,0,sizeof(coroutine_t *)*sch->cap);
        sch->coroutines[id]=co;
        sch->cap*=2;
        ++sch->nco;
        return id;
    }
    else
    {
        /* 找到一个NULL位置 */
        for(int i=0;i<sch->cap;++i)
        {
            int id=(i+sch->nco)%sch->cap;
            if(NULL==sch->coroutines[id])
            {
                sch->coroutines[id]=co;
                sch->nco++;
                return id;
            }
        }
    }
    assert(0);
    return -1;
}

static void mainfunc(u_int32_t low32,u_int32_t hi32)
{
    uintptr_t ptr=uintptr_t(low32)|(uintptr_t(hi32)<<32);
    schedule_t *sch=(schedule_t *)ptr;
    int id=sch->runId;
    coroutine_t *co=sch->coroutines[id];
    co->func(sch,co->arg);
    _co_delete(co);
    sch->coroutines[id]=NULL;
    sch->nco--;
    sch->runId=-1;
}

int coroutine_resume(schedule_t *sch,int id)
{
    assert(sch->runId==-1);
    assert(id>=0&&id<sch->cap);
    coroutine_t *co=sch->coroutines[id];
    if(NULL==co)
    {
        return -1;
    }

    switch (co->state)
    {
        case COROUTINE_READY:
        {
            getcontext(&(co->ctxt));
            co->ctxt.uc_link=&(sch->main);
            /* 设置共享栈 */
            co->ctxt.uc_stack.ss_flags=0;
            co->ctxt.uc_stack.ss_sp=sch->stack;
            co->ctxt.uc_stack.ss_size=DEAFAULT_STACK_SIZE;
            sch->runId=id;
            co->state=COROUTINE_RUNING;
            uintptr_t ptr=uintptr_t(sch);
            makecontext(&(co->ctxt),(void(*)())mainfunc,2,u_int32_t(ptr),u_int32_t(ptr>>32));
            swapcontext(&(sch->main),&(co->ctxt));
            break;
        }
        case COROUTINE_SUSPEND:
        {
            /* 将协程栈保存的内容拷贝到共享栈中 */
            memcpy(sch->stack+DEAFAULT_STACK_SIZE-co->size,co->stack,co->size);
            sch->runId=id;
            co->state=COROUTINE_RUNING;
            swapcontext(&(sch->main),&(co->ctxt));
            break;
        }
        default:
        {
            assert(0);
            return -1;
        }
    }
    return 0;
}

static void _save_stack(coroutine_t *co,char *top)
{
    char dummy=0;
    assert(top-&dummy<DEAFAULT_STACK_SIZE);
    if(co->cap<top-&dummy)
    {
        free(co->stack);
        co->cap=top-&dummy;
        co->stack=(char *)malloc(co->cap);
    }
    co->size=top-&dummy;
    memcpy(co->stack,&dummy,co->size);
}

int coroutine_yield(schedule_t *sch)
{
    assert(sch->runId>=0);
    int id=sch->runId;
    coroutine_t *co=sch->coroutines[id];
    assert((char *)&co>sch->stack);

    /* 保存栈内容 */
    _save_stack(co,sch->stack+DEAFAULT_STACK_SIZE);

    co->state=COROUTINE_SUSPEND;
    sch->runId=-1;
    swapcontext(&(co->ctxt),&(sch->main));
    return 0;
}

CoroutineState coroutine_state(schedule_t *sch,int id)
{
    assert(id>=0&&id<sch->cap);
    if(NULL==sch->coroutines[id])
    {
        return COROUTINE_DEAD;
    }
    return sch->coroutines[id]->state;
}

int coroutine_runId(schedule_t *sch)
{
    return sch->runId;
}