
#include "CTcpSessionId.hpp"

CTcpSessionId::CTcpSessionId(CIpAddress srcIp, uint16_t srcPort,
                             CIpAddress dstIp, uint16_t dstPort):
                             m_srcIp(srcIp),
                             m_srcPort(srcPort),
                             m_dstIp(dstIp),
                             m_dstPort(dstPort)
{


}

CTcpSessionId::CTcpSessionId(const CTcpSessionId& b){
    m_srcIp   = b.m_srcIp;
    m_srcPort = b.m_srcPort;
    m_dstIp   = b.m_dstIp;
    m_dstPort = b.m_dstPort;
}

size_t CTcpSessionId::getHash()const{
    size_t ret = m_srcIp.getFoldValue()+m_dstIp.getFoldValue();
    return ret ^ (m_srcPort+m_dstPort);
}

void CTcpSessionId::getIdStr(ostringstream& oss){

    int loop = 4;
    
    if (m_srcIp.isV4() && m_dstIp.isV4()){
        loop = 4;
    }else{
        loop = 16;
    }
    
    for (int i = 0; i < loop; i++){
       oss << (unsigned)m_srcIp.m_addr[i];
       oss << ".";
    }
    oss << m_srcPort;
    
    oss << "-";
    
    for (int i = 0; i < loop; i++){
       oss <<(unsigned) m_dstIp.m_addr[i];
       oss << ".";
    }
    oss << m_dstPort;
    return;
    
}

void CTcpSessionId::getIdStr(ostringstream& oss)const{
    int loop = 4;
    
    if (m_srcIp.isV4() && m_dstIp.isV4()){
        loop = 4;
    }else{
        loop = 16;
    }
    
    for (int i = 0; i < loop; i++){
       oss << (unsigned)m_srcIp.m_addr[i];
       oss << ".";
    }
    oss << m_srcPort;
    
    oss << "-";
    
    for (int i = 0; i < loop; i++){
       oss <<(unsigned) m_dstIp.m_addr[i];
       oss << ".";
    }
    oss << m_dstPort;
    return;
}


