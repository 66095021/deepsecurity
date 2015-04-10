#ifndef _CHTTP_INFO_HPP_
#define _CHTTP_INFO_HPP_

#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <http_parser.h>
#include "CLog.hpp"


using namespace std;

typedef map<string, string> HeaderMap;
typedef map<string, string>::iterator HeaderMap_it;

#define UNKNOWN_CONTENT_TYPE "UnknownType"

class CHttpInfo{
protected:    
    string    m_currentField;
    string    m_currentValue;
    HeaderMap m_headerMap;
    string    m_body;
    uint64_t  m_contentLength;
    string    m_contentEncoding;

    string    m_contentType;
    
public:
    CHttpInfo();
    inline void insertField(const char* buf, size_t len){
        if (m_currentField.empty()){
            m_currentField.append(buf,len);
        } else {
            Debug(LOG_LEVEL_WARNING, "field: field ?");
        }
    }
    inline void insertValue(const char* buf, size_t len){
        if (m_currentField.empty()){
            Debug(LOG_LEVEL_WARNING, "value: value ?");
            return;
        }
        m_currentValue.append(buf, len);
        m_headerMap[m_currentField]=m_currentValue;

        if(m_currentField.find("Content-Type") != string::npos){
            m_contentType = m_currentValue;
        }
        
        m_currentField.clear();
        m_currentValue.clear();
    }
    inline void setBody(const char* buf, size_t len){
        m_body.append(buf, len);
    }
    inline void clearCurrentBuf(){
        m_currentField.clear();
        m_currentValue.clear();
    }
    void printHead();
    inline string& getBody(){return m_body;}
    inline string& getContentType(){return m_contentType;}
    bool isContentTypeCared();
};


class CRequestHttpInfo : public CHttpInfo
{
private:
    string m_url;
    int m_method;
public:
    CRequestHttpInfo();
    inline void setUrl(const char* buf, size_t len){m_url.append(buf, len);}
    inline void setMethod(int m){m_method = m;}
    inline string& getUrl(){return m_url;}
    inline int getMethod(){return m_method;}
    void printRequest();

};


class CResponseHttpInfo : public CHttpInfo
{
private:
    int m_status;
    string m_descreption;
public:
    CResponseHttpInfo();
    inline void setStatus(int s){m_status = s;}
    inline void setDescreption(string& des){m_descreption = des;}
    inline int getStatus(){return m_status;}
    inline string& getDescreption(){return m_descreption;}
    void printResponse();

};



typedef vector<CRequestHttpInfo*>  RequestVector;
typedef vector<CResponseHttpInfo*> ResponseVector;


#endif
