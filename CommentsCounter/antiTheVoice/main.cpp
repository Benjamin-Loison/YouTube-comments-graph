#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
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

string getHTTPS(string url),
	   replace(string, const string&, const string& = ""),
	   withSpaces(long long amount);
vector<vector<string>> getSlicesOfVideos(vector<string> videosIds, unsigned int slice);
vector<string> split(string s, string delimiter),
	           getVideosIds(string youtuberId);
unsigned long long getCommentsNumber(string channelNameURL),
			       getCommentsNumberForVideos(vector<string> videosIds);
long long convertStrToLongLong(string str);
void print(string s);
bool contains(string subject, string find),
	 startsWith(string subject, string test);

string KEY = "YOUR_API_KEY";
unsigned long long costConsumed = 0;

int main()
{
	string youtuberId = "UCXptamDYEVcU4JCio30hYTw";//"UCQRELbX0H5FCokIFxOAsHFA";
	unsigned long long commentsNumber = getCommentsNumber(youtuberId);

	return 0;
}

unsigned long long getCommentsNumber(string youtuberId)
{
	vector<string> videosIds = getVideosIds(youtuberId);
	unsigned int videosIdsSize = videosIds.size();
	vector<vector<string>> videosIdsSlices = getSlicesOfVideos(videosIds, 50);
	unsigned int videosIdsSlicesSize = videosIdsSlices.size();
	unsigned long long totalCommentsNumber = 0;
	for(unsigned int videosIdsSlicesIndex = 0; videosIdsSlicesIndex < videosIdsSlicesSize; videosIdsSlicesIndex++)
	{
		vector<string> videosIdsSlice = videosIdsSlices[videosIdsSlicesIndex];
		unsigned long long commentsNumber = getCommentsNumberForVideos(videosIdsSlice);
		totalCommentsNumber += commentsNumber;
	}
	print("comments number: " + withSpaces(totalCommentsNumber) + " videos number: " + withSpaces(videosIdsSize));
	return totalCommentsNumber;
}

unsigned long long getCommentsNumberForVideos(vector<string> videosIdsSlice)
{
	string videosIdsStr = "";
	unsigned int videosIdsSliceSize = videosIdsSlice.size();
	for(unsigned int videosIdsSliceIndex = 0; videosIdsSliceIndex < videosIdsSliceSize; videosIdsSliceIndex++)
	{
		string videoId = videosIdsSlice[videosIdsSliceIndex];
		videosIdsStr += videoId;
		if(videosIdsSliceIndex < videosIdsSliceSize - 1)
		{
			videosIdsStr += ",";
		}
	}
	string url = "https://youtube.googleapis.com/youtube/v3/videos?part=statistics&id=" + videosIdsStr + "&maxResults=50&key=" + KEY,
		   content = getHTTPS(url);
	json data = json::parse(content),
		 items = data["items"];
	unsigned long long totalCommentsNumber = 0;
	for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
	{
		json item = *itemsIt,
			 statistics = item["statistics"];
		
		string commentCountStr = statistics.contains("commentCount") ? statistics["commentCount"] : "0";
		unsigned long long commentsNumber = convertStrToLongLong(commentCountStr);
		totalCommentsNumber += commentsNumber;
	}
	return totalCommentsNumber;
}

vector<vector<string>> getSlicesOfVideos(vector<string> videosIds, unsigned int slice)
{
	vector<vector<string>> videosIdsSlices;
	vector<string> videosIdsSlice;
	unsigned int videosIdsSize = videosIds.size();
	for(unsigned int videosIdsIndex = 0; videosIdsIndex < videosIdsSize; videosIdsIndex++)
	{
		string videosId = videosIds[videosIdsIndex];
		videosIdsSlice.push_back(videosId);
		if(videosIdsSlice.size() == 50)
		{
			videosIdsSlices.push_back(videosIdsSlice);
			videosIdsSlice.clear();
		}
	}
	if(videosIdsSlice.size() > 0)
		videosIdsSlices.push_back(videosIdsSlice);
	return videosIdsSlices;
}

string scrap(string uploadsPlaylist, string nextPageToken, vector<string>* ids)
{
	string url = "https://www.googleapis.com/youtube/v3/playlistItems?part=contentDetails&maxResults=50&playlistId=" + uploadsPlaylist + "&key=" + KEY;
	//print(url);
	if(nextPageToken != "")
		url += "&pageToken=" + nextPageToken;
	string content = getHTTPS(url);
	json data = json::parse(content);
	string originalPageToken = nextPageToken;
	if(data.contains("nextPageToken"))
		nextPageToken = data["nextPageToken"];
	json items = data["items"];
	for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
	{
		json item = *itemsIt,
			 contentDetails = item["contentDetails"];
		string videoId = contentDetails["videoId"];
		ids->push_back(videoId);
	}
	return nextPageToken;
}

vector<string> getVideosIds(string youtuberId)
{
	string url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=" + youtuberId + "&key=" + KEY,
           content = getHTTPS(url),
		   pattern = "\"uploads\": \"";
	//print("url: " + url);
	if(!contains(content, pattern))
	{
		vector<string> ids;
		print("no uploads: " + content + " URL: " + url);
		return ids;
	}
    string uploadsPlaylist = split(split(content, pattern)[1], "\"")[0];
	vector<string> ids;
	string originalPageToken = "ignition",
		   nextPageToken = "";
	while(nextPageToken != originalPageToken)
	{
		originalPageToken = nextPageToken;
		nextPageToken = scrap(uploadsPlaylist, nextPageToken, &ids);
	}
	print("got " + convertNbToStr(ids.size()) + " videos !");
	return ids;
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((string*)userp)->append((char*)contents, size *nmemb);
	return size *nmemb;
}

long long convertStrToLongLong(string str)
{
    long long res = stoll(str.c_str());
    return res;
}

void print(string s)
{
	cout << s << endl;
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

string getHTTPS(string url)
{
    string originURL = url;
    CURL* curl = curl_easy_init();
    string got;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &got);
    curl_easy_perform(curl);
    long responseCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    curl_easy_cleanup(curl);
    if(responseCode != 200)
    {
		print("key completely used ! url: " + url);
		exit(42);
    }
	costConsumed++;
    return got;
}

bool contains(string subject, string find)
{
    return subject.find(find) != string::npos;
}

bool startsWith(string subject, string test)
{
    return !subject.compare(0, test.size(), test);
}

string replace(string subject, const string& search, const string& replace)
{
    unsigned int s = subject.find(search);
    if(s > subject.length())
        return subject;
    return subject.replace(s, search.length(), replace);
}

string withSpaces(string amount)
{
    unsigned short amountLength = amount.length();
    if(amountLength < 4)
        return amount;
    string reminder = amount.substr(amountLength - 3, 3);
    return withSpaces(amount.substr(0, amountLength - 3)) + " " + reminder;
}

string withSpaces(long long amount)
{
    return withSpaces(convertNbToStr(amount));
}
