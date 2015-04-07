#ifndef _CPACKET_CREATER_HPP_
#define _CPACKET_CREATER_HPP_

class CPacketCreater{


public:
  static void sendIpv4Rst( uint32_t   src_ip, 
                           uint32_t   dst_ip,
                           uint16_t   id,
                           uint16_t   src_prt, 
                           uint16_t   dst_prt,
                           uint32_t   seq,
                           uint32_t   ack);

static void sendIpv6Rst( struct libnet_in6_addr src, 
                         struct libnet_in6_addr dst,
                         uint16_t  src_prt, 
                         uint16_t  dst_prt,
                         uint32_t  seq,
                         uint32_t  ack);


};


#endif
