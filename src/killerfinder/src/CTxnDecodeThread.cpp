//#include <unistd.h>

#include "CHttpTxnDecoder.hpp"
#include "CSmtpTxnDecoder.hpp"
#include "CTcpSessionPool.hpp"
#include "CLog.hpp"
#include "CTxnDecodeThread.hpp"
#include "CConfig.hpp"



CTxnDecodeThread::CTxnDecodeThread(){
    m_isRunning = false;
}

CTxnDecodeThread::~CTxnDecodeThread(){

}


CTxnDecodeThread* CTxnDecodeThread::getInstance(){
    static CTxnDecodeThread * instance = NULL;
    if (instance == NULL){
        instance = new CTxnDecodeThread();
    }
    return instance;
}

void CTxnDecodeThread::start(){
    if (m_isRunning){
        return;
    }

    int ret = pthread_create(&m_pid, NULL, CTxnDecodeThread::threadMain, (void*)this);
    if (ret != 0){
        Debug(LOG_LEVEL_ERR, "pthread_create failed");
        return;
    }
    
    m_isRunning = true;
    Debug(LOG_LEVEL_DEBUG, "decoder thread works now");
    return;
}

void CTxnDecodeThread::stop(){
    m_isRunning = false;
    return;
}


void CTxnDecodeThread::join(){
    pthread_join(m_pid, NULL);
}

void* CTxnDecodeThread::threadMain(void *arg){
    Debug(LOG_LEVEL_DEBUG, X_THREAD_TAG<<"thread in");
    CTcpSession* sess = NULL;
    CTxnDecodeThread* myself = (CTxnDecodeThread*)arg;

    while (myself->m_isRunning){  
        while((sess = CTcpSessionPool::getInstance()->popClosedSession()) != NULL){
            Debug(LOG_LEVEL_DEBUG, X_THREAD_TAG<<"["<<sess->getNum()<<"] protocol:"<<sess->getProtocol());
            sess->setActiveFlg(false);
            switch (sess->getProtocol()){
            case PROTOCOL_HTTP:
                if (CConfig::getInstance("")->m_caredHttpMethods.find(sess->getHttpDecoder()->m_activeMethod)== 
                    CConfig::getInstance("")->m_caredHttpMethods.end()){
                    Debug(LOG_LEVEL_WARNING, "["<<sess->getNum()<<"] ignore method "<<sess->getHttpDecoder()->m_activeMethod);
                    break;  
                }else{
                    Debug(LOG_LEVEL_WARNING, "["<<sess->getNum()<<"] cared method "<<sess->getHttpDecoder()->m_activeMethod);
                }
                sess->getHttpDecoder()->decodeHttp();
                sess->getHttpDecoder()->saveToFile();
                sess->getHttpDecoder()->printHttp();
                break;
            case PROTOCOL_SMTP: 
                sess->getSmtpDecoder()->decodeSmtp();
                sess->getSmtpDecoder()->saveToFile();
                sess->getSmtpDecoder()->printMailsInfo();
                break;
            default:
                break;
            }
            
            delete sess;
        }
        
    }
    Debug(LOG_LEVEL_DEBUG, X_THREAD_TAG<<"thread out");
    return 0;
}





