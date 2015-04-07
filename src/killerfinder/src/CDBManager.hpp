#ifndef _CDB_MANAGER_HPP
#define _CDB_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <map>

using namespace std;


void split(const string& str, const char* d, vector<string>& result);

typedef unordered_map<string, string> DbMap;

#define I_IPV4_MAP     0
#define I_DOMAIN_MAP   1
#define I_HOSTNAME_MAP 2

class CDBManager{
private:
    CDBManager();
    DbMap   m_httpMap[3];
    unordered_map<uint32_t, string> m_ip4Map;

public:
    static CDBManager* getInstance();
    void start();
    void printDB();
    bool isUrlHit(string& prot, string& host, string& path);
    bool isIpv4Hit(uint32_t ipv4);
};



#endif
