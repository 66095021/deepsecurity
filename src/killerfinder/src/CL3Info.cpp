

#include "CL3Info.hpp"
#include "CLog.hpp"




CL3Info::CL3Info(const CL2Info* pL2Info,
                 const uint8_t* pL4Data, 
                 const size_t l4Len, 
                 const CIpAddress src, 
                 const CIpAddress dst):
                 m_pL2Info(pL2Info),
                 m_L4Data(pL4Data), 
                 m_L4Datalen(l4Len),
                 m_src(src),
                 m_dst(dst){
    struct ip4_hdr* ip4Header;
    struct ip6_hdr* ip6Header;
                 
    switch (pL2Info->getIpVersion()){
    case 4:   
        ip4Header = (struct ip4_hdr*)(pL2Info->getL3Header());
        m_payloadProtocol = ip4Header->ip_p;
        m_id = ip4Header->ip_id;
        break;
    case 6:
        ip6Header =(struct ip6_hdr*)(pL2Info->getL3Header());
        m_payloadProtocol = ip6Header->ip6_ctlun.ip6_un1.ip6_un1_nxt;
        break;
    default:
        m_payloadProtocol = 0;
        break;
    }
   
}



void CL3Info::printDetailInfo()const{
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"src="<<m_src<<" dst="<<m_dst);
}
                 