#ifndef _CSESSION_MAP_HPP_
#define _CSESSION_MAP_HPP_

class CSessionMap : public CObject
{
private:
  CTcpSession* m_data[1024];
  int m_depth;
  int m_length;

public:
  CSessionMap::CSessionMap(){
    for(int i = 0; i < 1024; i++){
      m_data[i] = NULL;
    }
  }

  
  
};

#endif
