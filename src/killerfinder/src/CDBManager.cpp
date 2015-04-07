#include <string.h>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CLog.hpp"
#include "CDBManager.hpp"
#include "CConfig.hpp"

void split(const string& str, const char* d, vector<string>& result){
   size_t len = str.size();
   char* buf = (char*)malloc(len+1);
   char* b = buf;
   memcpy(b, str.c_str(), len);
   b[len] = 0;
   char*tmp;
   while((tmp = strsep(&buf, d))!=0){
     if(tmp[0] != 0){
       string s;
       s.append(tmp);
       result.push_back(s);
     }
   }
   free(b);
   return;
}

void printV(vector<string>& v){
    if (v.size()==0){
        cout<<"empty";
    }
    for(size_t i=0; i<v.size(); i++){
      cout<<v[i]<<"; ";
    }
    cout<<endl;
}






CDBManager::CDBManager(){

}




void CDBManager::printDB(){
    for(unsigned int i = 0; i<sizeof(m_httpMap)/sizeof(m_httpMap[0]);i++){
        Debug(LOG_LEVEL_INFO, "+++++++++++++++");
        DbMap::iterator it;
        for(it=m_httpMap[i].begin();it!=m_httpMap[i].end();++it){
            Debug(LOG_LEVEL_INFO, it->first<<"\t"<<it->second);
        }

    }
    
}

void CDBManager::start(){
    for(unsigned int i = 0; i<sizeof(m_httpMap)/sizeof(m_httpMap[0]);i++){
        string fileName = CConfig::getInstance("")->m_rootPath;
        if(i==I_IPV4_MAP){
            fileName.append("/db/2_ipv4.txt");
        }else if (i==I_DOMAIN_MAP){
            fileName.append("/db/2_domain.txt");
        }else if (i==I_HOSTNAME_MAP){
            fileName.append("/db/2_hostname.txt");
        }

        int num = 0;
        
        ifstream ifs;
        string line;
        ifs.open(fileName, ifstream::in);
        while (getline(ifs, line)) {
            if (line.size()==0||line[0]==' '||line[0]=='\t'){
                continue;
            }
            vector<string> result;
            split(line, "\t", result);
            if (result.size()<2){
                Debug(LOG_LEVEL_ERR, "db error:" << line);
            }
            m_httpMap[i][result[0]] = result[1];
            if((CConfig::getInstance("")->m_maxDb) &&
               (++num > CConfig::getInstance("")->m_maxDb)){
                break;
            }
        }
        ifs.close();
    } 
    printDB();

    DbMap::iterator it;
    struct in_addr addr;
    for(it=m_httpMap[I_IPV4_MAP].begin();
        it!=m_httpMap[I_IPV4_MAP].end();
        ++it){
        inet_aton(it->first.c_str(), &addr);
        m_ip4Map[addr.s_addr] = it->second;
    }
    
    
}


CDBManager* CDBManager::getInstance(){
    static CDBManager* instance = NULL;
    if (instance==NULL){
        instance = new CDBManager();
    }
    return instance;
}

bool CDBManager::isUrlHit(string& prot, string& host, string& path){
    bool hit = false;
    string host_ = host;
    string path_ = path;
    Debug(LOG_LEVEL_DEBUG, "[URL]"<<prot<<"://"<<host<<path);

    if (prot=="http"||prot=="https"||prot=="ftp"){
        size_t pos = host.find(":");
        if ( pos != string::npos ){
            host_ = host.substr(0, pos);
        }
        if ( path[0] != '/' ){
            path_ = string("/").append(path);   
        }
        DbMap::iterator it;
        
        // 1: try to find the URL in DB
        // www.baidu.com/index.html?x=1&y=2
        it = m_httpMap[I_HOSTNAME_MAP].find(host_+path_);
        hit = (it != m_httpMap[I_HOSTNAME_MAP].end());
        if (hit){
            Debug(LOG_LEVEL_DEBUG, it->first<<"\t"<<it->second);
            if (it->second == "1"){
                return true;
            }else if (it->second == "0"){
                return false;
            }else{
                Debug(LOG_LEVEL_ERR, "wrong db");
            }
        }
        
        // 2: try to find the HostName in DB
        // www.baidu.com
        it = m_httpMap[I_HOSTNAME_MAP].find(host_);
        hit = (it != m_httpMap[I_HOSTNAME_MAP].end());
        if (hit){
            Debug(LOG_LEVEL_DEBUG, it->first<<"\t"<<it->second);
            if (it->second == "1"){
                return true;
            }else if (it->second == "0"){
                return false;
            }else{
                Debug(LOG_LEVEL_ERR, "wrong db");
            }
        }
        
        // 3: try to find the Domain in DB
        // baidu.com
        vector<string> result;
        split(host_, ".", result);
        if(result.size() >= 2){
            string domain;
            domain.append(result[result.size()-2]);
            domain.append(".");
            domain.append(result[result.size()-1]);
            it = m_httpMap[I_DOMAIN_MAP].find(domain);
            hit = (it != m_httpMap[I_DOMAIN_MAP].end());
            if (hit){
                Debug(LOG_LEVEL_DEBUG, it->first<<"\t"<<it->second);
                if (it->second == "1"){
                    return true;
                }else if (it->second == "0"){
                    return false;
                }else{
                    Debug(LOG_LEVEL_ERR, "wrong db");
                }
            }  
        }
    } 
    return hit;
}

bool CDBManager::isIpv4Hit(uint32_t ip4){
    if (m_ip4Map.find(ip4) != m_ip4Map.end()){
        Debug(LOG_LEVEL_DEBUG, "ip4 hit:"<<ip4);
        return true;
    }else{
        return false;
    }
}


