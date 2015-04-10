#include <stdio.h>
#include <arpa/inet.h>
#include <algorithm>
#include <vector>

#include "CDBManager.hpp"
#include "CHttpTxnDecoder.hpp"
#include "CSmtpTxnDecoder.hpp"
#include "CTcpSession.hpp"
#include "CLog.hpp"
#include "CTcpSessionPlugin.hpp"
#include "CConfig.hpp"
#include "CPacketCreater.hpp"


CTcpSegBuf::CTcpSegBuf(){
    m_start = 0;
    m_end   = 0;
    m_used  = 0;
    m_buf.clear();
}

void CTcpSegBuf::setStartSeq(tcp_seq seq){
    m_start = seq;
}

void CTcpSegBuf::insertData(const tcp_seq seq, const uint8_t* buf, size_t size){
    if(m_start == 0){
        m_start = seq;
    }
    
    if (seq + size > m_end){
        m_end = seq + size;
        m_buf.resize(m_end - m_start);
    }

    m_buf.replace(seq-m_start, size, (const char*)buf, size);
}

void CTcpSegBuf::dumpUnusedData(string& out){
    out.clear();
    Debug(LOG_LEVEL_DEBUG, m_start<<"-"<<m_used<<"-"<<m_end);
    out.append(m_buf, (m_used-m_start), (m_end-m_start)-(m_used-m_start));
    m_used = m_end;
}

string& CTcpSegBuf::getBuf(){
    return m_buf;
}


void CTcpSegBuf::setUsedPos(tcp_seq used){
    m_used = m_start + used;
}






CTcpSession::CTcpSession(const CTcpSessionId& sessId):m_sessId(sessId){
    m_tcpState  = TCP_SM_HANDSHAKE;
    m_activeFlg = true;
    m_httpDecoder = new CHttpTxnDecoder(this);
    m_smtpDecoder = new CSmtpTxnDecoder(this);
    if( CConfig::getInstance("")->m_httpPorts.find(m_sessId.getDstPort())
        != CConfig::getInstance("")->m_httpPorts.end()){
        m_protocol = PROTOCOL_HTTP;
    } else if (CConfig::getInstance("")->m_smtpPorts.find(m_sessId.getDstPort())
               != CConfig::getInstance("")->m_smtpPorts.end()){
        m_protocol = PROTOCOL_SMTP;
    } else {
        m_protocol = PROTOCOL_UNKNOWN;
    }
    m_shouldBlock = false;
    
}

CTcpSession::~CTcpSession(){
  /*
  TcpSegmentMap_it it;
  for (int i = 0; i < 2; i++){
      for (it = m_segMap[i].begin(); 
         it != m_segMap[i].end();
         it++){
      delete (CTcpSegmentBuf*)(it->second);
    }
  }
  */
  if (m_httpDecoder){
    delete m_httpDecoder;
  }
  if (m_smtpDecoder){
    delete m_smtpDecoder;
  }
}  

CHttpTxnDecoder* CTcpSession::getHttpDecoder() {
    return m_httpDecoder;
}

CSmtpTxnDecoder* CTcpSession::getSmtpDecoder() {
    return m_smtpDecoder;
}



void CTcpSession::updateMe(const CL4Info& layer4Info){
    if (layer4Info.isSynSet()){
        m_tcpState = TCP_SM_HANDSHAKE;
        if (layer4Info.isAckSet()){
            m_continuousSeg[DIRECTION_TO_CLIENT].setStartSeq(layer4Info.getSeqNum()+1);
        }
        return;
    }

    int dir = getDirection(layer4Info);

    Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<getNum()<<"] before update, sm:"<<m_tcpState);

    if (layer4Info.getL5DataLen() > 0){
        Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<getNum()<<"] payload len is "\
                               <<layer4Info.getL5DataLen()<<",dir "<<dir);

        savePayload(layer4Info, dir);
        switch(m_tcpState){
        case TCP_SM_INIT:
        case TCP_SM_HANDSHAKE:
            CTcpSessionPlugin::revFirstPacket(this, dir);
            if (dir == DIRECTION_TO_SERVER){
              m_tcpState = TCP_SM_CLIENT_SENDING;
            }else{
              m_tcpState = TCP_SM_SERVER_SENDING;
            }
            break;
        case TCP_SM_CLIENT_SENDING:
            if (dir == DIRECTION_TO_SERVER){
              ;
            }else{
              //client has sent all packets to server
              CTcpSessionPlugin::directionReverse(this, dir);
              m_tcpState = TCP_SM_SERVER_SENDING;
            }
            break;
        case TCP_SM_SERVER_SENDING:
            if (dir == DIRECTION_TO_SERVER){
              //server has sent all packets to client
              CTcpSessionPlugin::directionReverse(this, dir);
              m_tcpState = TCP_SM_CLIENT_SENDING;
            }else{
              ;
            }
            break;
        case TCP_SM_SERVER_FINISHED:
            if (dir == DIRECTION_TO_SERVER){
              ;
            }else{
              Debug(LOG_LEVEL_ERR, TCP_SESSION_TAG<<"should never be here 1");
              m_tcpState = TCP_SM_CLOSED;
            }
            break;
            
        case TCP_SM_CLIENT_FINISHED:
            if (dir == DIRECTION_TO_SERVER){
              Debug(LOG_LEVEL_ERR, TCP_SESSION_TAG<<"should never be here 2");
              m_tcpState = TCP_SM_CLOSED;
            }else{
              ;
            }
            break;
        case TCP_SM_CLOSED:
        default:
            break;
        }
        CTcpSessionPlugin::payloadReceived(this, dir);
    }

    //FIN may be with payload packet
    finishMe(layer4Info, dir);
    
    Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<getNum()<<"] after update, sm:"<<m_tcpState);
    return;
    
}

void CTcpSession::savePayload(const CL4Info& layer4Info, int dir){
    if(dir != 0 && dir != 1){
        return;
    }

    m_continuousSeg[dir].insertData(layer4Info.getSeqNum(),
                                    layer4Info.getL5Data(),
                                    layer4Info.getL5DataLen());

    /*
    CTcpSegmentBuf* buf = new CTcpSegmentBuf(layer4Info.getSeqNum(), 
                                             layer4Info.getL5Data(),
                                             layer4Info.getL5DataLen());
    m_segMap[dir][layer4Info.getSeqNum()] = buf;
    */
    return;
}

void CTcpSession::finishMe(const CL4Info& layer4Info, int dir){
    if (layer4Info.isRstSet()){
        Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<getNum()<<"] RST received, dir "<<dir);
        CTcpSessionPlugin::rstReceived(this, dir);
        m_tcpState = TCP_SM_CLOSED;
        return;
    }
    if (!layer4Info.isFinSet()){
        return;
    }

    Debug(LOG_LEVEL_DEBUG, TCP_SESSION_TAG<<"["<<getNum()<<"] FIN received, dir "<<dir);
    CTcpSessionPlugin::finReceived(this, dir);

    switch (m_tcpState){
    case TCP_SM_INIT:
    case TCP_SM_HANDSHAKE:
    case TCP_SM_CLIENT_SENDING:
    case TCP_SM_SERVER_SENDING: 
      if (dir == DIRECTION_TO_CLIENT){
          m_tcpState = TCP_SM_SERVER_FINISHED;
      }else{
          m_tcpState = TCP_SM_CLIENT_FINISHED;
      }
      break;
    case TCP_SM_SERVER_FINISHED:
      if (dir == DIRECTION_TO_CLIENT){
          ;
      }else{
          m_tcpState = TCP_SM_CLOSED;
      }
      break;
    case TCP_SM_CLIENT_FINISHED:
      if (dir == DIRECTION_TO_CLIENT){
          m_tcpState = TCP_SM_CLOSED;
      }else{
          ;
      }
      break;
    case TCP_SM_CLOSED:
    default:
      break;
    }

}

void CTcpSession::finishMe(const CL4Info& layer4Info){
  finishMe(layer4Info, getDirection(layer4Info));
  return;
}

void CTcpSession::blockMe(const CL4Info& layer4Info){

    uint16_t srcPort, dstPort;
    srcPort = layer4Info.getSrcPort();
    dstPort = layer4Info.getDstPort();
    uint16_t id = 0x1111;//ntohs(layer4Info.getL3Info()->getId());
    uint32_t seq, ack;
    seq = layer4Info.getSeqNum() + layer4Info.getL5DataLen();
    ack = layer4Info.getAckNum();   


    
    if (layer4Info.getL3Info()->getL2Info()->getIpVersion() == 4){
        uint32_t srcIp, dstIp;
        layer4Info.getL3Info()->getSrcIp().getIpv4(&srcIp);
        layer4Info.getL3Info()->getDstIp().getIpv4(&dstIp);
        if (CDBManager::getInstance()->isIpv4Hit(srcIp) ||
            CDBManager::getInstance()->isIpv4Hit(srcIp)){
            m_shouldBlock = true;
        }

        if(m_shouldBlock){
            Debug(LOG_LEVEL_DEBUG, "block.ip "<<srcIp<<" "<<dstIp<<" id "<<id<<\
                      " port "<<srcPort<<" "<<dstPort<<" seq "<<seq<<" ack "<<ack);
            // send RST to sender
            CPacketCreater::sendIpv4Rst(dstIp,srcIp,id,dstPort,srcPort,ack,seq);
            // send RST to reciver
            //CPacketCreater::sendIpv4Rst(srcIp,dstIp,id,srcPort,dstPort,seq,0);
        }

        
    }else if(layer4Info.getL3Info()->getL2Info()->getIpVersion() == 6){
    
    }


}

/*
bool CTcpSession::isSeqNumContinuous(int dir){
    if(dir != 0 && dir != 1){
        return false;
    }
    int index = 0;
    bool firstFlag = true;
    bool ret = true;
    tcp_seq expectedSeq = 0;
    TcpSegmentMap_it it;
    CTcpSegmentBuf* buf = NULL;
    for (it = m_segMap[dir].begin(); 
         it != m_segMap[dir].end();
         it++){
      index++;
      if (index%1024==0){printf("\n");}
      buf = (CTcpSegmentBuf*)(it->second);
      if (firstFlag){
        expectedSeq = buf->getSeq() + buf->getSegSize();
        firstFlag = false;
        printf("%u-%u ", buf->getSeq(), expectedSeq - 1);
        continue;
      } 
      if (expectedSeq != buf->getSeq()){
        ret = false;
      }
      expectedSeq = buf->getSeq() + buf->getSegSize();
      printf("%u-%u ", buf->getSeq(), expectedSeq - 1);
    }
    printf("\n");

    return ret;
}
*/
CTcpSegBuf& CTcpSession::getContinuousBuf(int dir){  
    return m_continuousSeg[dir];
}

/*
bool CTcpSession::getContinuousData(int dir, string& result){
    if (dir != 0 && dir != 1){
        return false;
    }
    tcp_seq start = ((CTcpSegmentBuf*)(m_segMap[dir].begin()->second))->getSeq();
    tcp_seq end = getLastSeqNum(dir);

    //string& result = m_continuousSeg[dir];
    result.clear();
    result.resize(end-start);  
    Debug(LOG_LEVEL_DEBUG, "total size "<<(end-start));
    
    TcpSegmentMap_it it;
    CTcpSegmentBuf* buf = NULL;
    for (it = m_segMap[dir].begin(); 
         it != m_segMap[dir].end();
         it++){
      buf = (CTcpSegmentBuf*)(it->second);
      result.replace(buf->getSeq()-start,
                     buf->getSegSize(),
                     buf->getBuf(),
                     buf->getSegSize());
    }
    Debug(LOG_LEVEL_DEBUG, result);
    return true;

}
*/

/*
tcp_seq CTcpSession::getLastSeqNum(int dir){
    if (dir != 0 && dir != 1){
        return false;
    }

    tcp_seq max = 0;
    
    TcpSegmentMap_it it;
    CTcpSegmentBuf* buf = NULL;
    for (it = m_segMap[dir].begin(); 
         it != m_segMap[dir].end();
         it++){
      buf = (CTcpSegmentBuf*)(it->second);
      if (buf->getSeq() + buf->getSegSize() > max){
        max = buf->getSeq() + buf->getSegSize();
      }
    }
    return max;
}
*/



/*
CTcpSegmentBuf* CTcpSession::getLastSegment(int dir){
    if (dir != 0 && dir != 1){
        return NULL;
    }
    TcpSegmentMap_it it = m_segMap[dir].end();
    --it;
    return (CTcpSegmentBuf*)(it->second);
    
}
*/

/*
void CTcpSession::printBuf(int dir){
    if(dir != 0 && dir != 1){
        return;
    }

    TcpSegmentMap_it it;
    CTcpSegmentBuf* buf = NULL;
    for (it = m_segMap[dir].begin(); 
         it != m_segMap[dir].end();
         it++){
        buf = (CTcpSegmentBuf*)(it->second);
        buf->printBuf();
    }
}
*/

/*
void CTcpSession::printHttp(){
    size_t len = m_requestVector.size();
    for (size_t i = 0; i < len; i++){
        Debug(LOG_LEVEL_DEBUG, "-----http request begin----")
        m_requestVector[i]->printRequest();
        Debug(LOG_LEVEL_DEBUG, "-----http request end------")
    }

    len = m_responseVector.size();
    for (size_t i = 0; i < len; i++){
        Debug(LOG_LEVEL_DEBUG, "-----http response begin---")
        m_responseVector[i]->printResponse();
        Debug(LOG_LEVEL_DEBUG, "-----http response end-----")
    }
}
*/

/*
void CTcpSession::dumpData(int dir, string& out){
    if (dir != 0 && dir != 1){
        return;
    }
    
    out.clear();
    //if(isSeqNumContinuous(dir)){
        TcpSegmentMap_it it;
        CTcpSegmentBuf* buf = NULL;
        for (it = m_segMap[dir].begin(); 
             it != m_segMap[dir].end();
             it++){
            buf = (CTcpSegmentBuf*)(it->second);
            out.append(buf->getBuf(), buf->getSegSize());
        }  
    //}else{
    //    getContinuousData(dir, out);
    //}
}

void CTcpSession::dumpUnusedData(int dir, string& out){
    if (dir != 0 && dir != 1){
        return;
    }
    out.clear();
    TcpSegmentMap_it it;
    CTcpSegmentBuf* buf = NULL;
    for (it = m_segMap[dir].begin(); 
         it != m_segMap[dir].end();
         it++){
        buf = (CTcpSegmentBuf*)(it->second);
        if (!buf->getInuseFlg()){
            out.append(buf->getBuf(), buf->getSegSize());
            buf->setInuseFlg(true);
        }
    } 

}
*/
/*
void CTcpSession::getUnusedData(int dir, string& out){
    if (dir != 0 && dir != 1){
        return;
    }

    m_continuousSeg[dir].dumpUnusedData(out);
}
*/



