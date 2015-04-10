
#include "CConfig.hpp"
#include "CMailInfo.hpp"
#include "CLog.hpp"

void CAttachment::prinfInfo(){
    Debug(LOG_LEVEL_INFO, endl<<\
        "type:"<<m_mainType<<"/"<<m_subType<<endl<<\
        "name:"<<m_name<<endl<<\
        "desc:"<<m_description<<endl<<\
        "content:"<<m_content);
}

bool CAttachment::isContentTypeCared(){
    if (m_mainType.size()==0||m_subType.size()==0){
        return true;
    }
    set<string>::iterator it; 
    for(it = CConfig::getInstance("")->m_caredContentTypes.begin(); 
        it != CConfig::getInstance("")->m_caredContentTypes.end();
        ++it){
        string ct = *it;
        if (m_mainType.find(ct) != string::npos){
            return true;
        }
        if (m_subType.find(ct) != string::npos){
            return true;
        }
    }
    return false;

}





string CMailInfo::EXPEDITOR             = "Expeditor";
string CMailInfo::RECIPIENTS            = "Recipients";
string CMailInfo::BLIND_COPY_RECIPIENTS = "BlindCopyRecipients";
string CMailInfo::DATE                  = "Date";
string CMailInfo::SUBJECT               = "Subject";
string CMailInfo::COPY_RECIPIENTS       = "CopyRecipients";




CMailInfo::~CMailInfo(){
    for (size_t i = 0; i < m_attachments.size(); i++){
        if (m_attachments[i] != NULL){
            delete(m_attachments[i]);
        }
    }
}

void CMailInfo::printMailInfo(){
    Debug(LOG_LEVEL_INFO, "header:"<<endl<<\
          EXPEDITOR<<":"<<m_mimeMap[EXPEDITOR]<<endl<<\
          RECIPIENTS<<":"<<m_mimeMap[RECIPIENTS]<<endl<<\
          COPY_RECIPIENTS<<":"<<m_mimeMap[COPY_RECIPIENTS]<<endl<<\
          BLIND_COPY_RECIPIENTS<<":"<<m_mimeMap[BLIND_COPY_RECIPIENTS]<<endl<<\
          DATE<<":"<<m_mimeMap[DATE]<<endl<<\
          SUBJECT<<":"<<m_mimeMap[SUBJECT]);

    Debug(LOG_LEVEL_INFO, "body:"<<endl<<\
          "type:"<<m_text.m_mainType<<"/"<<m_text.m_subType<<endl<<\
          "content:"<<m_text.m_content);
    
    for (size_t i = 0; i < m_text.m_embeddedObjects.size(); i++){
        Debug(LOG_LEVEL_INFO, "embeded " << i << " :" << endl<<\
              m_text.m_embeddedObjects[i]);
    }

    for (size_t i = 0; i < m_attachments.size(); i++){
        m_attachments[i]->prinfInfo();
    }
}


 

