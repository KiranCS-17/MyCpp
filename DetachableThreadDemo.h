#include<iostream>
#include<pthread.h>
#include<string>
#include<algorithm>
#include<assert.h>
#include<string.h>
#include <unistd.h>
#include <stdlib.h>
#include<stdio.h>

using namespace std;

extern "C" typedef  void* (* StartMethod)(void*);

 class pthread_smart_attr
 {
  private:
  pthread_attr_t m_attr;

  public:

   pthread_smart_attr(unsigned int thread_type)
   {
      pthread_attr_init(&m_attr);
      pthread_attr_setdetachstate(&m_attr, thread_type);
   }
   ~pthread_smart_attr()
   {
      //Free attribute
      pthread_attr_destroy(&m_attr);
   }
   pthread_attr_t* get_pthread_attr()
   {
      return &m_attr;
   }
 };

 class NotificationRegistry
 {
    public:
        static void* start_notif_registration_thread(void* data);
	static void RunCmd(string& cmd);
        void start_thread();
    private:
     pthread_t m_thread_notif_reg;
     pthread_mutex_t m_mutex;
     bool createThreadforNotifReg(StartMethod method, void* data);
  };

