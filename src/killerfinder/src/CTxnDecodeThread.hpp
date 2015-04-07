#ifndef _CTXN_DECODE_THREAD_HPP_
#define _CTXN_DECODE_THREAD_HPP_

#include <pthread.h>



class CTxnDecodeThread{
private:
    bool m_isRunning;
    pthread_t m_pid;
    CTxnDecodeThread();
    ~CTxnDecodeThread();

public:    
    static CTxnDecodeThread* getInstance();
    static void* threadMain(void *arg);  
    void start();
    void join();
    void stop();

};

#endif
