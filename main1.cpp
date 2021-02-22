#include"coroutine1.h"
#include<stdio.h>

void func(schedule_t *sch,void *arg)
{
    int *num=(int *)arg;
    for(int i=0;i<5;++i)
    {
        printf("coroutine %d:%d\n",coroutine_runId(sch),*num+i);
        coroutine_yield(sch);
    }
}

void coroutine_test(schedule_t *sch)
{
    int num1=1,num2=100;
    int id1=coroutine_create(sch,func,&num1);
    int id2=coroutine_create(sch,func,&num2);
    printf("main start\n");
    while(coroutine_state(sch,id1)!=COROUTINE_DEAD&&coroutine_state(sch,id2)!=COROUTINE_DEAD)
    {
        coroutine_resume(sch,id1);
        coroutine_resume(sch,id2);
    }
    printf("main end\n");
}

int main()
{
    schedule_t *sch=schedule_open();
    coroutine_test(sch);
    schedule_close(sch);
}
