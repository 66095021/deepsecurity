#ifndef _CHTTP_INFO_MAP_HPP_
#define _CHTTP_INFO_MAP_HPP_

#include "CObject.hpp"
#include "CHttpInfo.hpp"

using namespace std;

typedef map<uint64_t, CHttpInfo*> HttpInfoMap;
typedef map<uint64_t, CHttpInfo*>::iterator HttpInfoMap_it;




class CHttpInfoMap : public CObject
{
private:
    HttpInfoMap m_requestMap;
    HttpInfoMap m_responseMap;
    CHttpInfoMap();
public:
    inline HttpInfoMap& getRequestMap(){return m_requestMap;}
    inline HttpInfoMap& getResponseMap(){return m_responseMap;}
    static CHttpInfoMap* getInstance();
};


#endif

