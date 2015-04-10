#include "CConfig.hpp"
#include "CHttpInfo.hpp"

CHttpInfo::CHttpInfo(){
    m_contentLength = 0;
    m_currentField = "";
    m_currentValue = "";
    m_contentType = UNKNOWN_CONTENT_TYPE;
}

void CHttpInfo::printHead(){
    HeaderMap_it it;
    for (it  = m_headerMap.begin();
         it != m_headerMap.end();
         ++it){
        Debug(LOG_LEVEL_INFO, (it->first)<<": "<<(it->second));
    }
}

bool CHttpInfo::isContentTypeCared(){
    if (m_contentType == UNKNOWN_CONTENT_TYPE){
        return true;
    }
    set<string>::iterator it; 
    for(it = CConfig::getInstance("")->m_caredContentTypes.begin(); 
        it != CConfig::getInstance("")->m_caredContentTypes.end();
        ++it){
        string ct = *it;
        if (m_contentType.find(ct) != string::npos){
            return true;
        }
    }
    return false;
}



CRequestHttpInfo::CRequestHttpInfo():CHttpInfo(){
    m_url = "";
    m_method = 0;
    m_body = "";
}
void CRequestHttpInfo::printRequest(){
    Debug(LOG_LEVEL_INFO, m_method << " " << m_url);
    printHead();
    Debug(LOG_LEVEL_INFO, m_body);
}


CResponseHttpInfo::CResponseHttpInfo():CHttpInfo(){
    m_status = 0;
    m_descreption = "";
    m_body = "";
}

void CResponseHttpInfo::printResponse(){
    Debug(LOG_LEVEL_INFO, m_status);
    printHead();
    Debug(LOG_LEVEL_INFO, m_body);
}




