#include"coroutine.h"
#include<stdio.h>

void func1(void *arg)
{
    schedule_t *ps=static_cast<schedule_t *>(arg);
    printf("########\n");
    coroutine_yield(*ps);
    printf("########\n");
}

void func2(void *arg)
{
    schedule_t *ps=static_cast<schedule_t *>(arg);
    printf("*********\n");
    coroutine_yield(*ps);
    printf("*********\n");
}

int main()
{
    schedule_t schedule;
    int id1=coroutine_create(schedule,func1,&schedule);
    int id2=coroutine_create(schedule,func2,&schedule);
    while(is_schedule_finished(schedule)!=1)
    {
        coroutine_resume(schedule,id1);
        coroutine_resume(schedule,id2);
    }
    return 0;
}
