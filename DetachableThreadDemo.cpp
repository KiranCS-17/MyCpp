#include "DetachableThreadDemo.h"

extern "C" void* startNotifRegThreadRoutine(void* data)
{
    return NotificationRegistry::start_notif_registration_thread(data);
}


void* NotificationRegistry::start_notif_registration_thread(void* data)
{
   cout<<"Inside thread function"<<(*(char*)(data))<<endl;
   /* Add all Your Logic Here*
      Below Example runs a system command and parses the output.
   */
   NotificationRegistry::RunCmd("ls");
}

bool NotificationRegistry::createThreadforNotifReg(StartMethod method, void* data)
{
    bool return_code = true;
    assert(method);
    pthread_smart_attr smart_attr(PTHREAD_CREATE_DETACHED);
    int result = pthread_create(&m_thread_notif_reg, smart_attr.get_pthread_attr(), method, (void*) data);
    if (result)
    {
        cout<<" createThreadforNsrCache failed and return code from pthread_create() = " << result;
        return_code = false;
    }

    return return_code;
}
void NotificationRegistry::RunCmd(string& cmd) {

    FILE *in;
    char buff[512];

    if(!(in = popen(cmd.c_str(), "r"))){
        return;
    }

    while(fgets(buff, sizeof(buff), in) != NULL)
    {
        cout << buff;
    }
    pclose(in);

 }
void NotificationRegistry::start_thread()
{

    char* l_ptr = new char[100];
    l_ptr[100]= {'\0'};
    strcpy(l_ptr,"---D-E-T-A-C-H-A-B-L-E T-H-R-E-A-D---");
    bool return_code = createThreadforNotifReg(startNotifRegThreadRoutine, l_ptr);
    delete l_ptr;
}

int main()
{
  NotificationRegistry nr;
  nr.start_thread();
  while(true)
  {
	  usleep(5);
  }/*Assumption Main thread Won't Stop in Real Time untill forcefull shutdown is performed
     Mainthread lifetime is more than worker thread duration.
    */
}
