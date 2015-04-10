#ifndef _CTCP_SESSION_PLUGIN_HPP_
#define _CTCP_SESSION_PLUGIN_HPP_

#include "CHttpTxnDecoder.hpp"
#include "CTcpSession.hpp"

class CTcpSessionPlugin
{
public:
    static void sessionCreated(CTcpSession* sess);
    static void payloadReceived(CTcpSession* sess, int triggerPacketDir);
    static void revFirstPacket(CTcpSession* sess, int triggerPacketDir);
    static void directionReverse(CTcpSession* sess, int triggerPacketDir);
    static void finReceived(CTcpSession* sess, int triggerPacketDir);
    static void sessionClosed(CTcpSession* sess, int triggerPacketDir);
    static void rstReceived(CTcpSession* sess, int triggerPacketDir);
    static void sessionPosted(CTcpSession* sess);
};

#endif
