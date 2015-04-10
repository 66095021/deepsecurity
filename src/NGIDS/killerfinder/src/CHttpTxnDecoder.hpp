#ifndef _CHTTP_TXN_DECODER_HPP_
#define _CHTTP_TXN_DECODER_HPP_

#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include "CTcpSession.hpp"
#include <http_parser.h>
#include "CLog.hpp"
#include "CHttpInfo.hpp"

/*
XX(0,  DELETE,      DELETE)       \
XX(1,  GET,         GET)          \
XX(2,  HEAD,        HEAD)         \
XX(3,  POST,        POST)         \
XX(4,  PUT,         PUT)          \

XX(5,  CONNECT,     CONNECT)      \
XX(6,  OPTIONS,     OPTIONS)      \
XX(7,  TRACE,       TRACE)        \

XX(8,  COPY,        COPY)         \
XX(9,  LOCK,        LOCK)         \
XX(10, MKCOL,       MKCOL)        \
XX(11, MOVE,        MOVE)         \
XX(12, PROPFIND,    PROPFIND)     \
XX(13, PROPPATCH,   PROPPATCH)    \
XX(14, SEARCH,      SEARCH)       \
XX(15, UNLOCK,      UNLOCK)       \

XX(16, REPORT,      REPORT)       \
XX(17, MKACTIVITY,  MKACTIVITY)   \
XX(18, CHECKOUT,    CHECKOUT)     \
XX(19, MERGE,       MERGE)        \

XX(20, MSEARCH,     M-SEARCH)     \
XX(21, NOTIFY,      NOTIFY)       \
XX(22, SUBSCRIBE,   SUBSCRIBE)    \
XX(23, UNSUBSCRIBE, UNSUBSCRIBE)  \

XX(24, PATCH,       PATCH)        \
XX(25, PURGE,       PURGE)        \

XX(26, MKCALENDAR,  MKCALENDAR)   \

*/

using namespace std;



class CHttpTxnDecoder{
private:
    
    static http_parser_settings settings;
    enum http_parser_type m_parserType;
    http_parser*          m_parser;
    CTcpSession*          m_tcpSession;

public:
    RequestVector         m_requestVector;
    ResponseVector        m_responseVector;
public:
    // begin
    // these members used for active mode.
    // for inactive mode, url is save in vector 
    string  m_url;
    string  m_hostField;
    string  m_hostValue;
    int     m_activeMethod;
    // end

public:
    
    CHttpTxnDecoder(CTcpSession* sess);
    CHttpTxnDecoder();
    ~CHttpTxnDecoder();

    inline enum http_parser_type getParserType(){return m_parserType;}
    inline RequestVector&  getRequestVector() {return m_requestVector;}
    inline ResponseVector& getResponseVector(){return m_responseVector;}
    inline http_parser*    getParser(){return m_parser;}

    static int messageBeginCb(http_parser* p);
    static int requestUrlCb(http_parser* p, const char*buf, size_t len);
    static int responseStatusCb(http_parser* p, const char*buf, size_t len);
    static int headerFieldCb(http_parser* p, const char*buf, size_t len);
    static int headerValueCb(http_parser* p, const char*buf, size_t len);
    static int headersCompleteCb(http_parser* p);
    static int body_cb(http_parser* p, const char*buf, size_t len);
    static int messageCompleteCb(http_parser* p);

    //void parserInit(void* data, enum http_parser_type type);
    bool parserExecute(const char* buf, size_t len);  
    //bool decodeHttp(CTcpSession* sess, enum http_parser_type type);
    //bool decodeHttpInactive(CTcpSession* sess, enum http_parser_type type);
    //bool decodeHttpActive(CTcpSession* sess, enum http_parser_type type);
    
    void parserInit(enum http_parser_type type);
    bool decodeHttp(enum http_parser_type type);
    bool decodeHttpInactive(enum http_parser_type type);
    bool decodeHttpActive(enum http_parser_type type);
    bool decodeHttp();
    void printHttp();
    void saveToFile();

};


#endif


