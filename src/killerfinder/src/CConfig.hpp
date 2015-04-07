#ifndef _CCONFIG_HPP_
#define _CCONFIG_HPP_

#include <string>
#include <set>
#include <stdint.h>


#include "CObject.hpp"




class CConfig
{
private:
    CConfig(const string& configFileName);
    ~CConfig();
    void setDefault();
    void loadFromFile(const string & configFileName);

public:
    static CConfig* getInstance(const string& configFileName);
    string m_mode;
    string m_offlineFile;
    string m_dev;
    string m_filter;
    int m_packetNum;
    int m_poolLen;
    set<uint16_t> m_httpPorts;
    set<uint16_t> m_smtpPorts;
    set<int> m_caredHttpMethods;
    set<string> m_caredContentTypes;
    int m_pcapTimeout;
    int m_maxDb;
    int m_logLevel;
    string m_rootPath;

    void printMe();
};



#endif

