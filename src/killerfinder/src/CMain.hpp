#ifndef _CMAIN_HPP_
#define _CMAIN_HPP_

#include "CObject.hpp"





class CMain
{
private:
    //CConfig* m_config;

public:
  CMain();
  ~CMain();
  void printHelp();
  void parserCmdLine(int argc, char** argv);
  int Main(int argc, char** argv);
};




#endif



