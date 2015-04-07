#ifndef _CPACKET_CAPTURE_THREAD_HPP_
#define _CPACKET_CAPTURE_THREAD_HPP_

#include <pthread.h>


class CPacketCaptureThread{
private:
    bool        m_isRunning;
    pthread_t   m_pid;
    
    CPacketCaptureThread();

public:
    static CPacketCaptureThread* getInstance();
    static void* threadMain(void *arg);  
    void start();
    void join();
    void stop();
};

#endif
