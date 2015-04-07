
#include <sys/statfs.h>   /* or <sys/vfs.h>*/
#include <time.h>
#include <unistd.h>    //access
#include <sys/stat.h>  //mkdir
#include <sys/types.h> //mkdir
#include <errno.h>



#include "CConfig.hpp"
#include "CLog.hpp"


bool isFileSystemOk(const string& path)
{
    if(!makeDir(path)){
        return false;
    }
    
    unsigned long limit = 200*1024*1024;
    struct statfs fs;
    if (statfs(path.c_str(), &fs) < 0){
        Debug(LOG_LEVEL_ERR, "statfs err. errno " << errno);
        return false;
    }
    unsigned long spaceLeft = fs.f_bavail;// * fs.f_bsize;
    if (spaceLeft < (limit/fs.f_bsize)){
        Debug(LOG_LEVEL_WARNING, "spaceLeft "<<spaceLeft);
        return false;
    }
    return true;
}


bool makeDir(const string& path){
    if (access(path.c_str(), F_OK) < 0){
        if (errno != ENOENT){
            Debug(LOG_LEVEL_ERR, "access error. errno "<<errno);
        }
        if(mkdir(path.c_str(), R_OK|W_OK)<0){
            Debug(LOG_LEVEL_ERR, "mkdir error. errno "<<errno);
            return false;
        }   
    }
    return true;
}


CLog::CLog(){
    string path = CConfig::getInstance("")->m_rootPath;
    path.append("/logs");
    if (access(path.c_str(), F_OK) < 0){
        if (errno != ENOENT){
            cout<<"access errno "<<errno<<endl;
            return;
        }
        if(mkdir(path.c_str(), R_OK|W_OK)<0){
            cout<<"mkdir errno "<<errno<<" path "<<path<<endl;
            return;
        }   
    }
    m_file.open(path.append("/killer-finder.log").c_str());
    m_logLevel = CConfig::getInstance("")->m_logLevel;
}
CLog::~CLog(){
    m_file.close();
}


void CLog::start(){

}

void CLog::debug(int level, string& content){
    if (m_logLevel >= level) {    
       m_lock.lock();            
       time_t __t__ = time(NULL);
       char __buf__[26];         
       ctime_r(&__t__, __buf__); 
       __buf__[24] = 0;          
       m_file<<"["<<__buf__<<"] "<<content<<endl;
       m_lock.unlock();
    }
}

CLog* CLog::getInstance(){
    static CLog* instance = NULL;
    if (instance == NULL){
        instance = new CLog();
    }
    return instance;
}


