#ifndef _CLOCK_HPP_
#define _CLOCK_HPP_

#include <unistd.h>  
#include "pthread.h"  


class CLock{
private:
    pthread_mutex_t m_mutex;
public:
    CLock();
    ~CLock();
    void lock(){ pthread_mutex_lock(&m_mutex); }
    void unlock(){pthread_mutex_unlock(&m_mutex); }
};

#endif

