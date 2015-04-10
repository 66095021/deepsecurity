#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include "CConfig.hpp"
#include "CDBManager.hpp"
#include "CHttpTxnDecoder.hpp"


#define LOG_PARSER     Debug(LOG_LEVEL_DEBUG, "["<<sess->getNum()<<"]"<<\
              __FUNCTION__<<" state:"<<p->state<<" hd_state:"<<p->header_state);

http_parser_settings CHttpTxnDecoder::settings={
    CHttpTxnDecoder::messageBeginCb,
    CHttpTxnDecoder::requestUrlCb,
    CHttpTxnDecoder::responseStatusCb,
    CHttpTxnDecoder::headerFieldCb,
    CHttpTxnDecoder::headerValueCb,
    CHttpTxnDecoder::headersCompleteCb,
    CHttpTxnDecoder::body_cb,
    CHttpTxnDecoder::messageCompleteCb
};


CHttpTxnDecoder::CHttpTxnDecoder(CTcpSession* sess){
    m_tcpSession = sess;
    m_url = "";
    m_hostField = "";
    m_hostValue = "";
    m_parserType = HTTP_REQUEST;
    m_parser = (struct http_parser*)malloc(sizeof(http_parser));
    m_activeMethod = 0;
}


CHttpTxnDecoder::CHttpTxnDecoder(){
    m_url = "";
    m_hostField = "";
    m_hostValue = "";
    m_parserType = HTTP_REQUEST;
    m_parser = (struct http_parser*)malloc(sizeof(http_parser));
    //Debug(LOG_LEVEL_DEBUG, HTTP_TAG<<"CHttpTxnDecoder in");
}

CHttpTxnDecoder::~CHttpTxnDecoder(){
    if (m_parser){
        free(m_parser);
    }
}


int CHttpTxnDecoder::messageBeginCb(http_parser* p){

    CTcpSession* sess = (CTcpSession*)p->data; 
    if (!sess){
        return 0;
    }    
    LOG_PARSER;

    if (!sess->getActiveFlg()){
        if (sess->getHttpDecoder()->getParserType() == HTTP_REQUEST){
            sess->getHttpDecoder()->m_requestVector.push_back(new CRequestHttpInfo());
        }else if (sess->getHttpDecoder()->getParserType() == HTTP_RESPONSE){
            sess->getHttpDecoder()->m_responseVector.push_back(new CResponseHttpInfo());
        }
    }
    return 0;
}

int CHttpTxnDecoder::requestUrlCb(http_parser* p, const char*buf, size_t len){
    CTcpSession* sess = (CTcpSession*)p->data;
    LOG_PARSER;
    if (sess->getActiveFlg()){
        sess->getHttpDecoder()->m_url.append(buf, len);
        if (p->state == 32){//s_req_http_start
            Debug(LOG_LEVEL_DEBUG, "["<<sess->getNum()<<"]"<<"active method "<<p->method);
            sess->getHttpDecoder()->m_activeMethod = p->method;
        }
    }else{
        //CRequestHttpInfo* request = sess->getRequestVector().back();
        //request->setUrl(buf, len);
        sess->getHttpDecoder()->m_requestVector.back()->setUrl(buf, len);
        
    }
    
    return 0;
}

int CHttpTxnDecoder::responseStatusCb(http_parser* p, const char*buf, size_t len){
    CTcpSession* sess = (CTcpSession*)p->data;
    LOG_PARSER;
    return 0;
}

int CHttpTxnDecoder::headerFieldCb(http_parser* p, const char*buf, size_t len){
    CTcpSession* sess = (CTcpSession*)p->data;
    LOG_PARSER;
    //string s(buf, len);
    //Debug(LOG_LEVEL_DEBUG, HTTP_TAG<<"["<<sess->getNum()<<"]"<<"header field:" << s << " state:" << p->state);
    if (sess->getActiveFlg()){
        string* hostF = &(sess->getHttpDecoder()->m_hostField);
        hostF->append(buf, len);
        if (p->state == 44){//s_header_value_discard_ws
            transform(hostF->begin(), hostF->end(), hostF->begin(), ::tolower);
            if (*hostF != "host"){
                hostF->clear();
            }
        }
    } else {
        if (sess->getHttpDecoder()->getParserType() == HTTP_REQUEST){
            if (!sess->getHttpDecoder()->m_requestVector.empty()){
                //CRequestHttpInfo* request = sess->getRequestVector().back();
                //request->insertField(buf, len);
                sess->getHttpDecoder()->m_requestVector.back()->insertField(buf, len);
            }
        }else{
            if (!sess->getHttpDecoder()->m_responseVector.empty()){
                //CResponseHttpInfo* response = sess->getResponseVector().back();
                //response->insertField(buf, len);
                sess->getHttpDecoder()->m_responseVector.back()->insertField(buf, len);
            }
        }
    }
    return 0;
}

int CHttpTxnDecoder::headerValueCb(http_parser* p, const char*buf, size_t len){
    CTcpSession* sess = (CTcpSession*)p->data;
    LOG_PARSER;
    //string s(buf, len);
    //Debug(LOG_LEVEL_DEBUG, HTTP_TAG<<"["<<sess->getNum()<<"]"<<"header value:" << s << " state:"<<p->state);
    if (sess->getActiveFlg()){
        if (sess->getHttpDecoder()->m_hostField == "host"){
            string& hostV = (sess->getHttpDecoder()->m_hostValue);
            hostV.append(buf, len);
            if (p->state == 50){//s_header_almost_done
                size_t pos = 0;
                string _prot = "http";
                string& url  = sess->getHttpDecoder()->m_url;
                
                if ((pos = url.find("://"))!=string::npos){
                    _prot = url.substr(0, pos);
                }
                
                string _path = url;
                if(_path[0] != '/'){
                    string tmp = url.substr(url.find("://")+3);
                    Debug(LOG_LEVEL_DEBUG, "URL "<<tmp);
                    if (tmp.find("/")!=string::npos){
                        _path = tmp.substr(tmp.find("/")); 
                    }else{
                        _path = "/";
                    }
                    
                }
                  
                bool f = CDBManager::getInstance()->isUrlHit(_prot,hostV,_path);
                sess->setBlockFlg(f);
                //url.clear();
                //url.append(_prot).append("://").append(hostV).append(_path);
            } 
        }
    } else {
        if (sess->getHttpDecoder()->getParserType() == HTTP_REQUEST){
            if (!sess->getHttpDecoder()->m_requestVector.empty()){
                //CRequestHttpInfo* request = sess->getRequestVector().back();
                //request->insertValue(buf, len);
                sess->getHttpDecoder()->m_requestVector.back()->insertValue(buf, len);
            }
        }else{
            if (!sess->getHttpDecoder()->m_responseVector.empty()){
                //CResponseHttpInfo* response = sess->getResponseVector().back();
                //response->insertValue(buf, len);
                sess->getHttpDecoder()->m_responseVector.back()->insertValue(buf, len);
            }
        }
    }

    return 0;
}

int CHttpTxnDecoder::headersCompleteCb(http_parser* p){
    CTcpSession* sess = (CTcpSession*)p->data;
    LOG_PARSER;
    if (sess->getActiveFlg()){
        string _prot;
        string _host;
        if (sess->getHttpDecoder()->m_hostValue.size()==0){
            string& url = sess->getHttpDecoder()->m_url;
            size_t hostPos = url.find("://");
            
            
            if(hostPos != string::npos){
                //http://www.baidu.com/index.html
                string prot = url.substr(0,hostPos);
                string host = url.substr(hostPos + 3);
                size_t pathPos = host.find("/");
                string path = (pathPos==string::npos?"/":host.substr(pathPos));
                host = host.substr(0, pathPos);
                 
                bool f = CDBManager::getInstance()->isUrlHit(prot, host, path);
                sess->setBlockFlg(f);
            }
        }
        sess->getHttpDecoder()->m_hostField.clear();
        sess->getHttpDecoder()->m_hostValue.clear();
        sess->getHttpDecoder()->m_url.clear();
    } 
    return 0;
}

int CHttpTxnDecoder::body_cb(http_parser* p, const char*buf, size_t len){
    CTcpSession* sess = (CTcpSession*)p->data;
    LOG_PARSER;
    if (!sess->getActiveFlg()){
        if (sess->getHttpDecoder()->getParserType() == HTTP_REQUEST){
            if (!sess->getHttpDecoder()->m_requestVector.empty()){
                //CRequestHttpInfo* request = sess->getRequestVector().back();
                //request->setBody(buf, len);
                sess->getHttpDecoder()->m_requestVector.back()->setBody(buf, len);
            }
        }else{
            if (!sess->getHttpDecoder()->m_responseVector.empty()){
                //CResponseHttpInfo* response = sess->getResponseVector().back();
                //response->setBody(buf, len);
                sess->getHttpDecoder()->m_responseVector.back()->setBody(buf, len);
            }
        }
    }
    
    return 0;
}

int CHttpTxnDecoder::messageCompleteCb(http_parser* p){
    CTcpSession* sess = (CTcpSession*)p->data;
    LOG_PARSER;
    if (!sess->getActiveFlg()){
        if (sess->getHttpDecoder()->getParserType() == HTTP_REQUEST){
            if (!sess->getHttpDecoder()->m_requestVector.empty()){
                //CRequestHttpInfo* request = sess->getRequestVector().back();
                //request->setMethod(p->method);
                sess->getHttpDecoder()->m_requestVector.back()->setMethod(p->method);
            }
        }else{
            if (!sess->getHttpDecoder()->m_responseVector.empty()){
                //CResponseHttpInfo* response = sess->getResponseVector().back();
                //response->setStatus(p->status_code);
                sess->getHttpDecoder()->m_responseVector.back()->setStatus(p->status_code);
            }
        }
    }
    return 0;
}



/*
void CHttpTxnDecoder::parserInit(void* data, enum http_parser_type type){
    m_parserType = type;
    memset(m_parser, 0, sizeof(struct http_parser));
    m_parser->data = data;
    http_parser_init(m_parser, m_parserType);
}
*/

void CHttpTxnDecoder::parserInit(enum http_parser_type type){
    m_parserType = type;
    memset(m_parser, 0, sizeof(struct http_parser));
    m_parser->data = m_tcpSession;
    http_parser_init(m_parser, m_parserType);
}


bool CHttpTxnDecoder::parserExecute(const char* buf, size_t len){
    size_t nparsed = http_parser_execute(m_parser, &settings, buf, len);
    enum http_errno err = HTTP_PARSER_ERRNO(m_parser);
    if (err != 0){
        Debug(LOG_LEVEL_ERR, HTTP_TAG<<"http parser err " << http_errno_description(err)<<" "<<nparsed);
        return false;
    }
    return true;
}

/*
bool CHttpTxnDecoder::decodeHttpActive(CTcpSession* sess, enum http_parser_type type){

    Debug(LOG_LEVEL_DEBUG, HTTP_TAG<<"["<<sess->getNum()<<"]"<<"realtime mode decoder");
    CTcpSegmentBuf* lastBuf = sess->getLastSegment(DIRECTION_TO_SERVER);
    if (lastBuf->getInuseFlg()){
        return true;
    }
    lastBuf->setInuseFlg(true);
    if (!parserExecute(lastBuf->getBuf(), lastBuf->getSegSize())){
        Debug(LOG_LEVEL_ERR, HTTP_TAG<<"["<<sess->getNum()<<"]"<<"decode HTTP error");
        lastBuf->printBuf();
        return false;
    }
    return true;
}
*/

bool CHttpTxnDecoder::decodeHttpActive(enum http_parser_type type){

    Debug(LOG_LEVEL_DEBUG, HTTP_TAG<<"["<<m_tcpSession->getNum()<<"]"<<"realtime mode decoder");
 
    string buf;
    /*
    m_tcpSession->dumpUnusedData(type == HTTP_REQUEST ?\
                                 DIRECTION_TO_SERVER:DIRECTION_TO_CLIENT, buf);*/
                                 
    int dir = (type == HTTP_REQUEST ? DIRECTION_TO_SERVER:DIRECTION_TO_CLIENT);
    m_tcpSession->getContinuousBuf(dir).dumpUnusedData(buf);
    if (!parserExecute(buf.c_str(), buf.size())){
        Debug(LOG_LEVEL_ERR, HTTP_TAG<<"["<<m_tcpSession->getNum()<<"]"<<"decode HTTP error");
        Debug(LOG_LEVEL_ERR, HTTP_TAG<<"["<<m_tcpSession->getNum()<<"]"<<buf);
        parserInit(type);
        m_tcpSession->getContinuousBuf(dir).setUsedPos(0);
        return false;
    }
    return true;


}


/*
bool CHttpTxnDecoder::decodeHttpInactive(CTcpSession* sess, enum http_parser_type type){

    Debug(LOG_LEVEL_DEBUG, HTTP_TAG<<"["<<sess->getNum()<<"]"<<"offtime mode decoder");
    string buf;
    sess->dumpData(type == HTTP_REQUEST ? \
                   DIRECTION_TO_SERVER : DIRECTION_TO_CLIENT, buf);
    if (!parserExecute(buf.c_str(), buf.length())){
       Debug(LOG_LEVEL_ERR, HTTP_TAG<<"["<<sess->getNum()<<"]"<<"decode HTTP error");
       return false;
    }
    return true;
}
*/

bool CHttpTxnDecoder::decodeHttpInactive(enum http_parser_type type){

    Debug(LOG_LEVEL_DEBUG, HTTP_TAG<<"["<<m_tcpSession->getNum()<<"]"<<"offtime mode decoder");
    int dir = (type == HTTP_REQUEST ? DIRECTION_TO_SERVER : DIRECTION_TO_CLIENT);
    string& buf = m_tcpSession->getContinuousBuf(dir).getBuf();

    if (!parserExecute(buf.c_str(), buf.length())){
       Debug(LOG_LEVEL_ERR, HTTP_TAG<<"["<<m_tcpSession->getNum()<<"]"<<"decode HTTP error");
       Debug(LOG_LEVEL_ERR, HTTP_TAG<<"["<<m_tcpSession->getNum()<<"]"<<buf);
       return false;
    }
    return true;
}

/*
// active   decode: decode packet by packet, url may be in different packet 
// inactive decode: collect all payload when session closed, then decode it 
//                  in one time
bool CHttpTxnDecoder::decodeHttp(CTcpSession* sess, enum http_parser_type type){
    if (sess->getActiveFlg()){
        return decodeHttpActive(sess, type);
    } else {
        return decodeHttpInactive(sess, type);
    }
    return true;
}
*/

bool CHttpTxnDecoder::decodeHttp(enum http_parser_type type){
    if (!m_tcpSession){
        return false;
    }
    if (m_tcpSession->getActiveFlg()){
        return decodeHttpActive(type);
    } else {
        return decodeHttpInactive(type);
    }
    return true;
}

bool CHttpTxnDecoder::decodeHttp(){
    if (!m_tcpSession){
        return false;
    }
    if (!m_tcpSession->getActiveFlg()){
        parserInit(HTTP_REQUEST);
        decodeHttp(HTTP_REQUEST);
        parserInit(HTTP_RESPONSE);
        decodeHttp(HTTP_RESPONSE);
        return true;
    }else{
        return false;
    }
    
}


void CHttpTxnDecoder::printHttp(){
    size_t len = m_requestVector.size();
    Debug(LOG_LEVEL_INFO, "-----http request begin----");
    for (size_t i = 0; i < len; i++){
        m_requestVector[i]->printRequest();
    }
    Debug(LOG_LEVEL_INFO, "-----http request end------");

    len = m_responseVector.size();
    Debug(LOG_LEVEL_INFO, "-----http response begin---");
    for (size_t i = 0; i < len; i++){   
        m_responseVector[i]->printResponse();
    }
    Debug(LOG_LEVEL_INFO, "-----http response end-----");
}

void CHttpTxnDecoder::saveToFile(){

    ostringstream path;
    path<<CConfig::getInstance("")->m_rootPath<<"/"<<"files";
    if (!isFileSystemOk(path.str())){
        return;
    }

    time_t _t;time(&_t);int dir = _t/60;
    path<<"/"<<dir;
    if (!makeDir(path.str())){
        return;
    }
    
    size_t len = m_responseVector.size()< m_requestVector.size()?
                   m_responseVector.size():m_requestVector.size() ;
    for (size_t i = 0; i < len; i++){  
        if (m_responseVector[i]->getStatus() != 200){
            Debug(LOG_LEVEL_DEBUG, "ignore status code " << m_responseVector[i]->getStatus());  
            continue;
        }
        if (!m_responseVector[i]->isContentTypeCared()){
            Debug(LOG_LEVEL_DEBUG, "ignore content type "<<m_responseVector[i]->getContentType());
            continue;
        }
        if (m_responseVector[i]->getBody().size()==0){
            Debug(LOG_LEVEL_DEBUG, "ignore empty body");
            continue;
        }

        //+session_id
        ostringstream name; 
        m_tcpSession->getTcpSessionId().getIdStr(name);   

        //+content_type
        string ct = m_responseVector[i]->getContentType();
        string ct_new;
        for (size_t index = 0; index < ct.size(); index++){
            if (ct[index] == '/'){
                ct_new = ct_new + "+";
            }else if (ct[index] == ' '){
                ;
            }else{
                ct_new = ct_new + ct[index];
            }
        }

        //+i
        name<<"_"<<ct_new<<"_"<<i;

        
        //+fileName
        string tmpurl = m_requestVector[i]->getUrl();
        if (tmpurl.find("://")!=string::npos){
            tmpurl = tmpurl.substr(tmpurl.find("://") + 3);
        }
        size_t pos = tmpurl.find_last_of("/");
        if(pos == string::npos || (pos+1) == tmpurl.size()){
            name<<"_name=none";
        }else{
            string tmpname = tmpurl.substr(pos+1);
            if(tmpname.find("?")!=string::npos){
                tmpname = tmpname.substr(0, tmpname.find("?"));
            }else if(tmpname.find("#")!=string::npos){
                tmpname = tmpname.substr(0, tmpname.find("#"));
            }
            name<<"_name="<<tmpname;
        }
        
        Debug(LOG_LEVEL_DEBUG, "file name "<<name.str());   

        ofstream file;
        path<<"/"<<name.str();
        file.open(path.str().c_str());
        file<<m_responseVector[i]->getBody();
        file.close();
    }
    
}






