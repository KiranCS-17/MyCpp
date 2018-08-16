#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
class SimpleSHM
{

  private:
  	key_t key;
  	int shmid;
  	pthread_mutex_t m;

  	SimpleSHM()
  	{
  	   key = ftok("shmfile",65);
  	   shmid = shmget(key,1024,0666|IPC_CREAT);

       pthread_mutexattr_t mattr;
       pthread_mutexattr_init(&mattr);
       pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK_NP);
       pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
       pthread_mutex_init(&m, &mattr);
  	}

  public:
   static SimpleSHM& getInstance()
   {
	   static SimpleSHM instance;
	   return instance;
   }


void write(const char* dataIn)
{
  pthread_mutex_lock(&m);
  char *str = (char*) shmat(shmid,(void*)0,0);
  memcpy(str,dataIn,strlen(dataIn));
  shmdt(str);
  pthread_mutex_unlock(&m);

}

void read(char* dataOut,unsigned len)
{
   pthread_mutex_lock(&m);
   char *str = (char*) shmat(shmid,(void*)0,0);
   memcpy(dataOut,str,len);
   shmdt(str);
   pthread_mutex_lock(&m);
}

void remove()
{
   shmctl(shmid,IPC_RMID,NULL);
}

};




