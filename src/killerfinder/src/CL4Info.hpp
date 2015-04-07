#ifndef _CL4_INFO_HPP_
#define _CL4_INFO_HPP_

#include <stdint.h>



#include "CObject.hpp"
#include "CL3Info.hpp"
#include "packet_def.hpp"




class CL4Info: public CObject{
private:
    const CL3Info* m_pL3Info;

    const uint8_t * m_L5Data;
    const size_t    m_L5Datalen; 

    const uint16_t m_srcPort;
    const uint16_t m_dstPort;

    tcp_seq m_seqNum;
    tcp_seq m_ackNum;
    
    bool m_synSet;
    bool m_ackSet;
    bool m_finSet;
    bool m_rstSet;
    bool m_pushSet;
    bool m_urgSet;
    bool m_ackPacket;

public:
    CL4Info(const CL3Info* pL3Info,
            const uint8_t* pL5Data, 
            const size_t l5Len, 
            const uint16_t srcPort, 
            const uint16_t dstPort);
    
    inline const uint16_t getSrcPort()const{return m_srcPort;}
    inline const uint16_t getDstPort()const{return m_dstPort;}
    inline const uint8_t* getL5Data()const{return m_L5Data;}
    inline const size_t getL5DataLen()const{return m_L5Datalen;}
    inline const CL3Info* getL3Info()const{return m_pL3Info;}
    inline bool isSynSet()const{return m_synSet;}
    inline bool isAckSet()const{return m_ackSet;}
    inline bool isFinSet()const{return m_finSet;}
    inline bool isRstSet()const{return m_rstSet;}
    inline bool isPushSet()const{return m_pushSet;}
    inline bool isUrgSet()const{return m_urgSet;}
    inline bool isAckPacket()const{return m_ackPacket;}
    inline tcp_seq getSeqNum()const{return m_seqNum;}
    inline tcp_seq getAckNum()const{return m_ackNum;}
    void setFlags(struct tcphdr* header);

    void printDetailInfo()const;
    
};

#endif

