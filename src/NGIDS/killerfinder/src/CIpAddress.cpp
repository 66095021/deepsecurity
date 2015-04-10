#include <arpa/inet.h>

#include "CIpAddress.hpp"
#include "packet_def.hpp"


CIpAddress::CIpAddress(){
    memset(m_addr, 0, sizeof(m_addr));
}

CIpAddress::CIpAddress(const CIpAddress& b){
    memcpy(m_addr, b.m_addr, sizeof(m_addr));
}


CIpAddress::CIpAddress(const in_addr_t &a){     
    m_addr[0] = ((uint8_t *)&a)[0];   
    m_addr[1] = ((uint8_t *)&a)[1];
    m_addr[2] = ((uint8_t *)&a)[2];
    m_addr[3] = ((uint8_t *)&a)[3];
    memset(m_addr + 4, 0, 12);
}

CIpAddress::CIpAddress(const uint8_t a[16]){
    memcpy(m_addr, a, 16);
}

void CIpAddress::getIpv4(in_addr_t* a)const{ 
    ((uint8_t *)a)[0] = m_addr[0];   
    ((uint8_t *)a)[1] = m_addr[1];
    ((uint8_t *)a)[2] = m_addr[2];
    ((uint8_t *)a)[3] = m_addr[3];
}

void CIpAddress::getIpv6(uint8_t a[16])const{
    memcpy(a, m_addr, 16);
}


