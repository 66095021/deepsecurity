#include "Precompiled.h"

#include "Monitor.h"

#include "Permission.h"
#include <vector>
#include <hash_map>
#include <fstream>
#include <boost\algorithm\string.hpp>
#include <boost\range\iterator_range.hpp>

Monitor::Monitor()
{
	hService = NULL;
}

Monitor::~Monitor()
{
	stdext::hash_map<std::wstring, std::list<Permission*>*>::iterator it;
	for(it = permissionMap.begin(); it != permissionMap.end(); it++)
	{
		std::list<Permission*>::iterator lit;
		for(lit = it->second->begin(); lit != it->second->end(); lit++)
		{
			delete (*lit);
		}
		it->second->clear();
	}
	permissionMap.clear();
//	unInstallKernelDriver();
}

bool
Monitor::isEventAllowed(const std::wstring& eventType,  const std::wstring& subject, const std::wstring& object)
{
	std::wstring event_type = eventType; // Copy
	stdext::hash_map<std::wstring, std::list<Permission*>*>::iterator it;
	std::transform(event_type.begin(),event_type.end(),event_type.begin(),std::towlower);
	it = permissionMap.find(event_type);
	PERMISSION_CLASSIFICATION excluded = NO_MATCH;
	if(it != permissionMap.end())
	{
		std::list<Permission*>* lp = it->second;
		std::list<Permission*>::iterator lit;

		for(lit = lp->begin(); lit != lp->end(); lit++)
		{
			PERMISSION_CLASSIFICATION newExcluded = (*lit)->Check(subject,object);
			if( newExcluded == ALLOWED)
			{
				if(excluded != DISALLOWED)
					excluded = ALLOWED;
			} else if(newExcluded == DISALLOWED) {
				excluded = DISALLOWED;
			}
		}
	}
	if(excluded == ALLOWED)
	{
		return true;
	} else {
		return false;
	}
}

void
Monitor::clearExclusionList()
{
	stdext::hash_map<std::wstring, std::list<Permission*>*>::iterator it;
	for(it = permissionMap.begin(); it != permissionMap.end(); it++)
	{
		std::list<Permission*>* lp = it->second;
		std::list<Permission*>::iterator lit;
		for(lit = lp->begin(); lit != lp->end(); lit++)
		{
			Permission* p = *lit;
			if(!p->permaneant)
			{
				lp->remove(p);
				delete (p);
			}
		}
	}
}

bool
Monitor::installKernelDriver(const std::wstring& driverPath, const std::wstring& driverName, const std::wstring& driverDescription)
{
	SC_HANDLE hSCManager;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

    if(hSCManager)
    {
		//LOG(INFO, "%ls: Kernel driver path: %ls\n", driverName.c_str(), driverPath.c_str());
        hService = CreateService(hSCManager, driverName.c_str(), 
								  driverDescription.c_str(), 
                                  SERVICE_START | DELETE | SERVICE_STOP, 
                                  SERVICE_KERNEL_DRIVER,
                                  SERVICE_DEMAND_START, 
                                  SERVICE_ERROR_IGNORE, 
								  driverPath.c_str(), 
                                  NULL, NULL, NULL, NULL, NULL);

        if(!hService)
        {
			hService = OpenService(hSCManager, driverName.c_str(), 
                       SERVICE_START | DELETE | SERVICE_STOP);
        }

        if(hService)
        {
            if(StartService(hService, 0, NULL))
			{
				LOG(INFO, "Loaded kernel driver: %ls", driverName.c_str());
			} else {
				DWORD err = GetLastError();
				if(err == ERROR_SERVICE_ALREADY_RUNNING)
				{
					LOG(INFO, "Driver already loaded: %ls", driverName.c_str());
				} else {
					LOG(INFO, "Error loading kernel driver: %ls - 0x%08x", driverName.c_str(), err);
					CloseServiceHandle(hSCManager);
					return false;
				}
			}
		} else {
			LOG(INFO, "Error loading kernel driver: %ls - 0x%08x", driverName.c_str(), GetLastError());
			CloseServiceHandle(hSCManager);
			return false;
		}
        CloseServiceHandle(hSCManager);
		return true;
    }
	LOG(INFO, "Error loading kernel driver: %ls - OpenSCManager 0x%08x", driverName.c_str(), GetLastError());
	return false;
}

void
Monitor::unInstallKernelDriver()
{
	if(hService != NULL)
	{
		SERVICE_STATUS ss;
		ControlService(hService, SERVICE_CONTROL_STOP, &ss);
		DeleteService(hService);
		CloseServiceHandle(hService);
	}
	hService = NULL;
}

void
Monitor::loadExclusionList(const std::wstring& file)
{
	std::string line;
	int lineNumber = 0;
	LOG(INFO, "loading exclusion list - %ls", file.c_str());
	std::ifstream exclusionList (file.c_str());
	if (exclusionList.is_open())
	{
		while (! exclusionList.eof() )
		{
			getline (exclusionList,line);
			lineNumber++;
			if(line.length() > 0 && line.at(0) != '#') {
				
				try {
					if(line.at(0) == '+' || line.at(0) == '-')
					{
						std::vector<std::wstring> splitLine;

						typedef boost::algorithm::split_iterator<std::string::iterator> sf_it;
						for(sf_it it = boost::algorithm::make_split_iterator(line, boost::algorithm::token_finder(boost::is_any_of("\t")));
							it!=sf_it(); ++it)
						{
							splitLine.push_back(boost::copy_range<std::wstring>(*it));				
						}

						if(splitLine.size() == 4)
						{
							if(splitLine[1] == L".*" || splitLine[1] == L".+")
							{
								LOG(INFO, "%ls ERROR on line %i: The action type is not supposed to be a regular expression", file.c_str(), lineNumber);
							} else {
								addExclusion(splitLine[0], splitLine[1], splitLine[2], splitLine[3]);
							}
						} else {
							LOG(INFO, "%ls token ERROR on line %i", file.c_str(), lineNumber);
						}
					} else {
						LOG(INFO, "%ls ERROR no exclusion type (+,-) on line %i", file.c_str(), lineNumber);
					}
				} catch(boost::regex_error r) {				
					LOG(INFO, "%ls ERROR on line %i", file.c_str(), lineNumber);
					LOG(INFO, "\t%s", r.what());
				}
			}
		}
	} else {
		LOG(INFO, "Could not open file: %ls", file.c_str());
	}
}

void
Monitor::prepareStringForExclusion(std::wstring& s)
{
	std::wstring from = L"\\";
	std::wstring to = L"\\\\";
	size_t offset = 0;
	while((offset = s.find(from, offset)) != std::wstring::npos)
	{
		s.replace(offset, 
			from.size(), 
			to);
		offset += to.length();
	}
	from = L".";
	to = L"\\.";
	offset = 0;
	while((offset = s.find(from, offset)) != std::wstring::npos)
	{
		s.replace(offset, 
			from.size(), 
			to);
		offset += to.length();
	}
}

void
Monitor::addExclusion(const std::wstring& excluded, const std::wstring& action, const std::wstring& subject, const std::wstring& object , bool permaneant)
{
	//LOG(INFO, "Adding exclusion\n");
	std::wstring action_copy = action; // Action copy
	try {
		Permission* p = new Permission();
		if(excluded == L"yes" || excluded == L"+")
		{
			p->allow = true;
		} else if(excluded == L"no" || excluded == L"-"){
			p->allow = false;
		}
		p->permaneant = permaneant;
		boost::wregex subjectRegex(subject.c_str(), boost::wregex::icase);
		boost::wregex objectRegex(object.c_str(), boost::wregex::icase);
		p->objects.push_back(objectRegex);
		p->subjects.push_back(subjectRegex);
		std::transform(action_copy.begin(),action_copy.end(),action_copy.begin(),std::towlower);
		stdext::hash_map<std::wstring, std::list<Permission*>*>::iterator it;
		it = permissionMap.find(action_copy );

		if(it == permissionMap.end())
		{
			std::list<Permission*>* l = new std::list<Permission*>();
			l->push_back(p);
			permissionMap.insert(std::pair<std::wstring, std::list<Permission*>*>(action_copy, l));
		} else {
			std::list<Permission*>* lp = it->second;
			lp->push_back(p);
		}
	} catch(boost::regex_error r) {
		throw r;
	}
}
