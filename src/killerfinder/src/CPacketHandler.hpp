#ifndef _PACKECT_HANDLER_HPP_
#define _PACKECT_HANDLER_HPP_

//#include <ctype.h>
#include <pcap.h>


#include "CObject.hpp"
#include "CL2Info.hpp"
#include "CL3Info.hpp"
#include "CL4Info.hpp"



class CPacketHandler 
{

public:
  static int DLT;

  static void packetCallBack(u_char *args, 
           const struct pcap_pkthdr *header,
           const u_char             *packet);
  static void processL2(u_char   *args, 
        const struct pcap_pkthdr *header,
        const u_char             *packet);
  static void processL2En10Mb( u_char *args, 
             const struct pcap_pkthdr *header,
                         const u_char *packet);
  static void processL2LinuxSll(u_char*args, 
             const struct pcap_pkthdr *header,
                         const u_char *packet);
  static void processL2Raw(u_char   *args, 
           const struct pcap_pkthdr *header,
           const u_char             *packet);
  static void processL2Null(u_char   *args, 
            const struct pcap_pkthdr *header,
            const u_char             *packet);
  static void processL2Ppp(u_char   *args, 
           const struct pcap_pkthdr *header,
           const u_char             *packet);

  static void processL3(const CL2Info& packetInfo);  
  static int  processL3Ip4(const CL2Info& packetInfo);
  static int  processL3Ip6(const CL2Info& packetInfo);
  static int  processL4(const CL3Info& layer3Info);
  static int  processL4Tcp(const CL3Info& layer3Info);
  static int  processL4Udp(const CL3Info& layer3Info);
  static int  processL5(const CL4Info& layer4Info);

  static void payloadPrint(const u_char* payLoad, int sizePayload);

  
};



#endif

