#ifndef _PACKET_DEF_HPP_
#define _PACKET_DEF_HPP_

//#include <ctype.h>
#include <netinet/in.h>
#include <stdint.h>

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __BYTE_ORDER __LITTLE_ENDIAN // I assume compile on little-endian

/* ethernet headers are always exactly 14 bytes [1] */
#ifndef SIZE_ETHERNET
#define SIZE_ETHERNET 14
#endif

#ifndef ETH_ALEN
#define ETH_ALEN 6                   
#endif

#ifndef NULL_HDRLEN
#define NULL_HDRLEN 4
#endif

#ifndef PPP_HDRLEN
#define PPP_HDRLEN 4
#endif



// protocol ID
#ifndef IPPROTO_IP
#define IPPROTO_IP      0       /* dummy for IP */
#endif
#ifndef IPPROTO_HOPOPTS
#define IPPROTO_HOPOPTS     0       /* IPv6 hop-by-hop options */
#endif
#ifndef IPPROTO_ICMP
#define IPPROTO_ICMP        1       /* control message protocol */
#endif
#ifndef IPPROTO_IGMP
#define IPPROTO_IGMP        2       /* group mgmt protocol */
#endif
#ifndef IPPROTO_IPV4
#define IPPROTO_IPV4        4
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP     6       /* tcp */
#endif
#ifndef IPPROTO_EGP
#define IPPROTO_EGP     8       /* exterior gateway protocol */
#endif
#ifndef IPPROTO_PIGP
#define IPPROTO_PIGP        9
#endif
#ifndef IPPROTO_UDP
#define IPPROTO_UDP     17      /* user datagram protocol */
#endif
#ifndef IPPROTO_DCCP
#define IPPROTO_DCCP        33      /* datagram congestion control protocol */
#endif
#ifndef IPPROTO_IPV6
#define IPPROTO_IPV6        41
#endif
#ifndef IPPROTO_ROUTING
#define IPPROTO_ROUTING     43      /* IPv6 routing header */
#endif
#ifndef IPPROTO_FRAGMENT
#define IPPROTO_FRAGMENT    44      /* IPv6 fragmentation header */
#endif
#ifndef IPPROTO_RSVP
#define IPPROTO_RSVP        46      /* resource reservation */
#endif
#ifndef IPPROTO_GRE
#define IPPROTO_GRE     47      /* General Routing Encap. */
#endif
#ifndef IPPROTO_ESP
#define IPPROTO_ESP     50      /* SIPP Encap Sec. Payload */
#endif
#ifndef IPPROTO_AH
#define IPPROTO_AH      51      /* SIPP Auth Header */
#endif
#ifndef IPPROTO_MOBILE
#define IPPROTO_MOBILE      55
#endif
#ifndef IPPROTO_ICMPV6
#define IPPROTO_ICMPV6      58      /* ICMPv6 */
#endif


// ether types
#ifndef ETHERTYPE_PUP
#define ETHERTYPE_PUP           0x0200          /* Xerox PUP */
#endif

#ifndef ETHERTYPE_SPRITE
#define ETHERTYPE_SPRITE        0x0500          /* Sprite */
#endif

#ifndef ETHERTYPE_IP
#define ETHERTYPE_IP            0x0800          /* IP */
#endif

#ifndef ETHERTYPE_ARP
#define ETHERTYPE_ARP           0x0806          /* Address resolution */
#endif

#ifndef ETHERTYPE_REVARP
#define ETHERTYPE_REVARP        0x8035          /* Reverse ARP */
#endif

#ifndef ETHERTYPE_AT
#define ETHERTYPE_AT            0x809B          /* AppleTalk protocol */
#endif

#ifndef ETHERTYPE_AARP
#define ETHERTYPE_AARP          0x80F3          /* AppleTalk ARP */
#endif

#ifndef ETHERTYPE_VLAN
#define ETHERTYPE_VLAN          0x8100          /* IEEE 802.1Q VLAN tagging */
#endif

#ifndef ETHERTYPE_IPX
#define ETHERTYPE_IPX           0x8137          /* IPX */
#endif

#ifndef ETHERTYPE_IPV6
#define ETHERTYPE_IPV6          0x86dd          /* IP protocol version 6 */
#endif

#ifndef ETHERTYPE_LOOPBACK
#define ETHERTYPE_LOOPBACK      0x9000          /* used to test interfaces */
#endif

#ifndef ETHERTYPE_IPX
#define ETHERTYPE_IPX           0x8137
#endif

#ifndef ETHERTYPE_IPV6
#define ETHERTYPE_IPV6          0x86dd
#endif

#ifndef ETHERTYPE_PPP
#define ETHERTYPE_PPP           0x880b
#endif

#ifndef ETHERTYPE_SLOW
#define ETHERTYPE_SLOW          0x8809
#endif

#ifndef ETHERTYPE_MPLS
#define ETHERTYPE_MPLS          0x8847
#endif

#ifndef ETHERTYPE_MPLS_MULTI
#define ETHERTYPE_MPLS_MULTI    0x8848
#endif

#ifndef ETHERTYPE_PPPOED
#define ETHERTYPE_PPPOED        0x8863
#endif






#ifndef DLT_NULL
#define DLT_NULL        0       /* BSD loopback encapsulation */
#endif

#ifndef DLT_EN10MB
#define DLT_EN10MB      1       /* Ethernet (10Mb) */
#endif

#ifndef DLT_EN3MB
#define DLT_EN3MB       2       /* Experimental Ethernet (3Mb) */
#endif

#ifndef DLT_AX25
#define DLT_AX25        3       /* Amateur Radio AX.25 */
#endif

#ifndef DLT_PRONET
#define DLT_PRONET      4       /* Proteon ProNET Token Ring */
#endif

#ifndef DLT_CHAOS
#define DLT_CHAOS       5       /* Chaos */
#endif

#ifndef DLT_IEEE802
#define DLT_IEEE802     6       /* 802.5 Token Ring */
#endif

#ifndef DLT_ARCNET
#define DLT_ARCNET      7       /* ARCNET, with BSD-style header */
#endif

#ifndef DLT_SLIP
#define DLT_SLIP        8       /* Serial Line IP */
#endif

#ifndef DLT_PPP
#define DLT_PPP         9       /* Point-to-point Protocol */
#endif

#ifndef DLT_FDDI
#define DLT_FDDI        10      /* FDDI */
#endif

#ifndef DLT_RAW
#define DLT_RAW         101     /* just packets */
#endif


#ifndef DLT_C_HDLC
#define DLT_C_HDLC      104 /* Cisco HDLC */
#endif

#ifndef DLT_IEEE802_11
#define DLT_IEEE802_11  105 /* IEEE 802.11 wireless */
#endif

#ifndef DLT_FRELAY
#define DLT_FRELAY      107
#endif

#ifndef DLT_LOOP
#define DLT_LOOP        108
#endif

#ifndef DLT_LINUX_SLL
#define DLT_LINUX_SLL   113
#endif

#ifndef DLT_LTALK
#define DLT_LTALK       114
#endif

#ifndef DLT_ECONET
#define DLT_ECONET      115
#endif

#ifndef DLT_IPFILTER
#define DLT_IPFILTER    116
#endif


#ifndef DLT_PRISM_HEADER
#define DLT_PRISM_HEADER	119
#endif

#ifndef DLT_AIRONET_HEADER
#define DLT_AIRONET_HEADER	120
#endif

#ifndef DLT_IEEE802_11_RADIO
#define DLT_IEEE802_11_RADIO	127
#endif

#ifndef DLT_IEEE802_11_RADIO_AVS
#define DLT_IEEE802_11_RADIO_AVS 163
#endif






#ifndef SLL_ADDRLEN
#define SLL_ADDRLEN 8
#endif

#ifndef SLL_HDR_LEN
#define SLL_HDR_LEN 16
#endif

struct sll_header {
    uint16_t   sll_pkttype;    /* packet type */
    uint16_t   sll_hatype;     /* link-layer address type */
    uint16_t   sll_halen;      /* link-layer address length */
    uint8_t    sll_addr[SLL_ADDRLEN];  /* link-layer address */
    uint16_t   sll_protocol;   /* protocol */
}__attribute__ ((__packed__));







struct ether_addr {
    uint8_t ether_addr_octet[ETH_ALEN];
} __attribute__ ((__packed__));


struct ether_header {
    uint8_t  ether_dhost[ETH_ALEN]; /* destination eth addr */
    uint8_t  ether_shost[ETH_ALEN]; /* source ether addr    */
    uint16_t ether_type;            /* packet type ID field */
} __attribute__ ((__packed__));



typedef uint32_t ip4_addr_t;         
struct ip4_addr {   
    ip4_addr_t addr;
};


struct ip4_hdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t ip_hl:4;                /* header length */
    uint8_t ip_v:4;                 /* version */
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    uint8_t ip_v:4;                 /* version */
    uint8_t ip_hl:4;                /* header length */
#endif
    uint8_t  ip_tos;                /* type of service */
    uint16_t ip_len;                /* total length */
    uint16_t ip_id;                 /* identification */
    uint16_t ip_off;                /* fragment offset field */
#define IP_RF 0x8000                    /* reserved fragment flag */
#define IP_DF 0x4000                    /* dont fragment flag */
#define IP_MF 0x2000                    /* more fragments flag */
#define IP_OFFMASK 0x1fff               /* mask for fragmenting bits */
    uint8_t ip_ttl;                 /* time to live */
    uint8_t ip_p;                   /* protocol */
    uint16_t ip_sum;                        /* checksum */
    struct ip4_addr ip_src, ip_dst; /* source and dest address */
} __attribute__ ((__packed__));

struct ip4_dgram {
    const struct ip4_hdr *header;
    const uint8_t *payload;
    uint16_t payload_len;
};



struct ip6_addr {           // our own private ipv6 definition
    union {
        uint8_t   addr8[16];        // three ways to get the data
        uint16_t  addr16[8];
        uint32_t  addr32[4];
    } addr;                    /* 128-bit IP6 address */
};

struct ip6_hdr {
    union {
        struct ip6_hdrctl {
            uint32_t ip6_un1_flow;  /* 20 bits of flow-ID */
            uint16_t ip6_un1_plen;  /* payload length */
            uint8_t  ip6_un1_nxt;   /* next header */
            uint8_t  ip6_un1_hlim;  /* hop limit */
        } ip6_un1;
        uint8_t ip6_un2_vfc;        /* 4 bits version, top 4 bits class */
    } ip6_ctlun;
    struct ip6_addr ip6_src;        /* source address */
    struct ip6_addr ip6_dst;        /* destination address */
} __attribute__((__packed__));

struct ip6_dgram {
    const struct ip6_hdr *header;
    const uint8_t *payload;
    uint16_t payload_len;
};

#ifndef FLAG_SET
#define FLAG_SET(vector, flag) ((vector) & (flag))
#endif

#define TH_FIN        0x01
#define TH_SYN        0x02
#define TH_RST        0x04
#define TH_PUSH       0x08
#define TH_ACK        0x10
#define TH_URG        0x20

typedef  uint32_t tcp_seq;
struct tcphdr {
    uint16_t th_sport;              /* source port */
    uint16_t th_dport;              /* destination port */
    tcp_seq th_seq;         /* sequence number */
    tcp_seq th_ack;         /* acknowledgement number */
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t th_x2:4;                /* (unused) */
    uint8_t th_off:4;               /* data offset */
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    uint8_t th_off:4;               /* data offset */
    uint8_t th_x2:4;                /* (unused) */
#endif
    uint8_t  th_flags;
    uint16_t th_win;            /* window */
    uint16_t th_sum;            /* checksum */
    uint16_t th_urp;            /* urgent pointer */
};



#endif

