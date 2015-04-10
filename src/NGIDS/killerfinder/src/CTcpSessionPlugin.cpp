#include "CTcpSessionPlugin.hpp"



void CTcpSessionPlugin::sessionCreated(CTcpSession* sess){
    if (!sess){
        return;
    }
 
    switch (sess->getProtocol()){
    case PROTOCOL_HTTP:
        sess->setActiveFlg(true);
        sess->getHttpDecoder()->parserInit(HTTP_REQUEST);
        break;
    case PROTOCOL_SMTP:
        break;
    default:
        break;
    }


}

void CTcpSessionPlugin::payloadReceived(CTcpSession* sess, int triggerPacketDir){

    if (!sess){
        return;
    }

    switch (sess->getProtocol()){
    case PROTOCOL_HTTP:
        if (sess->getActiveFlg() && triggerPacketDir == DIRECTION_TO_SERVER){
            sess->getHttpDecoder()->decodeHttp(HTTP_REQUEST);
        }
        break;
    case PROTOCOL_SMTP:
        break;
    default:
        break;
    }

}

void CTcpSessionPlugin::revFirstPacket(CTcpSession* sess, int triggerPacketDir){

}
void CTcpSessionPlugin::directionReverse(CTcpSession* sess, int triggerPacketDir){
    /*
    if (!sess){
        return;
    }

    switch (sess->getProtocol()){
    case PROTOCOL_HTTP:
        if (sess->getActiveFlg() && triggerPacketDir == DIRECTION_TO_CLIENT){
            sess->getHttpDecoder()->decodeHttpActive(HTTP_REQUEST);
        }
        break;
    case PROTOCOL_SMTP:
        break;
    default:
        break;
    }  
    */
}
void CTcpSessionPlugin::finReceived(CTcpSession* sess, int triggerPacketDir){

}
void CTcpSessionPlugin::sessionClosed(CTcpSession* sess, int triggerPacketDir){

}
void CTcpSessionPlugin::rstReceived(CTcpSession* sess, int triggerPacketDir){

}
void CTcpSessionPlugin::sessionPosted(CTcpSession* sess){

}


