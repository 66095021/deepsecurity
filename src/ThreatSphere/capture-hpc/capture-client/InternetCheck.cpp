#include "Precompiled.h"
#include "InternetCheck.h"

DWORD InternetCheckPing(IPAddr ip)
{
	HANDLE hIcmp;
	DWORD num_responses = 0;
	char data[] = "ping";
    BYTE buf[sizeof(ICMP_ECHO_REPLY) + 32];
    DWORD buf_len = 0;

	hIcmp = IcmpCreateFile();
    if (hIcmp == INVALID_HANDLE_VALUE)
        return 0;

	num_responses = IcmpSendEcho2(hIcmp, NULL, NULL, NULL, ip, data, sizeof(data), NULL, buf, sizeof(buf), 1000);
	
	return num_responses;
}