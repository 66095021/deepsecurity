#include "Precompiled.h"
#include "TaskManager.h"

Task::Task()
{
}

Task::~Task(void)
{
}

void Task::SetProcessImage(const wstring &processimage)
{
	m_processimage = processimage;
}

void Task::SetArgs(const vector<wstring> &args)
{
	for(int i=0; i<= args.size(); i++)
		m_arguments.push_back(args[i]);
}

 wstring Task::GetProcessImage()
{
	return m_processimage;
}

vector<wstring> Task::GetArgs()
{
	return m_arguments;
}

TaskManager::TaskManager(DWORD procruninterval, DWORD tasksaveinterval)
{
	m_savetaskinterval = tasksaveinterval;
	m_procruninterval = procruninterval;
	m_lastsavetasks = time(NULL);
	::InitializeCriticalSection( & m_taskqueueCS);
}

TaskManager::~TaskManager(void)
{
	::DeleteCriticalSection( & m_taskqueueCS);
}

void TaskManager::Start()
{
	LoadTasks();

	if(!manager_running)
	{
		manager_thread = new Thread(this);
		manager_thread->start("TaskManager");
	}
}

void TaskManager::Stop()
{
	SaveTasks();

	if(manager_running)
	{
		manager_running = false;
		manager_thread->stop();
		delete manager_thread;
	}
}

void TaskManager::Push(Task t)
{
	::EnterCriticalSection( & m_taskqueueCS);
	
	m_deque.push_back(t);
	AutoSaveTasks();
	
	::LeaveCriticalSection( & m_taskqueueCS);
}

bool TaskManager::Pop(Task& t)
{
	::EnterCriticalSection( & m_taskqueueCS);
	
	if(m_deque.size() == 0)
		return false;
	
	t = m_deque.back();
	m_deque.pop_back();

	AutoSaveTasks();
	
	::LeaveCriticalSection( & m_taskqueueCS);

	return true;
}

void TaskManager::LoadTasks()
{
	wstring line;
	wchar_t szFullPath[4096];
	GetFullPathName(L"TaskQueue", 4096, szFullPath, NULL);
	wifstream TaskQfile (szFullPath);
	if (!TaskQfile.is_open())
	{
		printf("TaskManager: Could not open the TaskQueue, no task will be load\n");
		return;
	}
	while (!TaskQfile.eof())
	{
		getline (TaskQfile,line);
		if((line.size() > 0))
		{
			vector<std::wstring> splitLine;	
			typedef split_iterator<wstring::iterator> sf_it;
			for(sf_it it=make_split_iterator(line, token_finder(is_any_of(L"\t")));
				it!=sf_it(); ++it)
			{
				splitLine.push_back(copy_range<std::wstring>(*it));				
			}

			Task task;
			task.SetProcessImage(splitLine[0]);

			vector<wstring> args;
			for (int i=1; i<splitLine.size(); i++)
				args.push_back(splitLine[i]);

			if (args.size() > 0 )
				task.SetArgs(args);

			m_deque.push_back(task);	
		}

	}
}

void TaskManager::SaveTasks()
{
	wstring line;
	wchar_t szFullPath[4096];
	GetFullPathName(L"TaskQueue", 4096, szFullPath, NULL);
	wofstream TaskQfile (szFullPath, ifstream::out | ifstream::trunc);

	if (!TaskQfile.is_open())
	{
		printf("TaskManager: Could not open the TaskQueue, no task will be saved\n");
		return;
	}

	for (int i = 0; i < m_deque.size(); i++)
	{
		Task t = m_deque[i];
		TaskQfile << t.GetProcessImage();

		for(int j = 0; j < t.GetArgs().size(); j++ )
			TaskQfile << "\t" << t.GetArgs()[j];

		TaskQfile << "\n";
	}
}

void TaskManager::AutoSaveTasks()
{
	time_t now = time(NULL);
	if ( (now-m_lastsavetasks) > m_savetaskinterval )
		SaveTasks();
}


void TaskManager::run()
{
	while (manager_running)
	{
		// 1. close overdue process

		for (int i=0; i<m_runnings.size(); i++)
		{
			if( (time(NULL) - m_runnings[i].starttime) > m_procruninterval)
			{
				TerminateProcess(m_runnings[i].hProc, 0);
				m_runnings.pop_front();
			}
			else
				break;
		}


		// 2. open new task
		Task task;
		if (true == Pop(task))
		{
			
			PROCESS_INFORMATION ProcessInfo;
			STARTUPINFO StartupInfo; //This is an [in] parameter
			ZeroMemory(&StartupInfo, sizeof(StartupInfo));
			StartupInfo.cb = sizeof StartupInfo ; //Only compulsory field
			if(CreateProcess(task.GetProcessImage().c_str(), NULL,
				 NULL,NULL,FALSE,0,NULL,
				 NULL,&StartupInfo,&ProcessInfo))
			{
				 CloseHandle(ProcessInfo.hThread);
				 CloseHandle(ProcessInfo.hProcess);
			}  
			else
			{
				std::wcout<< "could not spawn " << task.GetProcessImage().c_str();
				continue;
			}		

			RunItem rt;
			rt.hProc = ProcessInfo.hProcess;
			rt.starttime = time(NULL);
			m_runnings.push_back(rt);
		}

		Sleep(1000);
	}		
}