#include <arpa/inet.h>
#include <pcap.h>
//#include <net/ethernet.h>


#include "CL2Info.hpp"
#include "CLog.hpp"
#include "packet_def.hpp"



CL2Info::CL2Info(const struct pcap_pkthdr *header, 
                 const u_char *pcapData,
                 const uint8_t *ipHeader,
                 size_t ipdataLen,
                 const int type):
                 m_pcapHdr(header),
                 m_pcapData(pcapData),
                 m_L3Data(ipHeader),
                 m_L3Datalen(ipdataLen),
                 m_pcapDlt(type){
    
                         
    //Debug(LOG_LEVEL_DEBUG, "CL2Info In");
}



                       
                         

/*
CL2Info::CL2Info(const int type,
                         const struct pcap_pkthdr *header, 
                         const u_char *pcapData,
                         const uint8_t *ipHeader,
                         size_t ipdataLen ){

    

    u_int caplen = header->caplen;
    u_int length = header->len;
    
    if (length != caplen) {
        Debug(LOG_LEVEL_ERR, 
            "only captured "<<caplen<<" bytes of "<<length<<" byte ether frame");
        return;
    }
    
    struct ether_header *ethHeader = (struct ether_header *)pcap_data;


    const u_short *etherType = &ethHeader->ether_type;
    const u_char  *etherData = pcap_data + sizeof(struct ether_header); 
    if (ntohs(*etherType) == ETHERTYPE_VLAN) {
        etherData += 2;            
        etherData += 4;           
        caplen    -= 4;
        Debug(LOG_LEVEL_DEBUG, "VLAN packet");
    }
  
    if (caplen < sizeof(struct ether_header)) {
        Debug(LOG_LEVEL_ERR, "received incomplete ethernet frame");
        return;
    }

  

    switch (ntohs(*etherType)){
    case ETHERTYPE_IP:
    case ETHERTYPE_IPV6:
    case ETHERTYPE_ARP:
    case ETHERTYPE_LOOPBACK:
    case ETHERTYPE_REVARP:
        break;
    default:
        Debug(LOG_LEVEL_ERR, "received ethernet frame with unexpected type " << ntohs(ethHeader->ether_type));
        break;
    }

}
*/

                   







