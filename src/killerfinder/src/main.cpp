#include "main.hpp"
#include "CMain.hpp"
#include <memory>


int main(int argc, char** argv){
  CMain* cMain = new CMain();
  return cMain->Main(argc, argv);
}



