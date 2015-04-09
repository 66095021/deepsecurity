#pragma once
#include "Thread.h"

#define HTTP_PARSER_STRICT 0
#include "http_parser.h"


#include "InternetCheck.h"
//#include "OpenFile.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wininet.lib")

#define SERVER_PORT		58080
#define SERVER_BUFLEN	1024*8
#define SERVER_BUFSTEP	1024*1024*8		//   8 MB
#define MAX_FILE_SIZE	1024*1024*100	// 100 MB
#define MAX_URI_SIZE	1024

#define WEBSVC_HTTP_PARTIAL		0
#define WEBSVC_HTTP_SUCCESS		1

#define URI_FILE		"/file/"
#define URI_RUN			"/run/"
#define URI_OPEN		"/open/"
#define URI_EXPLORER	"/explorer/"
#define URI_CLEANTEMP	"/cleantemp/"
#define URI_START		"/start/"
#define URI_STOP		"/stop/"
#define URI_SCREENSHOT	"/screenshot/"
#define URI_EVENTS		"/events/"
#define URI_SIGCHECK	"/sigcheck/"
#define URI_PING		"/ping/"


#define URI_DATA_POLLUTION	"/data_pollution/"

#define FILE_DATA_POLLUTION "C:\\data_pollution.log"

DWORD WINAPI startWebService(void* lp);
DWORD WINAPI stopWebService();

typedef boost::signal<void (const std::string&, const std::string&)> signal_CMDEvent;

void onCMDEvent(const std::string& application, const std::string& parameter);
boost::signals::connection connect_onCMDEvent(const signal_CMDEvent::slot_type& s);

class WebCmdServer: public Runnable
{
public:
	WebCmdServer(void);
	virtual ~WebCmdServer(void);
	
	void start();
	void stop();

	void run();
	
private:
	static DWORD __stdcall  sockethandler(void* lp);

private:
	Thread* receiver;
	bool running;
};