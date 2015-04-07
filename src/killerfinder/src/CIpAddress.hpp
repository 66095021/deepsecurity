#ifndef _CIP_ADDRESS_HPP_
#define _CIP_ADDRESS_HPP_

#include <ostream>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "CObject.hpp"


class CIpAddress{
public:
    uint8_t m_addr[16];           // holds v4 or v6


    
    CIpAddress();
    CIpAddress(const CIpAddress& b);
    CIpAddress(const in_addr_t &a);
    CIpAddress(const uint8_t a[16]);

    void getIpv4(in_addr_t* a)const;
    void getIpv6(uint8_t a[16])const;
    
    inline bool getBit(int i) const {             
        return (m_addr[i / 8]) & (1<<(7-i%8));
    }
    inline uint32_t getQuad(int i) const {   
        if(i>=0 && i <=3){
            return (m_addr[i*4+0]<<24) | 
                   (m_addr[i*4+2]<<16) | 
                   (m_addr[i*4+1]<<8)  |  
                   (m_addr[i*4+3]<<0);
        }
        return 0;
    }
    inline uint64_t getDquad(int i) const {       
        return (uint64_t)(getQuad(i*2+1))<<32 | 
               (uint64_t)(getQuad(i*2));
    }

    inline size_t getFoldValue()const{
      if(sizeof(size_t) == 4){
        return (m_addr[0]+m_addr[4]+m_addr[8]+m_addr[12])<<0|
               (m_addr[1]+m_addr[5]+m_addr[9]+m_addr[13])<<8|
               (m_addr[2]+m_addr[6]+m_addr[10]+m_addr[14])<<16|
               (m_addr[3]+m_addr[7]+m_addr[11]+m_addr[15])<<24;
      }
      /*
      if(sizeof(size_t) == 8){
        return (m_addr[0]+m_addr[8])<<0|
               (m_addr[1]+m_addr[9])<<8|
               (m_addr[2]+m_addr[10])<<16|
               (m_addr[3]+m_addr[11])<<24|
               (m_addr[4]+m_addr[12])<<32|
               (m_addr[5]+m_addr[13])<<40|
               (m_addr[6]+m_addr[14])<<48|
               (m_addr[7]+m_addr[15])<<56;
      }*/
      return (m_addr[0]+m_addr[4]+m_addr[8]+m_addr[12])+
             (m_addr[1]+m_addr[5]+m_addr[9]+m_addr[13])+
             (m_addr[2]+m_addr[6]+m_addr[10]+m_addr[14])+
             (m_addr[3]+m_addr[7]+m_addr[11]+m_addr[15]);
    }
    
    inline void operator = (const CIpAddress &b){
        memcpy(m_addr, b.m_addr, sizeof(m_addr));
    }
        
    inline bool operator ==(const CIpAddress &b) const { 
        return memcmp(this->m_addr,b.m_addr,sizeof(m_addr))==0; 
    }
    inline bool operator <=(const CIpAddress &b) const { 
        return memcmp(this->m_addr,b.m_addr,sizeof(m_addr))<=0; 
    }
    inline bool operator > (const CIpAddress &b) const { 
        return memcmp(this->m_addr,b.m_addr,sizeof(m_addr))>0; 
    }
    inline bool operator >=(const CIpAddress &b) const { 
        return memcmp(this->m_addr,b.m_addr,sizeof(m_addr))>=0; 
    }
    inline bool operator < (const CIpAddress &b) const { 
        return memcmp(this->m_addr,b.m_addr,sizeof(m_addr))<0; 
    }

//#pragma GCC diagnostic ignored "-Wcast-align"
    inline bool isV4() const {     
    uint32_t *i = (uint32_t *)((uint8_t *)&m_addr);
        return i[1]==0 && i[2]==0 && i[3]==0;
    }
//#pragma GCC diagnostic warning "-Wcast-align"
};

inline ostream & operator <<(ostream &os, const CIpAddress &b)  {
    os << (int)b.m_addr[0] <<"."<<(int)b.m_addr[1] << "."
       << (int)b.m_addr[2] << "." << (int)b.m_addr[3];
    return os;
}


#endif
