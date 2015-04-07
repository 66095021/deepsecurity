#include <iostream>
#include <string> 
#include <sstream>        // std::ostringstream
#include <libxml/parser.h> 
#include "CConfig.hpp"



#define IF_INT_NODE(NAME, TYPE, VALUE)\
        if(!xmlStrcmp(curNode->name,(const xmlChar*)NAME)){\
            szKey = xmlNodeGetContent(curNode); \
            ostringstream oss;\
            oss << szKey;\
            istringstream iss(oss.str());\
            TYPE item = 0;\
            iss >> item;\
            VALUE=item;\
            xmlFree(szKey);\
        }

#define IF_STRING_NODE(NAME, VALUE) \
        if(!xmlStrcmp(curNode->name,(const xmlChar*)NAME)){\
            szKey = xmlNodeGetContent(curNode); \
            ostringstream oss;\
            oss << szKey;\
            VALUE = oss.str();\
            xmlFree(szKey);\
        }
        
#define IF_INT_SET_NODE(NAME, TYPE, VALUE) \
            if(!xmlStrcmp(curNode->name,(const xmlChar*)NAME)){\
                szKey = xmlNodeGetContent(curNode);\
                ostringstream oss;\
                oss << szKey;\
                string tmp = oss.str();\
                size_t index = 0;\
                while ((index = tmp.find(";")) != string::npos){\
                    istringstream iss(tmp.substr(0,index));\
                    TYPE item = 0;\
                    iss >> item;\
                    VALUE.insert(item);\
                    tmp=tmp.substr(index+1);\
                }\
                if (tmp.length()>0){\
                    istringstream iss(tmp);\
                    TYPE item = 0;\
                    iss >> item;\
                    VALUE.insert(item);\
                }\
                xmlFree(szKey);\
        }

#define IF_STRING_SET_NODE(NAME, VALUE) \
            if(!xmlStrcmp(curNode->name,(const xmlChar*)NAME)){\
                szKey = xmlNodeGetContent(curNode);\
                ostringstream oss;\
                oss << szKey;\
                string tmp = oss.str();\
                size_t index = 0;\
                while ((index = tmp.find(";")) != string::npos){\
                    string item = tmp.substr(0,index);\
                    VALUE.insert(item);\
                    tmp=tmp.substr(index+1);\
                }\
                if (tmp.length()>0){\
                    VALUE.insert(tmp);\
                }\
                xmlFree(szKey);\
        }





#define PRINT_PARA(para) cout<<#para<<"\t"<<para<<endl;

#define PRINT_SET_PARA(type, para) \
{\
    ostringstream oss;\
    set<type>::iterator it;\
    for(it=para.begin();it!=para.end();++it){\
      oss<<*it<<";";\
    }\
    cout<<#para<<"\t"<<oss.str()<<endl;\
}
            
void CConfig::setDefault(){
    this->m_mode = "online";
    this->m_offlineFile = "./killer-finder.pcap";
    this->m_dev = "eth0";
    this->m_filter = "tcp";
    this->m_httpPorts.insert(80);
    this->m_smtpPorts.insert(25);
    this->m_packetNum = 20;
    this->m_poolLen = 1024;
    this->m_caredHttpMethods.insert(1);//GET
    this->m_caredHttpMethods.insert(3);//POST
    this->m_caredHttpMethods.insert(4);//PUT
    this->m_caredContentTypes.insert("application");
    this->m_pcapTimeout = 1;
    this->m_maxDb = 10000;
    this->m_logLevel = 6;
    this->m_rootPath = "/usr/local/killer";
}

void CConfig::printMe(){
    PRINT_PARA(m_mode);
    PRINT_PARA(m_offlineFile);
    PRINT_PARA(m_dev);
    PRINT_PARA(m_filter);
    PRINT_SET_PARA(uint16_t, m_httpPorts);
    PRINT_SET_PARA(uint16_t, m_smtpPorts);
    PRINT_PARA(m_packetNum);
    PRINT_PARA(m_poolLen);
    PRINT_SET_PARA(int, m_caredHttpMethods);
    PRINT_SET_PARA(string, m_caredContentTypes);
    PRINT_PARA(m_pcapTimeout);
    PRINT_PARA(m_maxDb);
    PRINT_PARA(m_logLevel);
    PRINT_PARA(m_rootPath);
}


void CConfig::loadFromFile(const string& configFileName){
    
    const char* fileName = configFileName.size() != 0 ? 
                    configFileName.c_str():"/usr/local/etc/config.xml";
    xmlDocPtr  doc; 
    xmlNodePtr curNode; 
    xmlChar*   szKey; 
                                                                       
    doc = xmlParseFile(fileName); 
    if(doc == NULL){ 
        cout<<"Document not parsed successfully!\n"<<endl; 
        return; 
    } 

    curNode = xmlDocGetRootElement(doc); 
    if(curNode == NULL){ 
        cout<<"empty document.\n"<<endl; 
        xmlFreeDoc(doc); 
        return; 
    } 

    if(xmlStrcmp(curNode->name,BAD_CAST"root")){ 
        cout<<"root node is not 'root'\n"<<endl;   
        xmlFreeDoc(doc); 
        return; 
    }

    curNode = curNode->xmlChildrenNode; 

    
    //xmlNodePtr propNodePtr = curNode; 
    while(NULL != curNode){ 
        IF_STRING_NODE("mode", m_mode)
        else
        IF_STRING_NODE("offlineFile", m_offlineFile)
        else
        IF_INT_NODE("packetNum",int,m_packetNum)
        else
        IF_STRING_NODE("dev", m_dev)
        else
        IF_STRING_NODE("filter", m_filter)
        else
        IF_INT_SET_NODE("httpPorts", uint16_t, m_httpPorts)
        else
        IF_INT_NODE("poolLen",int,m_poolLen)
        else
        IF_INT_SET_NODE("caredHttpMethods", int, m_caredHttpMethods)
        else
        IF_STRING_SET_NODE("caredContentTypes", m_caredContentTypes)
        else
        IF_INT_NODE("pcapTimeout",int,m_pcapTimeout)
        else
        IF_INT_NODE("maxDb",int,m_maxDb)
        else
        IF_INT_NODE("logLevel",int,m_logLevel)
        else
        IF_STRING_NODE("rootPath", m_rootPath)
        
        curNode= curNode->next; 
    } 

}
/*
CConfig::loadFromCmdLine(int argc, char** argv){

}*/

CConfig::CConfig(const string& configFileName){
    setDefault();
    loadFromFile(configFileName); 
    printMe();
}



CConfig::~CConfig(){

}

CConfig* CConfig::getInstance(const string& configFileName){
    static CConfig* instance = NULL;
    if (instance == NULL){
        instance = new CConfig(configFileName);
    }
    return instance;

}

