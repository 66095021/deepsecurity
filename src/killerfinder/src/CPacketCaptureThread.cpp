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

#include "CPacketHandler.hpp"
#include "CLog.hpp"
#include "CConfig.hpp"

#include "CPacketCaptureThread.hpp"

#ifndef SNAP_LEN
#define SNAP_LEN 1518
#endif


CPacketCaptureThread::CPacketCaptureThread(){
    m_isRunning = false;
}

CPacketCaptureThread* CPacketCaptureThread::getInstance(){
    static CPacketCaptureThread* instance = NULL;
    if (instance == NULL){
        instance = new CPacketCaptureThread();
    }
    return instance;
}

void* CPacketCaptureThread::threadMain(void *arg){
    CConfig* config = CConfig::getInstance("./config.xml");
    
    const char *dev = NULL;
    const char* filter_exp = "tcp";
    int num_packets = config->m_packetNum;
    
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    
    
    struct bpf_program fp;
    bpf_u_int32 mask = 0;
    bpf_u_int32 net = 0;
    
    if (config->m_mode == "offline"){
      handle = pcap_open_offline(config->m_offlineFile.c_str(), errbuf);
      if (handle == NULL){    
        Debug(LOG_LEVEL_ERR, "Couldn't open file " <<config->m_offlineFile << errbuf);
        exit(EXIT_FAILURE);
      }
      Debug(LOG_LEVEL_DEBUG, "open "<<config->m_offlineFile);
    
    } else if (config->m_mode == "online"){
      dev = config->m_dev.c_str();
      if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        Debug(LOG_LEVEL_ERR, "Couldn't get netmask for device " <<dev << " " <<errbuf);
        net = 0;
        mask = 0;
      }
      
      Debug(LOG_LEVEL_DEBUG, "Device: "<< dev);
    
      handle = pcap_open_live(dev, SNAP_LEN, 1, 
                              CConfig::getInstance("")->m_pcapTimeout, errbuf);
      if (handle == NULL) {
        Debug(LOG_LEVEL_ERR, "Couldn't open device " << dev << " " << errbuf);
        exit(EXIT_FAILURE);
      }
    
    } else {
      exit(EXIT_FAILURE);
    }
    
    CPacketHandler::DLT = pcap_datalink(handle); 
    
    if (config->m_filter.c_str() != NULL && config->m_filter.length()>0){
      filter_exp = config->m_filter.c_str();
    }
    
    
    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
      Debug(LOG_LEVEL_ERR, "Couldn't parse filter " <<filter_exp<<":"<<pcap_geterr(handle));
      exit(EXIT_FAILURE);
    }
    
    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
      Debug(LOG_LEVEL_ERR, "Couldn't install filter "<<filter_exp<<":"<<pcap_geterr(handle));
      exit(EXIT_FAILURE);
    }
    
    Debug(LOG_LEVEL_DEBUG,"Number of packets: "<< num_packets);
    Debug(LOG_LEVEL_DEBUG,"Filter expression: "<< filter_exp);
    
    /* now we can set our callback function */
    pcap_loop(handle, num_packets, CPacketHandler::packetCallBack, NULL);
    
    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);
    
    Debug(LOG_LEVEL_DEBUG,"Capture complete");
    return NULL;
} 

void CPacketCaptureThread::start(){
    if (m_isRunning){
        return;
    }

    int ret = pthread_create(&m_pid, NULL, CPacketCaptureThread::threadMain, (void*)this);
    if (ret != 0){
        Debug(LOG_LEVEL_ERR, "pthread_create failed!!");
        return;
    }
    
    m_isRunning = true;
    return;

}

void CPacketCaptureThread::join(){
    pthread_join(m_pid, NULL);
}

void CPacketCaptureThread::stop(){
    m_isRunning = false;
}

