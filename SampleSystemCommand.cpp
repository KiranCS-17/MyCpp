#include<iostream>
#include<stdio.h>
#include<string>
#include <unistd.h>
#include<algorithm>
using namespace std;
std::string run_cmd(string& cmd)
{
  FILE *in;
  char buff[512] = {'\0'};
  std::string ip_port;

  if(!(in = popen(cmd.c_str(), "r")))
  {
     return "";
  }

  while(fgets(buff, sizeof(buff), in) != NULL)
  {
       ip_port = string(buff);
  }
  pclose(in);
  return ip_port;

}

int main()
{
 string CMD = "netstat -nap  | grep 30300 | grep LISTEN | awk '{print $4}'";
 string ip_port;
 while(true)
 {
  if(true == ip_port.empty())
  {
    ip_port = run_cmd(CMD);
    cout<<"Sleeping 1 sec and do retry  "<<endl;
    usleep(1000000);
  }
  else
  {
    break;
  }
 }
 ip_port.erase(std::remove(ip_port.begin(), ip_port.end(), '\n'), ip_port.end());
 cout<<"ip_port "<<ip_port<<endl;
 string CMD_2 = "sed 's/#/"+ip_port+"/g' sample.txt > sample.txt.copy";
 cout<<CMD_2<<endl;
 run_cmd(CMD_2);
}
