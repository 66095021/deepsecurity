#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pcap.h>


#include <arpa/inet.h>

#include "CDBManager.hpp"
#include "CPacketHandler.hpp"
#include "CLog.hpp"
#include "CConfig.hpp"
#include "CMain.hpp"
#include "CTxnDecodeThread.hpp"
#include "CPacketCaptureThread.hpp"
#include "CTcpSessionPool.hpp"




/* default snap length (maximum bytes per packet to capture) */
#ifndef SNAP_LEN
#define SNAP_LEN 1518
#endif

CMain::CMain(){

}

CMain::~CMain(){
  
}

void CMain::printHelp(){
    printf("\n"
           "killer-finder -m mode -f filter -n packetNum -d dev -o offlineFile -c configFile\n"
           "    -m : mode [online] or [offline], default online\n"
           "    -f : filter apply tcpdump filter, default all tcp\n"
           "    -n : total packet number before exit, -1 forever, default -1\n"
           "    -d : dev, default eth0\n"
           "    -o : offline file name, default is ./killer-finder.pcap\n"
           "    -c : config file name, defalut is /usr/local/etc/config.xml\n"
           "    -h : print this help info\n");
}

void CMain::parserCmdLine(int argc, char** argv){

    string _mode;
    string _filter;
    string _packetNum;
    string _device;
    string _offlineFile;
    string _configFile;

    int arg = 0;
    while ((arg = getopt(argc, argv, "m:f:n:d:o:c:h")) != -1) {
      switch (arg) {
      case 'm':
          _mode = optarg; break;
      case 'f':
          _filter = optarg;break;
      case 'n':
          _packetNum = optarg; break;
      case 'd':
          _device = optarg;break;    
      case 'o':
          _offlineFile = optarg; break;
      case 'c':
          _configFile = optarg;break; 
      case 'h':
          printHelp();
          exit(0);
      default:
          break;
      }
    }

    CConfig* config = CConfig::getInstance(_configFile.size()==0 ?
                               "/usr/local/killer/etc/config.xml" : _configFile );

   
    if (_mode.size() != 0){
      if (_mode == "online" || _mode == "offline"){
        config->m_mode = _mode;
      }
    }

    if (_filter.size() != 0){
        config->m_filter = _filter;
    }
    
    if (_packetNum.size() != 0){
         config->m_packetNum= atoi(_packetNum.c_str());
    }

    if (_device.size() != 0){
        config->m_dev = _device;
    }

    if (_offlineFile.size() != 0){
        config->m_offlineFile = _offlineFile;
    }

}


int CMain::Main(int argc, char** argv){

  parserCmdLine(argc, argv);

  CLog::getInstance()->start();

  CDBManager::getInstance()->start();

  CTcpSessionPool::getInstance()->start();

  CTxnDecodeThread::getInstance()->start();

  CPacketCaptureThread::getInstance()->start();

  CPacketCaptureThread::getInstance()->join();

  CTxnDecodeThread::getInstance()->stop();

  CTxnDecodeThread::getInstance()->join();
  
  return 0;
}



