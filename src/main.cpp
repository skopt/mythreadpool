#include <stdio.h>
#include "ThreadPool.h"
#include <unistd.h>

void funtest(void *argu);
int main()
{
   CThreadPool test;
   int count = 5;
   test.InitPool(5);
   int task = 0;
   while(true)
   {
   	  Task tmp(task,funtest);
      test.AddTask(tmp);
	  printf("add task %d\n", task);
	  task++;
	  sleep(3);
	  if(task == 5)
	  	break;	  
   }
   test.ShutDown();
	
	return 1;
}

void funtest(void *argu)
{
	int val = *((int*)argu);
	printf("funtest val=%d\n", val);	
}
