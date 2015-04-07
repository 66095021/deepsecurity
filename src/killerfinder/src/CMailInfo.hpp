#ifndef _CMAIL_INFO_HPP_
#define _CMAIL_INFO_HPP_

#include <map>
#include <string>
#include <vector>

using namespace std;

class CMailText{
public:
    string         m_mainType;
    string         m_subType;
    string         m_content;
    vector<string> m_embeddedObjects;
};

class CAttachment{
public:
    string   m_mainType;
    string   m_subType;
    string   m_name;
    string   m_description;

    string   m_content;
    void prinfInfo();
    bool isContentTypeCared();
};


class CMailInfo{
public:
    static string EXPEDITOR;
    static string RECIPIENTS;
    static string DATE;
    static string BLIND_COPY_RECIPIENTS;
    static string SUBJECT;
    static string COPY_RECIPIENTS;
        
    map<string, string>      m_mimeMap;
    CMailText                m_text;
    vector<CAttachment*>     m_attachments;
    ~CMailInfo();
    void printMailInfo();
};

#endif
