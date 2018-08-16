#include "SimpleSHM.h"
#include <string>
#include <iostream>

int main()
{

  char buff[100]={'\0'};
  SimpleSHM::getInstance().read(buff,100);
  std::cout<<std::string(buff);

}