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

string getYoutuberId(string youtuberName),
	   getHTTPS(string url),
	   exec(string cmd),
	   getRedirectedURL(string url),
	   replace(string, const string&, const string& = ""),
	   withSpaces(long long amount);
vector<vector<string>> getSlicesOfVideos(vector<string> videosIds, unsigned int slice);
vector<string> split(string s, string delimiter),
	           getVideosIds(string youtuberId),
			   getChannelsNames(),
			   getFileContent(string path),
			   getRealChannelsNames();
unsigned long long getCommentsNumber(string channelNameURL),
			       getCommentsNumberForVideos(vector<string> videosIds);
long long convertStrToLongLong(string str);
void print(string s);
bool contains(string subject, string find),
	 startsWith(string subject, string test);

//string KEY = "YOUR_API_KEY";
string KEY = "YOUR_API_KEY"; // got key completely used for pierre croce, is it really the case ?

int main()
{
	//string channelNameURL = "BenjaminLoison";
	//unsigned long long commentsNumber = getCommentsNumber(channelNameURL);
	
	// can' work properly because social blade doesn't reproduce correctly the case
	vector<string> channelsNames = getChannelsNames(),
		           realChannelsNames = getRealChannelsNames();
	unsigned int channelsNamesSize = channelsNames.size();
	for(unsigned int channelsNamesIndex = /*0*/67; channelsNamesIndex < channelsNamesSize; channelsNamesIndex++)
	{
		string channelName = channelsNames[channelsNamesIndex],
		       realChannelName = realChannelsNames[channelsNamesIndex];
		//print("channelName: " + channelName);
		cout << realChannelName << " ";
		unsigned long long commentsNumber = getCommentsNumber(channelName);
	}

	return 0;
}

unsigned long long getCommentsNumber(string channelNameURL)
{
	//print("channelNameURL: " + channelNameURL);
	string youtuberId = getYoutuberId(channelNameURL);
	//print(youtuberId);
	vector<string> videosIds = getVideosIds(youtuberId);
	unsigned int videosIdsSize = videosIds.size();
	vector<vector<string>> videosIdsSlices = getSlicesOfVideos(videosIds, 50);
	unsigned int videosIdsSlicesSize = videosIdsSlices.size();
	/*for(unsigned int videosIdsIndex = 0; videosIdsIndex < videosIdsSize; videosIdsIndex++)
	{
		string videoId = videosIds[videosIdsIndex];
		//print(videoId);
	}*/
	unsigned long long totalCommentsNumber = 0;
	//print("videosIdsSlicesSize: " + convertNbToStr(videosIdsSlicesSize));
	for(unsigned int videosIdsSlicesIndex = 0; videosIdsSlicesIndex < videosIdsSlicesSize; videosIdsSlicesIndex++)
	{
		vector<string> videosIdsSlice = videosIdsSlices[videosIdsSlicesIndex];
		unsigned int videosIdsSliceSize = videosIdsSlice.size();
		//print(convertNbToStr(videosIdsSliceSize));
		/*for(unsigned int videosIdsSliceIndex = 0; videosIdsSliceIndex < videosIdsSliceSize; videosIdsSliceIndex++)
		{
			string videoId = videosIdsSlice[videosIdsSliceIndex];
			//print(videoId);
		}*/
		unsigned long long commentsNumber = getCommentsNumberForVideos(videosIdsSlice);
		totalCommentsNumber += commentsNumber;
	}
	print(/*"channel: " + channelNameURL +*/ "comments number: " + /*convertNbToStr*/withSpaces(totalCommentsNumber) + " videos number: " + /*convertNbToStr*/withSpaces(videosIdsSize));
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
			 statistics = item["statistics"];//,
			 //commentCount = statistics["commentCount"];
		//cout << statistics << endl;
		//cout << commentCount << endl;
		
		string commentCountStr;// = commentCount;
		if(statistics.contains("commentCount"))
		{
			commentCountStr = statistics["commentCount"];
		}
		else
		{
			commentCountStr = "0";
		}
		unsigned long long commentsNumber = convertStrToLongLong(commentCountStr);
		//cout << statistics["commentCount"] << " " << commentCountStr << " " << commentsNumber << endl;
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

vector<string> getRealChannelsNames()
{
	vector<string> lines = getFileContent("../SocialBlade/top100fr.txt"),
                   realChannelsNames;
    string pattern = "\"/youtube/user/";
    unsigned int linesSize = lines.size();
    for(unsigned int linesIndex = 0; linesIndex < linesSize; linesIndex++)
    {
        string line = lines[linesIndex];
        if(contains(line, pattern))
        {
            string channelName = split(split(line, pattern)[1], "\"")[0],
                   realName = split(split(line, ">")[1], "<")[0];
			realChannelsNames.push_back(realName);
		}
	}
	return realChannelsNames;
}

vector<string> getChannelsNames()
{
	vector<string> channelsNames = getFileContent("top100fr.txt");
    // problèmes de majuscules, de nom qui ne sont plus utilisés, je m'y remet à la main - they are using cloudflare that's enough
	/*vector<string> lines = getFileContent("../SocialBlade/top100fr.txt"),
                   channelsNames;
    string pattern = "\"/youtube/user/";
    unsigned int linesSize = lines.size();
    for(unsigned int linesIndex = 0; linesIndex < linesSize; linesIndex++)
    {
        string line = lines[linesIndex];
        if(contains(line, pattern))
        {
            string channelName = split(split(line, pattern)[1], "\"")[0],
			       redirectedURL = getRedirectedURL("https://socialblade.com/youtube/user/" + channelName);
			//print("redirectedURL: " + redirectedURL);
			channelName = replace(redirectedURL, "https://socialblade.com/youtube/user/");
			unsigned int channelNameLength = channelName.length();
			if(channelNameLength == 22)
				channelName = "UC" + channelName;
			channelsNames.push_back(channelName);
        }
    }*/
    return channelsNames;
}

string getYoutuberId(string youtuberName)
{
	if(startsWith(youtuberName, "UC"))
	{
		return youtuberName;
	}
	string url = "https://www.youtube.com/user/" + youtuberName, // used to be /c/
		   content = getHTTPS(url),
		   youtuberId = split(split(content, "\"browseId\":\"")[1], "\"")[0];
	return youtuberId;
}

vector<string> getVideosIds(string youtuberId)
{

	string url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=" + youtuberId + "&key=" + KEY,
           content = getHTTPS(url),
		   pattern = "\"uploads\": \"";
	if(!contains(content, pattern))
	{
		vector<string> ids;
		print("no uploads: " + content + " URL: " + url);
		return ids;
	}
    string uploadsPlaylist = split(split(content, pattern)[1], "\"")[0],
           cmd = "youtube-dl -j --flat-playlist \"https://www.youtube.com/playlist?list=" + uploadsPlaylist + "\" | jq -r '.id'",
           idsStr = exec(cmd);
	vector<string> ids = split(idsStr, "\n");
	return ids;
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((string*)userp)->append((char*)contents, size *nmemb);
	return size *nmemb;
}

string getRedirectedURL(string url)
{
	//print("url: " + url);
    //curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
	if(!curl)
		print("curl error");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // was accepting string and not char* u_u
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
    
	//curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

	string got; // otherwise it prints to screen u_u
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &got);

    CURLcode curl_res = curl_easy_perform(curl);
	if(curl_res != CURLE_OK)
	{
		print("perform error");
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_res));
	}

	/*string pattern = "\"https://youtube.com/channel/";
	if(contains(got, pattern))
	{
		string channelId = split(split(got, pattern)[1], "\"")[0];
		//print("ouba");
		curl_easy_cleanup(curl);
		return channelId;
	}
	else
	{
		//print("hey: " + got);
		return url;
	}*/

	//string redirectedUrlStr;
    char* redirectedUrl = NULL;
    if(curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &redirectedUrl) != CURLE_OK)
    //if(curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &redirectedUrlStr) != CURLE_OK)
		print("getinfo error");
	//printf("redirectedUrl: %s\n", redirectedUrl);

    //curl_global_cleanup();
	//string redirectedUrlStr(redirectedUrl);
	string redirectedUrlStr = redirectedUrl;
	//print("redirectedUrlStr: " + redirectedUrlStr + " !");
	curl_easy_cleanup(curl); // it clears redirectedUrl :'(

	//exit(24);
	return redirectedUrlStr;
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
    return got;
}

string exec(string cmd)
{
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if(!pipe)
        throw runtime_error("popen() failed!");
    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    return result;
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
