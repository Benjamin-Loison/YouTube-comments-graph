#include <stdio.h>
#include <pthread.h>
#include <curl/curl.h>
#include <gcrypt.h>
#include <errno.h>

#define NUMT 4
 
static pthread_mutex_t* lockarray;
 
GCRY_THREAD_OPTION_PTHREAD_IMPL;
 
void init_locks(void)
{
	gcry_control(GCRYCTL_SET_THREAD_CBS); // maybe required second argument cf https://www.gnupg.org/documentation/manuals/gcrypt-devel/Multi_002dThreading.html
}

// maybe need gcry_check_version
 
#define kill_locks()

const char* const urls[]=
{
  	"https://www.example.com/",
  	"https://www.example.com/",
  	"https://www.example.com/",
  	"https://www.example.com/",
};
 
static void* pull_one_url(void* url)
{
	CURL* curl = curl_easy_init();
  	curl_easy_setopt(curl, CURLOPT_URL, url);
  	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  	curl_easy_perform(curl); /* ignores error */
  	curl_easy_cleanup(curl);
 
  	return NULL;
}
 
int main()
{
  	pthread_t tid[NUMT];
 
  	curl_global_init(CURL_GLOBAL_ALL);
 
  	init_locks();
 
  	for(unsigned int i = 0; i < NUMT; i++)
  	{
    	int error = pthread_create(&tid[i], NULL, pull_one_url, (void*)urls[i]);
    	if(0 != error)
      		fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
    	else
      		fprintf(stderr, "Thread %d, gets %s\n", i, urls[i]);
  	}
 
  	for(unsigned int i = 0; i < NUMT; i++)
	{
    	pthread_join(tid[i], NULL);
    	fprintf(stderr, "Thread %d terminated\n", i);
  	}
 
  	kill_locks();
 
  	return 0;
}
