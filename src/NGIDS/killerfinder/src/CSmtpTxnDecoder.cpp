#include <vmime/vmime.hpp>
#include <vmime/platforms/posix/posixHandler.hpp>
#include <fstream>
#include <sstream>

#include "CSmtpTxnDecoder.hpp"
#include "CConfig.hpp"
#include "CLog.hpp"

CSmtpTxnDecoder::CSmtpTxnDecoder(CTcpSession *sess){
    m_tcpSession = sess;
}
CSmtpTxnDecoder::~CSmtpTxnDecoder(){
    for (size_t i = 0; i < m_mailInfoVector.size(); i++){
        if (m_mailInfoVector[i] != NULL){
            delete(m_mailInfoVector[i]);
        }
    }
}


void CSmtpTxnDecoder::decode(const string& data)
{
    Debug(LOG_LEVEL_DEBUG, "smtp decode");
    // VMime initialization
    vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();
    try
    {
        vmime::messageParser msgParser(data);

        CMailInfo* mailInfo = new CMailInfo();
        mailInfo->m_mimeMap[CMailInfo::EXPEDITOR] = msgParser.getExpeditor().generate();
        mailInfo->m_mimeMap[CMailInfo::RECIPIENTS] = msgParser.getRecipients().generate();
        mailInfo->m_mimeMap[CMailInfo::COPY_RECIPIENTS] = msgParser.getCopyRecipients().generate();
        mailInfo->m_mimeMap[CMailInfo::BLIND_COPY_RECIPIENTS] = msgParser.getBlindCopyRecipients().generate();
        mailInfo->m_mimeMap[CMailInfo::SUBJECT] = msgParser.getSubject().generate();
        mailInfo->m_mimeMap[CMailInfo::DATE] = msgParser.getDate().generate();

            
        for (int i = 0 ; i < msgParser.getAttachmentCount() ; ++i)
        {
           const vmime::attachment& att = *msgParser.getAttachmentAt(i);
           CAttachment* item = new CAttachment();
           item->m_mainType    = att.getType().getType();
           item->m_subType     = att.getType().getSubType();
           item->m_name        = att.getName().generate();
           item->m_description = att.getDescription().generate();
           
           string content;
           vmime::utility::outputStreamStringAdapter ossa(content);
           att.getData()->extract(ossa);
           item->m_content = content;

           mailInfo->m_attachments.push_back(item);
        }

        // Enumerate text parts
        for (int i = 0 ; i < msgParser.getTextPartCount() ; ++i)
        {
            const vmime::textPart& part = *msgParser.getTextPartAt(i);
            mailInfo->m_text.m_mainType = part.getType().getType();
            mailInfo->m_text.m_subType  = part.getType().getSubType();

            // text/html
            if (part.getType().getSubType() == vmime::mediaTypes::TEXT_HTML)
            {
                const vmime::htmlTextPart& hp = dynamic_cast<const vmime::htmlTextPart&>(part);
                string html;
                vmime::utility::outputStreamStringAdapter ossa1(html);
                hp.getText()->extract(ossa1);
                mailInfo->m_text.m_content = html; 
                
                // Corresponding plain text is in "hp.getPlainText()"

                // Enumerate embedded objects (eg. images)
                for (int j = 0 ; j < hp.getObjectCount() ; ++j)
                {
                    const vmime::htmlTextPart::embeddedObject& obj = *hp.getObjectAt(j);

                    string object;
                    vmime::utility::outputStreamStringAdapter ossa2(object);
                    obj.getData()->extract(ossa2);
                    mailInfo->m_text.m_embeddedObjects.push_back(object);
                }
            }
            // text/plain
            else if (part.getType().getSubType()==vmime::mediaTypes::TEXT_PLAIN)
            {
                const vmime::textPart& tp = dynamic_cast<const vmime::textPart&>(part);

                string plain;
                vmime::utility::outputStreamStringAdapter ossa3(plain);
                tp.getText()->extract(ossa3);
                mailInfo->m_text.m_content = plain;
            }
        }

        m_mailInfoVector.push_back(mailInfo);
    }
    // VMime exception
    catch (vmime::exception& e)
    {
        Debug(LOG_LEVEL_ERR, "vmime::exception: " << e.what());
        throw;
    }
    // Standard exception
    catch (std::exception& e)
    {
        Debug(LOG_LEVEL_ERR, "std::exception: " << e.what());
        throw;
    }
}




bool CSmtpTxnDecoder::decodeSmtp(){
    if (!m_tcpSession){
        return false;
    }
  
    //m_tcpSession->getContinuousData(DIRECTION_TO_SERVER, m_buf[DIRECTION_TO_SERVER]);
    //m_tcpSession->getContinuousData(DIRECTION_TO_CLIENT, m_buf[DIRECTION_TO_CLIENT]);
    
    getMailDatas(m_tcpSession->getContinuousBuf(DIRECTION_TO_SERVER).getBuf());
    getMailDatas(m_tcpSession->getContinuousBuf(DIRECTION_TO_CLIENT).getBuf());

    printMailsRawData();

    for (size_t i = 0; i < m_mailDataVector.size(); i++){
        decode(m_mailDataVector[i]);
    }
    return true;
}

void CSmtpTxnDecoder::printMailsRawData(){
    for(size_t i=0; i < m_mailDataVector.size(); i++){
        Debug(LOG_LEVEL_INFO, "raw mail " << i);
        Debug(LOG_LEVEL_INFO, m_mailDataVector[i]);
    }
}

void CSmtpTxnDecoder::printMailsInfo(){
    for(size_t i=0; i < m_mailInfoVector.size(); i++){
        Debug(LOG_LEVEL_INFO, "detailed mail " << i);
        m_mailInfoVector[i]->printMailInfo();
    }
}




void CSmtpTxnDecoder::getMailDatas(string& buf){
   const char* startTag = "\r\nDATA\r\n";
   const char* endTag   = "\r\n.\r\n";
   size_t startTagLen = strlen(startTag);
   size_t endTagLen   = strlen(endTag);
   
   const char* begin = buf.c_str();
   size_t len = buf.size();
   size_t off1 = 0;
   size_t off2 = 0;
   
   while (off1 < len-startTagLen+1){
       if (0 == strncasecmp(begin+off1, startTag, startTagLen)){
           off2 = off1; 
           while (off2 < len-endTagLen+1){
               if (0 == strncasecmp(begin+off2, endTag, endTagLen)){
                   string s(begin+off1+startTagLen, off2-off1-startTagLen);
                   m_mailDataVector.push_back(s);
                   break;
               }
               off2++;
           }
           off1 = off2;
       }else{
           off1++;
       }
   }
}


void CSmtpTxnDecoder::saveToFile(){

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



    for (size_t index=0; index<m_mailInfoVector.size();index++){
        CMailInfo* mail = m_mailInfoVector[index];
        for(size_t j=0; j<mail->m_attachments.size(); j++){
            if (!mail->m_attachments[j]->isContentTypeCared()){
                Debug(LOG_LEVEL_DEBUG,"ignore content type "<<\
                    mail->m_attachments[j]->m_mainType<<"/"<<\
                    mail->m_attachments[j]->m_subType);
                continue;
            }
            ostringstream name; 
            m_tcpSession->getTcpSessionId().getIdStr(name);
            name<<"_"<<mail->m_attachments[j]->m_mainType<<"+"<<\
                mail->m_attachments[j]->m_subType<<"_"<<index<<j;
            Debug(LOG_LEVEL_DEBUG, "attachment name "<<name.str());

            ofstream file;
            path<<"/"<<name.str();
            file.open(path.str().c_str());
            file<<mail->m_attachments[j]->m_content;
            file.close();
        }
    }

}




