#include <libnet.h>
#include "CLog.hpp"
#include "CPacketCreater.hpp"



void CPacketCreater::sendIpv4Rst(uint32_t   src_ip, 
                                 uint32_t   dst_ip,
                                 uint16_t   id,
                                 uint16_t   src_prt, 
                                 uint16_t   dst_prt,
                                 uint32_t   seq,
                                 uint32_t   ack){

    libnet_t *l;
    libnet_ptag_t t;
    char*   payload   = NULL;
    u_short payload_s = 0;
    char errbuf[LIBNET_ERRBUF_SIZE];
    int count = 0;

    /*
     *  Initialize the library.  Root priviledges are required.
     */
    l = libnet_init(LIBNET_RAW4,                /* injection type */
                    NULL,                       /* network interface */
                    errbuf);                    /* errbuf */

    if (l == NULL)
    {
        Debug(LOG_LEVEL_ERR,"libnet_init() failed: "<<errbuf);
        return;
    }

    t = libnet_build_tcp(
        src_prt,                                    /* source port */
        dst_prt,                                    /* destination port */
        seq,                                        /* sequence number */
        ack,                                        /* acknowledgement num */
        TH_RST,                                     /* control flags |TH_ACK*/
        32767,                                      /* window size */
        0,                                          /* checksum */
        0,                                          /* urgent pointer */
        LIBNET_TCP_H + payload_s,                   /* TCP packet size */
        (uint8_t*)payload,                          /* payload */
        payload_s,                                  /* payload size */
        l,                                          /* libnet handle */
        0);                                         /* libnet id */
    
    if (t == -1)
    {
        Debug(LOG_LEVEL_ERR, "Can't build TCP header: "<<(libnet_geterror(l)));
        goto bad;
    }

    t = libnet_build_ipv4(
        LIBNET_IPV4_H + LIBNET_TCP_H + payload_s,   /* length */
        0,                                          /* TOS */
        id,                                         /* IP ID */
        0,                                          /* IP Frag */
        64,                                         /* TTL */
        IPPROTO_TCP,                                /* protocol */
        0,                                          /* checksum */
        src_ip,                                     /* source IP */
        dst_ip,                                     /* destination IP */
        NULL,                                       /* payload */
        0,                                          /* payload size */
        l,                                          /* libnet handle */
        0);                                         /* libnet id */
    if (t == -1)
    {
        Debug(LOG_LEVEL_ERR, "Can't build IP header: "<<(libnet_geterror(l)));
        goto bad;
    } 
    
    count = libnet_write(l);
    if (count == -1)
    {
        Debug(LOG_LEVEL_ERR, "Write error: "<<(libnet_geterror(l)));
        goto bad;
    }
        
bad:
    libnet_destroy(l);
    return; 


}


void CPacketCreater::sendIpv6Rst(struct libnet_in6_addr src, 
                                 struct libnet_in6_addr dst,
                                 uint16_t  src_prt, 
                                 uint16_t  dst_prt,
                                 uint32_t  seq,
                                 uint32_t  ack){

}


 



