#include "SimpleSHM.h"
#include <string>
#include <iostream>

int main()
{
 std::string shared_data="LDAP_CONN_DOWN";
 SimpleSHM::getInstance().write(shared_data.c_str());
}