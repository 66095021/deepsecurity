#ifndef _CL3_INFO_HPP_
#define _CL3_INFO_HPP_

#include <stdint.h>



#include "CObject.hpp"
#include "CIpAddress.hpp"
#include "CL2Info.hpp"




class CL3Info: public CObject{
private:
    const CL2Info* m_pL2Info;
    //CL4Info* m_pL4Info;

    const uint8_t * m_L4Data;
    const size_t    m_L4Datalen; 

    const CIpAddress m_src;
    const CIpAddress m_dst;
    uint8_t m_payloadProtocol;
    uint16_t m_id;

public:
    CL3Info(const CL2Info* pL2Info,
            const uint8_t* pL4Data, 
            const size_t l4Len, 
            const CIpAddress src, 
            const CIpAddress dst);
    
    inline const CIpAddress& getSrcIp()const{return m_src;}
    inline const CIpAddress& getDstIp()const{return m_dst;}
    inline const uint8_t* getL4Data()const{return m_L4Data;}
    inline const size_t getL4DataLen()const{return m_L4Datalen;}
    inline const CL2Info* getL2Info()const{return m_pL2Info;}
    inline uint8_t getPayloadProtocol()const{return m_payloadProtocol;}
    inline uint16_t getId()const{return m_id;}
    /*
    inline void setIpversion(uint8_t v){
        m_ipVersion = v;
    }*/

    void printDetailInfo(void)const;
};

#endif
