#include "CTcpSessionPool.hpp"
#include "CTcpSessionId.hpp"
#include "CLog.hpp"


CTcpSessionPool::CTcpSessionPool(){
    m_index = 0;
    //m_closedSessList.clear();
    //m_activeSessMap.clear();
    Debug(LOG_LEVEL_DEBUG, "m_closedSessList size "<<m_closedSessList.size());
}


CTcpSessionPool* CTcpSessionPool::getInstance(){
    static CTcpSessionPool* instance = NULL;
    if (instance == NULL){
        instance = new CTcpSessionPool();
    }
    return instance;
}

void CTcpSessionPool::start(){
    Debug(LOG_LEVEL_DEBUG, "CTcpSessionPool start");
}
    

CTcpSession* CTcpSessionPool::createSession(const CTcpSessionId& id){
    CTcpSession* session = new CTcpSession(id);
    if (session == NULL){
        Debug(LOG_LEVEL_ERR, TCP_SESSION_TAG<<"can not new a tcpsession");
        return NULL;
    }
    session->setNum(++m_index);
    m_activeSessMap[id] = session;

    printMe();
    
    return session;
}

CTcpSession* CTcpSessionPool::findActiveSession(const CTcpSessionId& id){
    SessMap::const_iterator it = m_activeSessMap.find(id);
    if (it==m_activeSessMap.end()){
        return NULL; 
    }
    return it->second;
}
void CTcpSessionPool::removeSession(const CTcpSessionId& id){
    SessMap::iterator it = m_activeSessMap.find(id);
    if(it != m_activeSessMap.end()){
        CTcpSession* item = (CTcpSession*)(it->second);
        if (item){
            delete item;
        }
        m_activeSessMap.erase(it);
    }
}

CTcpSession* CTcpSessionPool::popClosedSession(){
    CTcpSession* sess = NULL;
    m_lock.lock();
    if (!m_closedSessList.empty()){
        sess = m_closedSessList.front();
        m_closedSessList.pop_front();
    }
    m_lock.unlock();
    return sess;
}


void CTcpSessionPool::postSession(const CTcpSessionId& id){
    SessMap::iterator it = m_activeSessMap.find(id);
    if(it != m_activeSessMap.end()){
        CTcpSession* item = (CTcpSession*)(it->second);
        m_lock.lock();
        m_closedSessList.push_back(item);
        m_lock.unlock();
        m_activeSessMap.erase(it);
    }
}

void CTcpSessionPool::postTimeoutSessions(time_t now, time_t timeout){
    SessMap::iterator it = m_activeSessMap.begin();
    SessMap::iterator it_tmp;
    int num = 0;
    
    while (it != m_activeSessMap.end()){
        CTcpSession* item = (CTcpSession*)(it->second);
        if (item->getLastUpdateTime() + timeout < now){
            Debug(LOG_LEVEL_DEBUG, "["<<item->getNum()<<"] time out.");
            m_lock.lock();
            m_closedSessList.push_back(item);
            m_lock.unlock();
            it_tmp = it;
            ++it;
            m_activeSessMap.erase(it_tmp);
            if(++num > 10){
                break;
            }
        }else{
            ++it;
        }
    }
    
}

void CTcpSessionPool::printMe(){
    Debug(LOG_LEVEL_DEBUG, " active session "<<m_activeSessMap.size()<<\
                           " closed session "<<m_closedSessList.size());
}



