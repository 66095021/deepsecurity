#ifndef _CLOG_HPP_
#define _CLOG_HPP_

#include <iostream>
#include <time.h>  
#include <stdio.h>
#include <fstream>
#include <sstream>     //ostringstream
#include "CLock.hpp"



using namespace std;

#define PACKET_TAG "[packet]"
#define TCP_SESSION_TAG "[tcpsession]"
#define HTTP_TAG "[http]"
#define X_THREAD_TAG "[x_thread]"

#define LOG_LEVEL_INFO    7
#define LOG_LEVEL_DEBUG   5
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERR     1
/*
#define Debug(level, content)     \
    if (g_logLevel >= level) {    \
        g_lock.lock();            \
        time_t __t__ = time(NULL);\
        char __buf__[26];         \
        ctime_r(&__t__, __buf__); \
        __buf__[24] = 0;          \
        cout<<"["<<__buf__<<"] "<<content<<endl;\
        g_lock.unlock();\
    }
*/

bool isFileSystemOk(const string& path);
bool makeDir(const string& path);


class CLog 
{
private:
  CLog();
  ~CLog();
  ofstream m_file;
  CLock    m_lock;
  int      m_logLevel;
  
public:
  static CLog* getInstance();
  void start();
  void debug(int level, string& content);
  inline int getLogLevel(){return m_logLevel;}
  inline CLock& getLock(){return m_lock;}
  inline ofstream& getFileStream(){return m_file;}
};


#define Debug(level, content)     \
    if (CLog::getInstance()->getLogLevel() >= level){\
        CLog::getInstance()->getLock().lock();\
        time_t __t__ = time(NULL);\
        char __buf__[26];\
        ctime_r(&__t__, __buf__);\
        __buf__[24] = 0;\
        CLog::getInstance()->getFileStream()<<"["<<__buf__<<"] "<<content<<endl;\
        CLog::getInstance()->getLock().unlock();\
    }

#endif

