#ifndef _CSMTP_TXN_DECODER_HPP_
#define _CSMTP_TXN_DECODER_HPP_

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <string>
#include <vector>

#include "CTcpSession.hpp"
#include "CMailInfo.hpp"

using namespace std;


typedef vector<string>     MailDataVector;
typedef vector<CMailInfo*> MailInfoVector;

class CSmtpTxnDecoder{

private:
    MailInfoVector  m_mailInfoVector;
    MailDataVector  m_mailDataVector;
    CTcpSession*    m_tcpSession;
    //string          m_buf[2];
    
public:
    CSmtpTxnDecoder(CTcpSession* sess); 
    ~CSmtpTxnDecoder();
    size_t readLine(const char* buf, size_t len);

    void decode(const string& data);
    bool decodeSmtp();
    void getMailDatas(string& buf);
    void printMailsRawData();
    void printMailsInfo();
    void saveToFile();
};

#endif
