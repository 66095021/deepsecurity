#ifndef _CTCP_SESSION_HPP_
#define _CTCP_SESSION_HPP_


#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
//#include <ctype.h>
#include <memory>
#include <map>
#include <list>


#include "packet_def.hpp"
#include "CObject.hpp"
#include "CL4Info.hpp"
#include "CTcpSessionId.hpp"
#include "CHttpInfo.hpp"
#include "CL4Info.hpp"


class CHttpTxnDecoder;
class CSmtpTxnDecoder;


class CTcpSegBuf {
private:   
    tcp_seq m_start;
    tcp_seq m_end;
    tcp_seq m_used;
    string  m_buf;
    
public:    
    CTcpSegBuf();
    void insertData(const tcp_seq seq, const uint8_t* buf, size_t size);
    void setStartSeq(tcp_seq seq);
    void dumpUnusedData(string& out);
    string& getBuf();
    void setUsedPos(tcp_seq used);
};





#define TCP_SM_INIT 0
#define TCP_SM_HANDSHAKE 1
#define TCP_SM_CLIENT_SENDING 2
#define TCP_SM_SERVER_SENDING 3
#define TCP_SM_CLIENT_FINISHED 4
#define TCP_SM_SERVER_FINISHED 5
#define TCP_SM_CLOSED 6

#define DIRECTION_UNKOWN -1
#define DIRECTION_TO_SERVER 0
#define DIRECTION_TO_CLIENT 1

#define PROTOCOL_UNKNOWN 0
#define PROTOCOL_HTTP    1
#define PROTOCOL_SMTP    2


class CTcpSession : public CObject
{
private:
  CTcpSessionId    m_sessId;
  uint64_t         m_tcpTxnId;
  int              m_tcpState;
  //TcpSegmentMap    m_segMap[2];
  bool             m_activeFlg;
  int              m_protocol;
  CHttpTxnDecoder* m_httpDecoder;
  CSmtpTxnDecoder* m_smtpDecoder;
  time_t           m_lastUpdateTime;
  bool             m_shouldBlock;
  CTcpSegBuf       m_continuousSeg[2];

public:
  CTcpSession(const CTcpSessionId& sessId);
  ~CTcpSession();

  CTcpSegBuf& getContinuousBuf(int dir);

  inline int getDirection(const CL4Info& layer4Info){
    if(m_sessId.getSrcIp()   == layer4Info.getL3Info()->getSrcIp() &&
       m_sessId.getSrcPort() == layer4Info.getSrcPort()){
      return DIRECTION_TO_SERVER;
    }
    return DIRECTION_TO_CLIENT;
  }

  inline int getProtocol(){return m_protocol;}
  //inline TcpSegmentMap*  getSegmentMap(int dir){return &(m_segMap[dir]);}
  //inline RequestVector&  getRequestVector() {return m_requestVector;}
  //inline ResponseVector& getResponseVector(){return m_responseVector;}
  inline CTcpSessionId&  getTcpSessionId(){return m_sessId;}
  inline int getTcpState(){return m_tcpState;}
  inline void setNum(uint64_t num){ m_tcpTxnId = num;}
  inline uint64_t getNum(){ return m_tcpTxnId;}
  inline bool isClosed(){return m_tcpState == TCP_SM_CLOSED;}
  inline bool getActiveFlg(){return m_activeFlg;}
  inline void setActiveFlg(bool f){m_activeFlg = f;}
  inline bool shouldBlock(){return m_shouldBlock;}
  inline void setBlockFlg(bool f){m_shouldBlock = f;}
  void finishMe(const CL4Info& layer4Info, int dir);
  void finishMe(const CL4Info& layer4Info);
  void updateMe(const CL4Info& layer4Info);
  void savePayload(const CL4Info& layer4Info, int dir);
  //bool isSeqNumContinuous(int dir);
  //void printBuf(int dir);
  
  //CTcpSegmentBuf* getLastSegment(int dir);
  CHttpTxnDecoder* getHttpDecoder();
  CSmtpTxnDecoder* getSmtpDecoder();
  /*
  void dumpData(int dir, string& out);
  void dumpUnusedData(int dir, string& out);
  */
  inline time_t getLastUpdateTime(){
    return m_lastUpdateTime;
  }
  inline void setLastUpdateTime(time_t t){
    m_lastUpdateTime = t;
  }

  //bool getContinuousData(int dir, string& result);
  //tcp_seq getLastSeqNum(int dir);
  void blockMe(const CL4Info& layer4Info);
};




#endif

