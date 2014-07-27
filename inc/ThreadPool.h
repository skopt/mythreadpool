#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <pthread.h>
using namespace std;  
#include<list>  
//---------------define---------------------------
typedef void (*TaskProcess)(void *pArgu);

//----------------struct--------------------------
class Task
{
public:
	Task(){}
	Task(int value, TaskProcess fun)
		:val(value),ProcessFun(fun)
		{}
	Task(const Task &tmp)
	{
		val = tmp.val;
		ProcessFun = tmp.ProcessFun;
	}
public:
	int val;
	TaskProcess ProcessFun;
};
typedef void (*TaskProcess)(void *pArgu);
class CThreadPool
{
public:
	CThreadPool();
	~CThreadPool();
private:
	pthread_mutex_t TaskListLock;  
    pthread_cond_t TaskListReady; 
	int RuningFlag;
	list<Task> TaskList;
	pthread_t *ThreadCreated;        //all the thread created
	int mThreadCount;                //thread count
public:

private:
	static void* _ThreadRoutine(void *pArgu);
public:
	bool InitPool(int threadCount);
	bool AddTask(Task addTask);
	Task GetTask();
	bool ShutDown();
};

#endif