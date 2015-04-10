#ifndef _CTCP_SESSION_ID_HPP_
#define _CTCP_SESSION_ID_HPP_

#include <stdint.h>
#include <ostream>
#include <sstream>




#include "CIpAddress.hpp"
#include "CObject.hpp"

class CTcpSessionId
{
private:
    CIpAddress m_srcIp;
    uint16_t m_srcPort;
    CIpAddress m_dstIp;
    uint16_t m_dstPort;

public:
    inline CIpAddress& getSrcIp(){return m_srcIp;}
    inline CIpAddress& getDstIp(){return m_dstIp;}
    inline uint16_t getSrcPort(){return m_srcPort;}
    inline uint16_t getDstPort(){return m_dstPort;}
    
    CTcpSessionId(CIpAddress srcIp, uint16_t srcPort,
                  CIpAddress dstIp, uint16_t dstPort);
    CTcpSessionId(const CTcpSessionId& b);

    size_t getHash()const;

    inline bool operator ==(const CTcpSessionId &b)const{
        if (m_srcIp   == b.m_srcIp &&
            m_srcPort == b.m_srcPort &&
            m_dstIp   == b.m_dstIp &&
            m_dstPort == b.m_dstPort){
            return true;
        }
        if (m_srcIp   == b.m_dstIp &&
            m_srcPort == b.m_dstPort &&
            m_dstIp   == b.m_srcIp &&
            m_dstPort == b.m_srcPort){
            return true;
        }
        return false;
    }

    inline void operator = (const CTcpSessionId &b){
        m_srcIp   = b.m_srcIp;
        m_srcPort = b.m_srcPort;
        m_dstIp   = b.m_dstIp;
        m_dstPort = b.m_dstPort;
    }
    void getIdStr(ostringstream& result);
    void getIdStr(ostringstream& result)const;

};




#endif
