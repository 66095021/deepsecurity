

#include "CHttpInfoMap.hpp"


CHttpInfoMap::CHttpInfoMap(){

}

CHttpInfoMap* CHttpInfoMap::getInstance(){
    static CHttpInfoMap* instance = NULL;
    if (instance == NULL){
        instance = new CHttpInfoMap();
    }
    return instance;
}


