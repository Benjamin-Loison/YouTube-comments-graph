#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <curl/curl.h>
#include <json.hpp>
using namespace std;
using json = nlohmann::json;

template<typename T>
string convertNbToStr(const T& number)
{
    ostringstream convert;
    convert << number;
    return convert.str();
}

vector<vector<string>> getSlices(vector<string> channelsIds, unsigned int slice);
vector<string> getFileContent(string path),
	           split(string s, string delimiter),
			   areFrench(vector<string> channelsIds);
void print(string s);
bool isFrench(string youtuberId),
	 contains(string subject, string find),
	 writeFile(string filePath, string option, string toWrite);
string getHTTPS(string url),
	   replace(string, const string&, const string& = ""),
	   toString(vector<string>),
	   scrap(string nextPageToken = "", unsigned int callsIndex = 0); // is there a difference if put different default arguments and call the function above the real implementation and below (if implementation also have default arguments) ? doesn't seem to accept both default arguments at the same time ^^

vector<string> KEYS,
	           channels;
unsigned int keyIndex = 0,
			 costConsumed = 0;
string KEY = "KEY",
	   REAL_KEY;

int main()
{
	KEYS = getFileContent("../CPP/keys.txt");
	REAL_KEY = KEYS[0];

    string originalPageToken = "ignition",
           nextPageToken = "";
    unsigned int callsIndex = 0;
    while(nextPageToken != originalPageToken)
    {
        originalPageToken = nextPageToken;
        nextPageToken = scrap(nextPageToken, callsIndex);
        callsIndex++;
    }
	print("gathered " + convertNbToStr(channels.size()) + " channels"); // ~ 115 676
	string toWrite = toString(channels);
    writeFile(/*"channels.txt"*/"videos.txt", "w", toWrite);
	//vector<string> frenchChannels = areFrench(channels);
	//print(convertNbToStr(frenchChannels.size()) + " french !");
	//toWrite = toString(frenchChannels);
	//writeFile("channelsFR.txt", "w", toWrite);

	return 0;
}

string scrap(string nextPageToken, unsigned int callsIndex)
{
	print("callsIndex: " + convertNbToStr(callsIndex) + " " + convertNbToStr(channels.size()) + " " + convertNbToStr(costConsumed));
	//string url = "https://youtube.googleapis.com/youtube/v3/search?maxResults=50&type=channel&order=videoCount&key=" + KEY; // &relevanceLanguage=fr gives me around 600 results why expect 115 769 according to him u_u - so let's grab everything and filter what is french I should hav enough keys (expecting: 114 196) let's also remove &order=viewCount
	//string url = "https://youtube.googleapis.com/youtube/v3/search?maxResults=50&type=video&order=viewCount&relevanceLanguage=fr&key=" + KEY; // not even any nextPageToken u_u - if remove "video" type searched it is back u_u
	//viewCount force video type
	string url = "https://youtube.googleapis.com/youtube/v3/search?key=" + KEY; // with any combination we don't get more than 600 results u_u - even without maxResults modified...
	if(nextPageToken != "")
		url += "&pageToken=" + nextPageToken;
	string content = getHTTPS(url);
	json data = json::parse(content);
	string originalPageToken = nextPageToken;
	if(data.contains("nextPageToken"))
	{
		nextPageToken = data["nextPageToken"];
		print("nextPageToken: " + nextPageToken);
	}
	else
	{
		print("no next page token found !");
	}
	json items = data["items"];
	unsigned int totalResults = data["pageInfo"]["totalResults"];
	print("totalResults: " + convertNbToStr(totalResults));
	for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
	{
		json item = *itemsIt,
			 id = item["id"];
		string channelId;
		if(id.contains("videoId"))
			 channelId = id["videoId"];//id["channelId"];
		else
			channelId = id["channelId"];
		channels.push_back(channelId);
		// if check here for isFrench it's going to be long due to ping, let's vectorize this at the end
	}
	return nextPageToken;
}

vector<string> areFrench(vector<string> channelsIds) // should check multiple at once
{
	vector<string> res;
	vector<vector<string>> channelsIdsSlices = getSlices(channelsIds, 50);
	unsigned int channelsIdsSlicesSize = channelsIdsSlices.size();
	for(unsigned int channelsIdsSlicesIndex = 0; channelsIdsSlicesIndex < channelsIdsSlicesSize; channelsIdsSlicesIndex++)
	{
		vector<string> channelsIdsSlice = channelsIdsSlices[channelsIdsSlicesIndex];
		unsigned int channelsIdsSliceSize = channelsIdsSlice.size();
		string channelsIdsSliceStr = "";
		for(unsigned int channelsIdsSliceIndex = 0; channelsIdsSliceIndex < channelsIdsSliceSize; channelsIdsSliceIndex++)
		{
			string channelId = channelsIdsSlice[channelsIdsSliceIndex];
			channelsIdsSliceStr += channelId;
			if(channelsIdsSliceIndex < channelsIdsSliceSize - 1)
				channelsIdsSliceStr += ",";
		}
		string content = getHTTPS("https://youtube.googleapis.com/youtube/v3/channels?part=snippet&id=" + channelsIdsSliceStr + "&key=" + KEY);
		json data = json::parse(content),
             items = data["items"];
		for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
		{
			json item = *itemsIt,
				 snippet = item["snippet"];
			string id = item["id"];
			bool countrySet = snippet.contains("country");
			if(countrySet)
			{
				string country = snippet["country"];
				if(country == "FR")
				{
					res.push_back(id);
					print("french channel <3");
				}
				else
				{
					print("not considering because country: " + country);
				}
			}
			else
			{
				res.push_back(id);
				print("no country info but got it my asking preference fr content so let's add it");
			}
		}
	}
	return res;
}

void changeKey()
{
	if(keyIndex >= KEYS.size())
	{
		print("all keys exhausted");
		exit(42);
	}
	print("key " + convertNbToStr(keyIndex) + " consumed, increasing !");
	keyIndex++;
	REAL_KEY = KEYS[keyIndex];
}

vector<vector<string>> getSlices(vector<string> channelsIds, unsigned int slice)
{
	vector<vector<string>> channelsIdsSlices;
	vector<string> channelsIdsSlice;
	unsigned int channelsIdsSize = channelsIds.size();
	for(unsigned int channelsIdsIndex = 0; channelsIdsIndex < channelsIdsSize; channelsIdsIndex++)
	{
		string videosId = channelsIds[channelsIdsIndex];
		channelsIdsSlice.push_back(videosId);
		if(channelsIdsSlice.size() == 50)
		{
			channelsIdsSlices.push_back(channelsIdsSlice);
			channelsIdsSlice.clear();
		}
	}
	if(channelsIdsSlice.size() > 0)
		channelsIdsSlices.push_back(channelsIdsSlice);
	return channelsIdsSlices;
}

string toString(vector<string> vec)
{
	string res = "";
	unsigned int vecSize = vec.size();
	for(unsigned int vecIndex = 0; vecIndex < vecSize; vecIndex++)
	{
		string vecEl = vec[vecIndex];
		res += vecEl;
		if(vecIndex < vecSize)
			res += "\n";
	}
	return res;
}

unsigned long long getMillis()
{
    struct timeval tp;
    if(gettimeofday(&tp, NULL) == -1)
	{
		print("gettimeofday error");
	}
    return (long long)tp.tv_sec * 1000L + tp.tv_usec / 1000;
}

string getNbZero(double number, unsigned short numberOfDigits = 2, bool atTheEnd = false)
{
    string strNb = convertNbToStr(number);
	unsigned int strNbLength = strNb.length();
    for(unsigned short digit = strNbLength; digit < numberOfDigits; digit++)
    {
        strNb = atTheEnd ? strNb + "0" : "0" + strNb;
    }
    return strNb;
}

string getDate()
{
    time_t t = time(0);
    if(t == (time_t)(-1))
		print("time error");
	struct tm *now = localtime(&t);

    unsigned long long ms = getMillis();
    
	return getNbZero(now->tm_mday) + "-" + getNbZero(now->tm_mon + 1) + "-" + convertNbToStr<int>(now->tm_year - 100) + "#" + getNbZero(now->tm_hour) + "-" + getNbZero(now->tm_min) + "-" + getNbZero(now->tm_sec) + "-" + getNbZero(ms % 1000, 3);
}

bool writeFile(string filePath, string option, string toWrite)
{
    FILE* file = fopen(filePath.c_str(), option.c_str());
    if(file != NULL)
    {
        if(fputs(toWrite.c_str(), file) == EOF)
		{
			print("fputs error");
		}
        if(fclose(file) != 0)
		{
			print("fclose error");
		}
        return true;
    }
	else
	{
		print("fopen error");
	}
    return false;
}

vector<string> split(string s, string delimiter)
{
    vector<string> toReturn;
    size_t pos = 0;
    while((pos = s.find(delimiter)) != string::npos)
    {
        toReturn.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    toReturn.push_back(s);
    return toReturn;
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
	string originURL = url;
	url = replace(url, KEY, REAL_KEY);
    //print("url: " + url + " !");
	CURL* curl = curl_easy_init();
    string got;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &got);
	print("before");
    curl_easy_perform(curl); // 304 ms while other stuff at start take 18 ms
	print("after");
    long responseCode;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	curl_easy_cleanup(curl);
	if(responseCode != 200)
	{
		changeKey();
		return getHTTPS(originURL);
	}
	costConsumed++;
    return got;
}

string replace(string subject, const string& search, const string& replace)
{
    unsigned int s = subject.find(search);
    if(s > subject.length())
        return subject;
    return subject.replace(s, search.length(), replace);
}

vector<string> getFileContent(string path)
{
	vector<string> vec;
	ifstream infile(path.c_str());
    string line;
    while(getline(infile, line))
    	vec.push_back(line);
    return vec;
}

void print(string s)
{
	cout << getDate() << ": " << s << endl;
}
