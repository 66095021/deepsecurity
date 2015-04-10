
#include "CLock.hpp"

CLock::CLock(){
  m_mutex = PTHREAD_MUTEX_INITIALIZER;
}

CLock::~CLock(){

}

