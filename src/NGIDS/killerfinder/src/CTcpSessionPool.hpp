#ifndef _CTCP_SESSION_POOL_HPP_
#define _CTCP_SESSION_POOL_HPP_

#include <unordered_map>
#include <list>
#include <time.h>

#include "CObject.hpp"
#include "CTcpSession.hpp"
#include "CTcpSessionId.hpp"
#include "CLock.hpp"

typedef struct sessHash_t{
public:
    size_t operator() (const CTcpSessionId &k) const {
        return k.getHash();
        
    }
} SessHash;

typedef struct sessEqual_t{
public:
    bool operator() (const CTcpSessionId &x, const CTcpSessionId &y) const { 
        return x==y;
    }
} SessEqual;


typedef unordered_map<CTcpSessionId,CTcpSession*,SessHash,SessEqual> SessMap; 
//typedef std::tr1::unordered_map<CTcpSessionId,CTcpSession*,SessHash,SessEqual> SessMap; 

//typedef unordered_map<string, CTcpSession*> SessionMap;
typedef list<CTcpSession*> SessList;

class CTcpSessionPool:public CObject
{
private:
    uint64_t m_index;
    SessMap  m_activeSessMap;
    SessList m_closedSessList;
    CLock    m_lock;
private:
    CTcpSessionPool();

public:
    static CTcpSessionPool* getInstance();
    CTcpSession* createSession(const CTcpSessionId& id);
    CTcpSession* findActiveSession(const CTcpSessionId& id);
    CTcpSession* popClosedSession();
    void removeSession(const CTcpSessionId& id);
    void postSession(const CTcpSessionId& id);
    void start();
    void postTimeoutSessions(time_t now, time_t timeout);
    void printMe();
};

#endif


