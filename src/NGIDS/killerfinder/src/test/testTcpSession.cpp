#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <memory>
#include <map>


#include "packet_def.hpp"
#include "CTcpSession.hpp"
#include "CTcpSessionId.hpp"

int main(int argc, char** argv){
  in_addr_t ip1 = 0x11223344;
  in_addr_t ip2 = 0xaabbccdd;

  CIpAddress srcIp(ip1);
  uint16_t srcPort = 1234;
  CIpAddress dstIp(ip2);
  uint16_t dstPort = 80;

  CTcpSessionId id(srcIp, srcPort, dstIp, dstPort);
  
  CTcpSession* tcps = new CTcpSession(id);
  printf("main\n");

  const char* request = "GET www.baidu.com HTTP/1.1\r\n\r\n";
  tcps->saveSegment(1,request,strlen(request), true);

  
  ip1.s_addr = 0xaabbccdd;
  ip2.s_addr = 0x11223344;
  port1 = 1234;
  port2 = 80;
  const char* resp1 = "200 HTTP/1.1 OK\r\n\r\n";
  const char* resp2 = "Content-Length: 12\r\n";
  const char* resp3 = "<html>test</html>";
  if (tcps->isEqualTo(ip1,port1,ip2,port2)){
    tcps->saveSegment(strlen(resp1)+1, resp2,strlen(resp2), false);
    tcps->saveSegment(1,resp1,strlen(resp1), false);
    tcps->saveSegment(strlen(resp1)+strlen(resp2)+1,resp3,strlen(resp3), false);
  }

  tcps->sortBuf();
  tcps->printBuf();
  
  delete tcps;
  return 0;
}
