#ifndef _CPACKET_INFO_HPP_
#define _CPACKET_INFO_HPP_

//#include <ctype.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <stdint.h>

#include "CObject.hpp"
#include "packet_def.hpp"



class CL2Info : public CObject
{
private:
    const struct pcap_pkthdr *m_pcapHdr; // provided by libpcap
    const u_char *m_pcapData;            // provided by libpcap; where the MAC layer begins
    const uint8_t * m_L3Data;            // pointer to where ip data begins
    const size_t m_L3Datalen;            // length of ip data
    const int    m_pcapDlt;              // data link type; needed by libpcap, not provided
    //const struct timeval &m_timstamp;    // when packet received; possibly modified before packet_info created


    
public:
    const struct pcap_pkthdr* getPacketHeader()const{return m_pcapHdr;}
    const u_char*  getPacketData()const{return m_pcapData;}
    inline const uint8_t* getL3Header()const{return m_L3Data;}
    const size_t   getL3Len()const{return m_L3Datalen;}
    const int getDataLinkType()const{return m_pcapDlt;}
public:
    // IPv4 header offsets
    static const size_t ip4_proto_off = 9;
    static const size_t ip4_src_off = 12;
    static const size_t ip4_dst_off = 16;
    // IPv6 header offsets
    static const size_t ip6_nxt_hdr_off = 6;
    static const size_t ip6_plen_off = 4;
    static const size_t ip6_src_off = 8;
    static const size_t ip6_dst_off = 24;
    // TCP header offsets
    static const size_t tcp_sport_off = 0;
    static const size_t tcp_dport_off = 2;
    

    enum vlan_t {
        NO_VLAN = -1
    };

    CL2Info( const struct pcap_pkthdr *header, 
             const u_char *pcapData,
             const uint8_t *ipHeader,
             size_t ipdataLen,
             const int type);
    
    static u_short nshort(const u_char *buf, size_t pos);
    int getIpVersion() const;         
    int getVlan() const;
    inline u_short getEtherType() const;


    
    /** create a packet, usually an IP packet.
     * @param d - start of MAC packet
     * @param d2 - start of IP data
     */
    /*
    CL2Info(const int dlt,const struct pcap_pkthdr *h,const u_char *d,
                const struct timeval &ts_,const uint8_t *d2,size_t dl2):
        pcap_dlt(dlt),pcap_hdr(h),pcap_data(d),ts(ts_),ip_data(d2),ip_datalen(dl2){}
    CL2Info(const int dlt,const struct pcap_pkthdr *h,const u_char *d):
        pcap_dlt(dlt),pcap_hdr(h),pcap_data(d),ts(h->ts),ip_data(d),ip_datalen(h->caplen){}
    */

/*
    static u_short nshort(const u_char *buf,size_t pos);   // return a network byte order short at offset pos
    int     ip_version() const;         // returns 4, 6 or 0
    u_short ether_type() const;         // returns 0 if not IEEE802, otherwise returns ether_type
    int     vlan() const;               // returns NO_VLAN if not IEEE802 or not VLAN, othererwise VID
    const uint8_t *get_ether_dhost() const;   // returns a pointer to ether dhost if ether packet
    const uint8_t *get_ether_shost() const;   // returns a pointer to ether shost if ether packet

    // packet typing
    bool    is_ip4() const;
    bool    is_ip6() const;
    bool    is_ip4_tcp() const;
    bool    is_ip6_tcp() const;
    // packet extraction
    // IPv4 - return pointers to fields or throws frame_too_short exception
    const struct in_addr *get_ip4_src() const;
    const struct in_addr *get_ip4_dst() const;
    uint8_t get_ip4_proto() const;
    // IPv6
    uint8_t  get_ip6_nxt_hdr() const;
    uint16_t get_ip6_plen() const;
    const struct ip6_addr *get_ip6_src() const;
    const struct ip6_addr *get_ip6_dst() const;
    // TCP
    uint16_t get_ip4_tcp_sport() const;
    uint16_t get_ip4_tcp_dport() const;
    uint16_t get_ip6_tcp_sport() const;
    uint16_t get_ip6_tcp_dport() const;
*/
}; 

inline int CL2Info::getIpVersion() const
{
    if (m_L3Datalen >= sizeof(struct ip4_hdr)) {
        const struct ip4_hdr *ipHeader = (struct ip4_hdr *)m_L3Data;
        switch(ipHeader->ip_v){/*it both works for ip4 and ip6*/
        case 4: return 4;
        case 6: return 6;
        default: return ipHeader->ip_v;
        }
    }
    return 0;
}
inline u_short CL2Info::nshort(const u_char *buf, size_t pos) 
{
    return (buf[pos]<<8) | (buf[pos+1]);
}

inline int CL2Info::getVlan() const
{
    if(getEtherType()==ETHERTYPE_VLAN){
        return nshort(m_pcapData, sizeof(struct ether_header));
    }
    return -1;
}

inline u_short CL2Info::getEtherType() const
{
    if(m_pcapDlt==DLT_IEEE802 || m_pcapDlt==DLT_EN10MB){
        const struct ether_header *ethHeader = (struct ether_header *) m_pcapData;
        return ntohs(ethHeader->ether_type);
    }
    return 0;
}




#endif

