#include <iostream>
#include <sstream>
#include <thread>
#include <curl/curl.h>
using namespace std;

template<typename T>
string convertNbToStr(const T& number)
{
    ostringstream convert;
    convert << number;
    return convert.str();
}

#define THREADS_NB 100

void work(),
	 print(string s);
string getHTTPS(string url);
bool contains(string subject, string find);

string KEY = "YOUR_API_KEY",
	   //url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=UCzYC9ss2P77Ry2LzIDL5Xsw&key=" + KEY;
	   url = "https://youtube.googleapis.com/youtube/v3/search?key=" + KEY + "&maxResults=1";
unsigned int costConsumed = 0,
			 costPrice = 100/*1*/;

int main()
{
	thread threads[THREADS_NB];
	for(unsigned short threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++)
	{
		threads[threadsIndex] = thread(work);
	}
	for(unsigned short threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++)
	{
		threads[threadsIndex].join();
	}
	print("costConsumed: " + convertNbToStr(costConsumed));
	return 0;
}

void work()
{
	while(true)
	{
		string content = getHTTPS(url);
		if(contains(content, /*"UUzYC9ss2P77Ry2LzIDL5Xsw"*/"youtube#searchResult"))
		{
			//costConsumed++;
			costConsumed += costPrice;
			print("current costConsumed: " + convertNbToStr(costConsumed));
		}
		else
		{
			break;
		}
	}
}

bool contains(string subject, string find)
{
    return subject.find(find) != string::npos;
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size *nmemb);
    return size *nmemb;
}

string getHTTPS(string url)
{
	CURL* curl = curl_easy_init();
    string got;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &got);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
	return got;
}

void print(string s)
{
	cout << s << endl;
}
