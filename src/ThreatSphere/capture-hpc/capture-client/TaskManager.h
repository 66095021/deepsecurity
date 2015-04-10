#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp> 
#include <boost/tokenizer.hpp>
#include "Thread.h"

using namespace std;
using namespace boost;

class Task
{
public:
	Task();
	~Task(void);
	
	wstring GetProcessImage();
	vector<wstring> GetArgs();

	void SetProcessImage(const wstring &processimage);
	void SetArgs(const vector<wstring> &args);
	
private:
	wstring m_processimage;
	vector<wstring> m_arguments;
};


class TaskManager: public Runnable
{
public:

	typedef struct _runitem
	{
		HANDLE	hProc;
		time_t	starttime;
	}RunItem;
	
	TaskManager(DWORD procruninterval = 10, DWORD tasksaveinterval = 60);
	~TaskManager(void);
	
	void Start();
	void Stop();
	void run();

	void Push(Task t);
	bool Pop(Task& t);

protected:
	void LoadTasks();
	void SaveTasks();
	void AutoSaveTasks();

private:
	deque<Task>			m_deque;
	deque<RunItem>		m_runnings;

	CRITICAL_SECTION	m_taskqueueCS;

	time_t				m_lastsavetasks;
	DWORD				m_savetaskinterval; //unit second
	DWORD				m_procruninterval; //unit second

	Thread* manager_thread;
	bool manager_running;
};
