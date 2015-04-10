#include "Precompiled.h"

#include "ProcessMonitor.h"
#include "EventController.h"
#include "ProcessManager.h"

#define IOCTL_CAPTURE_GET_PROCINFO	CTL_CODE(0x00000022, 0x0802, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_CAPTURE_PROC_LIST    CTL_CODE(0x00000022, 0x0807, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)

typedef struct _ProcessInfo
{
	TIME_FIELDS time;
    DWORD  ParentId;
	DWORD  OriginId;
    DWORD  ProcessId;
	DWORD  ThreadId;
    BOOLEAN bCreate;
	UINT processPathLength;
	WCHAR processPath[1024];
	UINT parentProcessPathLength;
	WCHAR parentProcessPath[1024];
	UINT originProcessPathLength;
	WCHAR originProcessPath[1024];
} ProcessInfo;

typedef struct _PROCESS_TUPLE
{
	DWORD processID;
	WCHAR name[1024];
} PROCESS_TUPLE, * PPROCESS_TUPLE;

ProcessMonitor::ProcessMonitor(void)
{
	wchar_t kernelDriverPath[1024];
	wchar_t exListDriverPath[1024];

	driverInstalled = false;
	monitorRunning = false;
	hEvent = INVALID_HANDLE_VALUE;
	
	// Load exclusion list
	GetFullPathName(L"ProcessMonitor.exl", 1024, exListDriverPath, NULL);
	Monitor::loadExclusionList(exListDriverPath);

	onProcessExclusionReceivedConnection = EventController::getInstance()->connect_onServerEvent(L"process-exclusion", boost::bind(&ProcessMonitor::onProcessExclusionReceived, this, _1));

	// Load process monitor kernel driver
	GetFullPathName(L"CaptureProcessMonitor.sys", 1024, kernelDriverPath, NULL);
	if(Monitor::installKernelDriver(kernelDriverPath, L"CaptureProcessMonitor", L"Capture Process Monitor"))
	{	
		hDriver = CreateFile(
					L"\\\\.\\CaptureProcessMonitor",
					GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					0,                     // Default security
					OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED,  // Perform asynchronous I/O
					0);                    // No template
		if(INVALID_HANDLE_VALUE == hDriver) {
			LOG(INFO, "ProcessMonitor: ERROR - CreateFile Failed: %08x", GetLastError());
		} else {
			initialiseKernelDriverProcessMap();
			driverInstalled = true;
		}
	}
}

ProcessMonitor::~ProcessMonitor(void)
{
	processManagerConnection.disconnect();
	stop();
	if(driverInstalled)
	{
		driverInstalled = false;
		CloseHandle(hDriver);	
	}
	Monitor::unInstallKernelDriver();
}


/////////////////// tool function /////////////
std::wstring GetProcName(std::wstring& processPath)
{
	std::wstring processModuleName = L"UNKNOWN";
	size_t moduleNameStart = processPath.find_last_of(L"\\");
	if(moduleNameStart != std::wstring::npos)
	{
		processModuleName = processPath.substr(moduleNameStart+1);
	}
	return processModuleName;
}

/////////////////// tool function /////////////



boost::signals::connection 
ProcessMonitor::connect_onProcessEvent(const signal_processEvent::slot_type& s)
{ 
	return signalProcessEvent.connect(s); 
}

void
ProcessMonitor::onProcessExclusionReceived(const Element& element)
{
	std::wstring excluded = L"";
	std::wstring parentProcessPath = L"";
	std::wstring processPath = L"";
	std::wstring action = L"";

	std::vector<Attribute>::const_iterator it;
	for(it = element.getAttributes().begin(); it != element.getAttributes().end(); it++)
	{
		if((*it).getName() == L"subject") {
			parentProcessPath = (*it).getValue();
		} else if((*it).getName() == L"object") {
			processPath = (*it).getValue();
		} else if((*it).getName() == L"excluded") {
			excluded = (*it).getValue();
		} else if((*it).getName() == L"action") {
			action = (*it).getValue();
		}
	}


	Monitor::addExclusion(excluded, action, parentProcessPath, processPath);
}

void
ProcessMonitor::initialiseKernelDriverProcessMap()
{
	ProcessManager* processManager = ProcessManager::getInstance();
	stdext::hash_map<DWORD, std::wstring> processMap;

	processMap = processManager->getProcessMap();
	stdext::hash_map<DWORD, std::wstring>::iterator it;
	for(it = processMap.begin(); it != processMap.end(); it++)
	{
		DWORD dwReturn;
		PROCESS_TUPLE pTuple;
		ZeroMemory(&pTuple, sizeof(PROCESS_TUPLE));
		pTuple.processID = it->first;
		memcpy(pTuple.name, it->second.c_str(), it->second.length()*sizeof(wchar_t));
		DeviceIoControl(hDriver,
			IOCTL_CAPTURE_PROC_LIST, 
			&pTuple, 
			sizeof(PROCESS_TUPLE), 
			0, 
			0, 
			&dwReturn, 
			NULL);
	}
}

void
ProcessMonitor::start()
{
	if(!isMonitorRunning() && isDriverInstalled())
	{
		hEvent = OpenEvent(SYNCHRONIZE, FALSE, L"Global\\CaptureProcDrvProcessEvent");
		processMonitorThread = new Thread(this);
		processMonitorThread->start("ProcessMonitor");
	}
}

void
ProcessMonitor::stop()
{
	if(isMonitorRunning() && isDriverInstalled())
	{
		monitorRunning = false;
		CloseHandle(hEvent);

		processMonitorThread->exit();
		delete processMonitorThread;
	}
}

void ProcessMonitor::onProcessEvent(DWORD eventType, DWORD processId,std::wstring& processPath, DWORD parentProcessId,std::wstring& parentPath, std::wstring time) 
{
	std::wstring processModuleName;
	std::wstring parentProcessModuleName;
	
	ProcessManager::getInstance()->onProcessEvent(eventType, time, parentProcessId, 
		processId, processPath);	

	parentPath = ProcessManager::getInstance()->convertFileObjectPathToDOSPath(parentPath);
	processPath = ProcessManager::getInstance()->convertFileObjectPathToDOSPath(processPath);
	parentProcessModuleName = GetProcName(parentPath);
	processModuleName = GetProcName(processPath);

	LOG(INFO, "received process event %i %i:%ls -> %i:%ls", eventType, parentProcessId, parentPath.c_str(), processId, processPath.c_str()); 
	if(!Monitor::isEventAllowed(processModuleName,parentProcessModuleName,processPath))
	{
		signalProcessEvent(eventType, time, parentProcessId, parentPath, processId, processPath);
	}
}

void
ProcessMonitor::run()
{
	BYTE *processEvents = (BYTE *) malloc(PROCESS_EVENTS_BUFFER_SIZE);
	ProcessInfo* tempP = 0;
	monitorRunning = true;
	DWORD      dwBytesReturned = 0;
	while(running && isMonitorRunning())
	{
	/*	DWORD status = WaitForSingleObject(hEvent, 1000);
		if(status == WAIT_TIMEOUT)
			continue;*/

		BOOL       bReturnCode = FALSE;
		ProcessInfo* p;

		bReturnCode = DeviceIoControl(
			hDriver,
			IOCTL_CAPTURE_GET_PROCINFO,
			0, 
			0,
			processEvents, PROCESS_EVENTS_BUFFER_SIZE,
			&dwBytesReturned,
			NULL
			);

		if(dwBytesReturned >= sizeof(ProcessInfo))
		{
			DWORD offset = 0;
			DWORD event_initiate_id = 0;
			std::wstring initpath;
			std::wstring procpath;

			do
			{	
				p = (ProcessInfo*) (processEvents+offset);
				if( !tempP ||
					p->bCreate	!= tempP->bCreate	||
					p->ParentId	!= tempP->ParentId	||
					p->ProcessId!= tempP->ProcessId	||
					p->ThreadId	!= tempP->ThreadId)
				{	
					DWORD type = 0;
					if(p->ThreadId == 0)
					{
						event_initiate_id = p->ParentId;
						initpath = p->parentProcessPath;
						
						if(p->bCreate)
						{
							type = EventProcess_Create;
						}
						else
						{
							type = EventProcess_Terminate;
						}
					}
					else
					{
						event_initiate_id = p->OriginId;
						initpath = p->originProcessPath;
						
						if(p->bCreate)
						{
							if (p->OriginId != p->ProcessId &&
								p->OriginId != p->ParentId)
							{						
								type = EventThread_Inject;
							}
						}
					}

					if(type)
					{
						std::wstring time = Time::timefieldToString(p->time);
						procpath = p->processPath;
						onProcessEvent(type,p->ProcessId, procpath, event_initiate_id, initpath, time);
					}
					tempP = p;
				}

				
				offset += sizeof(ProcessInfo);
			} while(offset < dwBytesReturned);
			
			tempP = 0;
		}

		Sleep(5);
	}
}
