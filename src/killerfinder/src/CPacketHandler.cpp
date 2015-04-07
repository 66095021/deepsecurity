#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>
//#include <net/ethernet.h>
//#include <ctype.h>
#include <netinet/in.h>




#include "packet_def.hpp"
#include "CPacketHandler.hpp"
#include "CTcpSession.hpp"
#include "CLog.hpp"
#include "CIpAddress.hpp"
#include "CTcpSessionId.hpp"
#include "CTcpSessionPool.hpp"
#include "CTcpSessionPlugin.hpp"
//#include "CPacketCreater.hpp"


int CPacketHandler::DLT = DLT_NULL;

int CPacketHandler::processL5(const CL4Info& layer4Info){

    layer4Info.printDetailInfo();

    time_t now = layer4Info.getL3Info()->getL2Info()->getPacketHeader()->ts.tv_sec;

    CTcpSessionId id(layer4Info.getL3Info()->getSrcIp(), layer4Info.getSrcPort(),
                     layer4Info.getL3Info()->getDstIp(), layer4Info.getDstPort());

    CTcpSession* sess = CTcpSessionPool::getInstance()->findActiveSession(id);
    if (sess == NULL){
        if (layer4Info.isSynSet() && !layer4Info.isAckSet()){
            sess = CTcpSessionPool::getInstance()->createSession(id);
            if (sess == NULL){
                return -1;
            }
            sess->getContinuousBuf(DIRECTION_TO_SERVER).setStartSeq(layer4Info.getSeqNum()+1);
            sess->getContinuousBuf(DIRECTION_TO_SERVER).setUsedPos(0);
            sess->setLastUpdateTime(now);
            Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<sess->getNum()<<"]time "<<sess->getLastUpdateTime());  
            Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<sess->getNum()<<"] create a session");
            CTcpSessionPlugin::sessionCreated(sess);
            sess->blockMe(layer4Info);
            return 0;
        }else{
            Debug(LOG_LEVEL_WARNING, PACKET_TAG<<"first packet is not SYN, drop it");
            return -1;
        }
        
    }
    
    sess->setLastUpdateTime(now);
    Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<sess->getNum()<<"]time "<<sess->getLastUpdateTime());        


    if (layer4Info.isAckPacket()){
      //sess->blockMe(layer4Info);
      
      // I think ACK packet is very frequent.
      // every 10 second, we post the timeout sessions
      if (now % 10 == 0){
        CTcpSessionPool::getInstance()->postTimeoutSessions(now, 60);
      }
      Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"ignore ACK");
      return 0;
    }


    if (sess->isClosed()){
        Debug(LOG_LEVEL_WARNING, PACKET_TAG<<"["<<sess->getNum()<<"] "\
                                 "receive data for closed session, drop it");
        return -1;
    }

    sess->updateMe(layer4Info);
    
    sess->blockMe(layer4Info);
    
    if (sess->isClosed()){
        Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<sess->getNum()<<"] post session");
        CTcpSessionPlugin::sessionPosted(sess);
        CTcpSessionPool::getInstance()->postSession(id);
    }
    return 0;
}

int CPacketHandler::processL4Udp(const CL3Info& layer3Info){
    return -1;
}

int CPacketHandler::processL4Tcp(const CL3Info& layer3Info){
    if (layer3Info.getL4DataLen() < sizeof(struct tcphdr)) {
        Debug(LOG_LEVEL_ERR,PACKET_TAG<<"received truncated TCP segment!"<<\
                 layer3Info.getL4DataLen()<<":"<<sizeof(struct tcphdr));
        return -1;
    }

    struct tcphdr *tcpHeader = (struct tcphdr*)layer3Info.getL4Data();
    uint8_t tcpHeaderLen = tcpHeader->th_off * 4;

    const uint8_t* tcpPayload = layer3Info.getL4Data() + tcpHeaderLen;
    
    size_t tcpDataLen = (layer3Info.getL4DataLen() > tcpHeaderLen)? 
                         layer3Info.getL4DataLen() - tcpHeaderLen : 0;



    uint16_t srcPort = ntohs(tcpHeader->th_sport);
    uint16_t dstPort = ntohs(tcpHeader->th_dport);

    CL4Info layer4Info(&layer3Info, tcpPayload, tcpDataLen, srcPort, dstPort);

    return processL5(layer4Info);

}

int CPacketHandler::processL4(const CL3Info& layer3Info){
    uint8_t protocol = layer3Info.getPayloadProtocol();
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"layer 4 protocol is "<<((uint32_t)protocol));
    switch(protocol){
    case IPPROTO_TCP:
        return processL4Tcp(layer3Info);
        break;
    case IPPROTO_UDP:
        return processL4Udp(layer3Info);
        break;
    default:
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"no handler now... protocol"<<((uint32_t)protocol));
    }
    return -1;

}

int CPacketHandler::processL3Ip4(const CL2Info& layer2Info){

    if (layer2Info.getL3Len() < sizeof(struct ip4_hdr)){
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received truncated IP datagram!");
        return -1;
    }

    const struct ip4_hdr *ipHeader = (struct ip4_hdr *)layer2Info.getL3Header();

    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<\
        " caplen="<<(layer2Info.getPacketHeader()->caplen)<<\
        " vlan="<<layer2Info.getVlan());
        
    /* check and see if we got everything.  NOTE: we must use
     * ip_total_len after this, because we may have captured bytes
     * beyond the end of the packet (e.g. ethernet padding).
     */
    size_t ipLen = ntohs(ipHeader->ip_len);
    if (layer2Info.getL3Len() < ipLen) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<\
            "captured "<<layer2Info.getL3Len()<<" bytes of "<<ipLen<<" byte IP");
        return -1;
    }

    /* throw away everything but fragment 0; this version doesn't
     * know how to do fragment reassembly.
     */
    if (ntohs(ipHeader->ip_off) & IP_OFFMASK) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"throwing away IP fragment");
        return -1;
    }

    
    size_t ipHeaderLen = ipHeader->ip_hl * 4;
    if (ipHeaderLen > ipLen) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received truncated IP datagram!");
        return -1;
    }
    
    /* do TCP processing, faking an ipv6 address  */
    size_t ipPayloadLen = ipLen - ipHeaderLen;
    const uint8_t* ipPayloadData = layer2Info.getL3Header() + ipHeaderLen;

    CIpAddress srcIp(ipHeader->ip_src.addr);
    CIpAddress dstIp(ipHeader->ip_dst.addr);

    CL3Info layer3Info(&layer2Info,ipPayloadData, ipPayloadLen, srcIp, dstIp);
    return processL4(layer3Info);
}

int CPacketHandler::processL3Ip6(const CL2Info& layer2Info){

    if (layer2Info.getL3Len() < sizeof(struct ip6_hdr)){
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received truncated IP datagram!");
        return -1;
    }

    const struct ip6_hdr *ipHeader = (struct ip6_hdr*)layer2Info.getL3Header();

    const uint8_t* ipPayloadData = layer2Info.getL3Header() + sizeof(struct ip6_hdr);
    uint16_t ipPayloadLen  = ntohs(ipHeader->ip6_ctlun.ip6_un1.ip6_un1_plen);
    CIpAddress srcIp(ipHeader->ip6_src.addr.addr8);
    CIpAddress dstIp(ipHeader->ip6_dst.addr.addr8);
        
    CL3Info layer3Info(&layer2Info,ipPayloadData, ipPayloadLen, srcIp, dstIp);

    return processL4(layer3Info);

}


void CPacketHandler::processL3(const CL2Info& layer2Info){
    int result = 1; 
    int version = layer2Info.getIpVersion();
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"layer3 ip version is " << version);
    switch(version){
    case 4:
        result = processL3Ip4(layer2Info);
        break;
    case 6:
        result = processL3Ip6(layer2Info);
        break;
    default:
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"no handler now... ip version " << version);
        break;
    }
    if(result != 0){                          
        
    }    
}


void CPacketHandler::processL2En10Mb(u_char     *args, 
                       const struct pcap_pkthdr *header,
                       const u_char             *packet){

    uint32_t caplen = header->caplen;
    uint32_t length = header->len;
    if (length != caplen) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<\
            "captured "<<caplen<<" bytes of "<<length<<" byte ether frame");
        return;
    }
    
    struct ether_header *ethHeader = (struct ether_header *)packet;
    const uint16_t *etherType = &ethHeader->ether_type;
    const uint8_t  *etherData = packet + sizeof(struct ether_header); 
    if (ntohs(*etherType) == ETHERTYPE_VLAN) {
        etherType += 2; // note: etherType is pointer to uint16_t. +2 == mv 4 byte          
        etherData += 4;           
        caplen    -= 4;
        Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"VLAN packet");
    }
    
    if (caplen < sizeof(struct ether_header)) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received incomplete ethernet frame");
        return;
    }
 
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"the protocol of layer 2 payload is "<<(ntohs(*etherType)));

    uint8_t* ipHeader = (uint8_t*)etherData;
    size_t ipLen = caplen - sizeof(struct ether_header);
    CL2Info layer2Info(header, packet, ipHeader, ipLen, DLT_EN10MB);
    processL3(layer2Info);
    
}

void CPacketHandler::processL2LinuxSll(u_char   *args, 
                       const struct pcap_pkthdr *header,
                       const u_char             *packet){

    uint32_t caplen = header->caplen;
    uint32_t length = header->len;
    if (length != caplen) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<\
            "captured "<<caplen<<" bytes of "<<length<<" byte ether frame");
        return;
    }
    
    if (caplen < SLL_HDR_LEN) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received incomplete Linux cooked frame");
        return;
    }
  
    sll_header *sllp = (sll_header*)packet;
    uint32_t mpls_sz = 0;
    if (ntohs(sllp->sll_protocol) == ETHERTYPE_MPLS) {
        // unwind MPLS stack
        do {
            if(caplen < SLL_HDR_LEN + mpls_sz + 4){
                Debug(LOG_LEVEL_ERR, PACKET_TAG<<"MPLS stack overrun");
                return;
            }
            mpls_sz += 4;
            caplen -= 4;
        } while ((packet[SLL_HDR_LEN + mpls_sz - 2] & 1) == 0 );
    }
    
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"the protocol of layer 2 payload is "<<(ntohs(sllp->sll_protocol)));

    const uint8_t* ipHeader = packet + SLL_HDR_LEN + mpls_sz;
    size_t ipLen = caplen - SLL_HDR_LEN;
    CL2Info layer2Info(header, packet, ipHeader, ipLen, DLT_LINUX_SLL);
    processL3(layer2Info);
}

void CPacketHandler::processL2Raw(u_char   *args, 
                       const struct pcap_pkthdr *header,
                       const u_char             *packet){

    /* DLT_RAW: just a raw IP packet, no encapsulation or link-layer
     * headers.  Used for PPP connections under some OSs including Linux
     * and IRIX. */
    if (header->caplen != header->len) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"only captured "<<header->caplen<<\
                             " bytes of "<<header->len<<" byte raw frame");
    }

    CL2Info layer2Info(header, packet, packet, header->caplen, DLT_RAW);
    processL3(layer2Info);

}


void CPacketHandler::processL2Null(u_char   *args, 
                   const struct pcap_pkthdr *header,
                   const u_char             *packet){
    /* The DLT_NULL packet header is 4 bytes long. It contains a network
     * order 32 bit integer that specifies the family, e.g. AF_INET.
     * DLT_NULL is used by the localhost interface.
     */

    uint32_t caplen = header->caplen;
    uint32_t length = header->len;
    uint32_t family = *(uint32_t *)packet;

    if (length != caplen) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<\
            "captured "<<caplen<<" bytes of "<<length<<" byte ether frame");
        return;
    }

    if (caplen < NULL_HDRLEN) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received incomplete null frame");
        return;
    }

    if (family != AF_INET && family != AF_INET6) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received null frame with unknown type "<<family);
        return;
    }
    
    CL2Info layer2Info(header, packet, packet+NULL_HDRLEN, caplen-NULL_HDRLEN, DLT_NULL);
    processL3(layer2Info);
}

void CPacketHandler::processL2Ppp(u_char   *args, 
                  const struct pcap_pkthdr *header,
                  const u_char             *packet){

    uint32_t caplen = header->caplen;
    uint32_t length = header->len;

    if (length != caplen) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<\
            "captured "<<caplen<<" bytes of "<<length<<" byte ether frame");
        return;
    }

    if (caplen < PPP_HDRLEN) {
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"received incomplete null frame");
        return;
    }
    
    CL2Info layer2Info(header, packet, packet+PPP_HDRLEN, caplen-PPP_HDRLEN, DLT_PPP);
    processL3(layer2Info);
}



void CPacketHandler::processL2(u_char   *args, 
               const struct pcap_pkthdr *header,
               const u_char             *packet){
               
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"layer 2 link type (DLT): "<<CPacketHandler::DLT);
    switch (CPacketHandler::DLT){
    case DLT_EN10MB:
    case DLT_IEEE802:
        processL2En10Mb(args, header, packet);
        break;
    case DLT_LINUX_SLL:
        processL2LinuxSll(args, header, packet);
        break;
    //Some systems define DLT_RAW as 12, some as 14, and some as 101.
    case 101:
    case 12:
    case 14:
        processL2Raw(args, header, packet);
        break;
    case DLT_NULL:
        processL2Null(args, header, packet);
        break;
    case DLT_PPP:
        processL2Ppp(args, header, packet);
        break;
    case DLT_IEEE802_11:
    case DLT_IEEE802_11_RADIO:
    case DLT_PRISM_HEADER:
    default:
        Debug(LOG_LEVEL_ERR, PACKET_TAG<<"no handler now...");
        break;

    }
}


void CPacketHandler::packetCallBack(u_char* args, 
                  const struct pcap_pkthdr* header,
                              const u_char* packet)
{
    static int count = 1;
    Debug(LOG_LEVEL_DEBUG, "+++++++++++++++++++++++++");
    Debug(LOG_LEVEL_DEBUG, PACKET_TAG<<"Packet number " << count);
    count++;
    processL2(args, header, packet);
    return;    
}

void CPacketHandler::payloadPrint(const u_char* payLoad, int sizePayload)
{
  
  for (int i = 1; i <= sizePayload; i++){
      printf("%X ", *(payLoad+i-1));
      if ( i%16 == 0){
         printf("\n");
      }
  }
  printf("\n");

}


