#include "CL4Info.hpp"
#include "CLog.hpp"


CL4Info::CL4Info(const CL3Info* pL3Info,
                 const uint8_t* pL5Data, 
                 const size_t l5Len, 
                 const uint16_t srcPort, 
                 const uint16_t dstPort):
                 m_pL3Info(pL3Info),
                 m_L5Data(pL5Data),
                 m_L5Datalen(l5Len),
                 m_srcPort(srcPort),
                 m_dstPort(dstPort)
{
    struct tcphdr* tcpHeader = (struct tcphdr*)(pL3Info->getL4Data());
    setFlags(tcpHeader);
}

void CL4Info::setFlags(struct tcphdr* tcpHeader){
    
    m_seqNum = ntohl(tcpHeader->th_seq);
    m_ackNum = ntohl(tcpHeader->th_ack);

    m_synSet = FLAG_SET(tcpHeader->th_flags, TH_SYN);
    m_ackSet = FLAG_SET(tcpHeader->th_flags, TH_ACK);
    m_finSet = FLAG_SET(tcpHeader->th_flags, TH_FIN);
    m_rstSet = FLAG_SET(tcpHeader->th_flags, TH_RST);
    m_pushSet= FLAG_SET(tcpHeader->th_flags, TH_PUSH);
    m_urgSet = FLAG_SET(tcpHeader->th_flags, TH_URG);

    m_ackPacket = (!m_L5Datalen)&&(!FLAG_SET(tcpHeader->th_flags, ~TH_ACK));

}

void CL4Info::printDetailInfo()const{
    m_pL3Info->printDetailInfo();
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"src.port="<<m_srcPort<<" dst.port="<<m_dstPort);
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"seq="<<m_seqNum<<" ack="<<m_ackNum);
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"syn "<<m_synSet<<" ack "<<m_ackSet<<" fin "<<m_finSet\
                           <<" rst "<<m_rstSet<<" push "<<m_pushSet<<" urg "<<m_urgSet);  
}



