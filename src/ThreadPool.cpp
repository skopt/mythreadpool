#include "ThreadPool.h"
#include <stdio.h>

#define LogE printf
#define LogD printf
#define LogI printf
CThreadPool::CThreadPool()
{
	RuningFlag = true;
	TaskList.clear();
}
CThreadPool::~ CThreadPool()
{
}
bool CThreadPool::InitPool(int threadCount)
{
	pthread_mutex_init(&TaskListLock, NULL);
	pthread_cond_init(&TaskListReady, NULL);

	mThreadCount = threadCount;
	ThreadCreated = new pthread_t[mThreadCount];
	for(int i = 0; i < mThreadCount; i++)
	{
		if(0 != pthread_create(&ThreadCreated[i], NULL, _ThreadRoutine, (void *)this))
		{
			LogE("creat thread failed, i=%d\n",i);
			RuningFlag = false;//stop the thread running
			return false;
		}		
	}
	LogI("creat %d threads\n",mThreadCount);

	return true;
}
void* CThreadPool::_ThreadRoutine(void *pArgu)
{
	CThreadPool *pthis = (CThreadPool *)pArgu;
	Task workTask;
	//get task
	while(true)
	{
		workTask = pthis->GetTask();
		if(false == pthis->RuningFlag)
		{
			LogD("Thread %d, RuningFlag is false, exit\n", pthread_self());
			return NULL;
		}
		LogI("task val is %d\n", workTask.val);
		if(NULL == workTask.ProcessFun)
		{
			LogE("Process fun is null,continue\n");
			continue;
		}

		workTask.ProcessFun((void *)&workTask.val);
	}
    pthread_exit (NULL);//for exit complete
}
bool CThreadPool::AddTask(Task addTask)
{
	pthread_mutex_lock(&TaskListLock);
	TaskList.push_back(addTask);
	pthread_mutex_unlock(&TaskListLock);
	pthread_cond_signal(&TaskListReady);
	return true;
}
Task CThreadPool::GetTask()
{
	Task ret;
	pthread_mutex_lock(&TaskListLock);
	while(TaskList.empty() && RuningFlag)
	{
		LogI("Thread %d is waiting\n", pthread_self());
		pthread_cond_wait(&TaskListReady, &TaskListLock);
		LogI("Thread %d weakup\n", pthread_self());
	}
	LogD("Thread %d to work\n", pthread_self());

	if(!RuningFlag)//exit
	{
		pthread_mutex_unlock(&TaskListLock);
		return ret;
	}

	ret = TaskList.front();
	TaskList.pop_front();
	pthread_mutex_unlock(&TaskListLock);
	return ret;
}
bool CThreadPool::ShutDown()
{
	//set flag to be false
	while(RuningFlag)
	{
		RuningFlag = false;
	}
	//send broadcast
	pthread_cond_broadcast(&TaskListReady);
	LogI("send exit broadcast\n");
	for(int i = 0; i < mThreadCount; i++)
	{
		pthread_join(ThreadCreated[i], NULL);
	}
	LogI("all the thread exit\n");
	return true;
}