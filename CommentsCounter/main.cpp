#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <curl/curl.h>
#include <json.hpp>
using namespace std;
using json = nlohmann::json;

#define THREADS_NB 24

// two multithreading possibilities: within a channel if many videos or accross channels

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
	   withSpaces(long long amount),
	   toString(vector<string> vec);
vector<vector<string>> getSlicesOfVideos(vector<string> videosIds, unsigned int slice);
vector<string> split(string s, string delimiter),
	           getVideosIds(string youtuberId),
			   getChannelsNames(),
			   getFileContent(string path),
			   getRealChannelsNames();
unsigned long long getCommentsNumber(string channelNameURL),
			       getCommentsNumberForVideos(vector<string> videosIds, bool isPeppaPigLike);
long long convertStrToLongLong(string str);
void print(string s);
bool contains(string subject, string find),
	 startsWith(string subject, string test),
	 writeFile(string filePath, string option, string toWrite),
	 writeFile(string filePath, string option, vector<string> toWrite),
	 isFrench(string youtuberId);

unsigned int peppaPigLike = 0,
			 peppaPigLikeMost = 0,
			 notGettable = 0;

string KEY = "YOUR_API_KEY";//"YOUR_API_KEY";//"YOUR_API_KEY";//"YOUR_API_KEY";
unsigned long long costConsumed = 0;

int main()
{
	vector<string> channelsNames = getChannelsNames(),
		           realChannelsNames = getRealChannelsNames();
	unsigned int channelsNamesSize = channelsNames.size();
	/*string toWrite = "";
	for(unsigned int channelsNamesIndex = 0; channelsNamesIndex < channelsNamesSize; channelsNamesIndex++)
	{
		string channelName = channelsNames[channelsNamesIndex],
		       youtuberId = getYoutuberId(channelName);
		toWrite += youtuberId;
		if(channelsNamesIndex < channelsNamesSize - 1)
			toWrite += "\n";
	}
	writeFile("french100Ids.txt", "w", toWrite);
	return 0;*/
	/*writeFile("names.txt", "w", realChannelsNames);
	return 0;*/
	
	if(false)
	for(unsigned int channelsNamesIndex = 0/*19*//*17*/; channelsNamesIndex < channelsNamesSize/*20*//*18*/; channelsNamesIndex++)
	{
		string channelName = channelsNames[channelsNamesIndex],
		       realChannelName = realChannelsNames[channelsNamesIndex];
		//cout << realChannelName << " ";
		print(realChannelName);
		unsigned long long commentsNumber = getCommentsNumber(channelName);
	}
	//getCommentsNumber("UCn9l4gU5mkmmlC2eiVu0LHw");
	getCommentsNumber("UCLMKLU-ZuDQIsbjMvR3bbog");//"UCWIdqSQekeGmUWlSFeCiEnA");
	print("notGettable: " + convertNbToStr(notGettable));

	return 0;
}

unsigned long long getCommentsNumber(string channelNameURL)
{
	peppaPigLike = 0;
	peppaPigLikeMost = 0;
	string youtuberId = getYoutuberId(channelNameURL),
		   url = "https://youtube.googleapis.com/youtube/v3/commentThreads?part=replies,snippet&allThreadsRelatedToChannelId=" + youtuberId + "&key=" + KEY,
		   comment = getHTTPS(url);
	bool isPeppaPigLike = contains(comment, "has disabled comments."); // does "censor" comments ?
	if(isPeppaPigLike)
	{
		print("isPeppaPigLike");
		notGettable++;
	}
	/*if(!isFrench(youtuberId))
	{
		print("not declared as french");
		notGettable++;
	}*/
	//return 0;

	vector<string> videosIds = getVideosIds(youtuberId);
	unsigned int videosIdsSize = videosIds.size();
	
	//if(isPeppaPigLike)
	//	print("isPeppaPigLike " + convertNbToStr(videosIdsSize));
	vector<vector<string>> videosIdsSlices = getSlicesOfVideos(videosIds, 50);
	unsigned int videosIdsSlicesSize = videosIdsSlices.size();
	unsigned long long totalCommentsNumber = 0;
	for(unsigned int videosIdsSlicesIndex = 0; videosIdsSlicesIndex < videosIdsSlicesSize; videosIdsSlicesIndex++)
	{
		vector<string> videosIdsSlice = videosIdsSlices[videosIdsSlicesIndex];
		unsigned long long commentsNumber = getCommentsNumberForVideos(videosIdsSlice, isPeppaPigLike);
		totalCommentsNumber += commentsNumber;
	}
	/*if(isPeppaPigLike)
	{
		print("peppaPigLikeMost: " + convertNbToStr(peppaPigLikeMost));
	}*/
	if(isPeppaPigLike)
		cout << "isPeppaPigLike ";
	print("comments number: " + withSpaces(totalCommentsNumber) + " videos number: " + withSpaces(videosIdsSize));
	return totalCommentsNumber;
}

unsigned long long getCommentsNumberForVideos(vector<string> videosIdsSlice, bool isPeppaPigLike)
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
		
		string id = item["id"],
			   commentCountStr = statistics.contains("commentCount") ? statistics["commentCount"] : "0";
		unsigned long long commentsNumber = convertStrToLongLong(commentCountStr);
		if(commentsNumber > 0)
		{
			//print("id: " + id);
			/*string url = "https://youtube.googleapis.com/youtube/v3/commentThreads?part=id&videoId=" + id + "&key=" + KEY,
				   content = getHTTPS(url);
			if(!contains(content, " has disabled comments."))
			{
				//print("Terre, Terre: " + id + " !"); // Christophe Colomb
				totalCommentsNumber += commentsNumber;
			}*/
			totalCommentsNumber += commentsNumber;
		}
		/*if(commentsNumber > 0 && isPeppaPigLike)
		{
			*//*string url = "https://youtube.googleapis.com/youtube/v3/commentThreads?part=id&videoId=" + id + "&key=" + KEY,
				   content = getHTTPS(url);
			bool commentsEnabled = !contains(content, "has disabled comment");*/
			/*string url = "https://www.youtube.com/watch?v=" + id,
				   content = getHTTPS(url);
			bool commentsEnabled = !contains(content, "Les commentaires sont désactivés.");
			if(commentsEnabled)
			{
				print("ALIBA ALIBA ALIBA ALIBA id: " + id + " " + convertNbToStr(commentsNumber) + " " + url + " " + content);
				//exit(42);
			}
			else
			{
				//print("peppa pig like " + convertNbToStr(commentsNumber) + " " + convertNbToStr(peppaPigLikeMost) + " " + convertNbToStr(peppaPigLike));
				peppaPigLike++;
				if(commentsNumber > peppaPigLikeMost)
					peppaPigLikeMost = commentsNumber;
			}
		}*/
		//totalCommentsNumber += commentsNumber;
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
    return channelsNames;
}

string getYoutuberId(string youtuberName)
{
	if(startsWith(youtuberName, "UC"))
		return youtuberName;
	string url = "https://www.youtube.com/user/" + youtuberName,
		   content = getHTTPS(url),
		   youtuberId = split(split(content, "\"browseId\":\"")[1], "\"")[0];
	return youtuberId;
}

vector<string> getVideosIds(string youtuberId)
{

	string url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=" + youtuberId + "&key=" + KEY,
           content = getHTTPS(url),
		   pattern = "\"uploads\": \"";
	print("url: " + url);
	if(!contains(content, pattern))
	{
		vector<string> ids;
		print("no uploads: " + content + " URL: " + url);
		return ids;
	}
    string uploadsPlaylist = split(split(content, pattern)[1], "\"")[0],
           cmd = "youtube-dl -j --flat-playlist \"https://www.youtube.com/playlist?list=" + uploadsPlaylist + "\" | jq -r '.id'",
           idsStr = exec(cmd);
	//print("cmd: " + cmd);
	vector<string> ids = split(idsStr, "\n");
	return ids;
}

bool isFrench(string youtuberId) 
{
	string content = getHTTPS("https://youtube.googleapis.com/youtube/v3/channels?part=snippet&id=" + youtuberId + "&key=" + KEY),
		   pattern = "\"country\": \"FR\"";
	return contains(content, pattern);
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((string*)userp)->append((char*)contents, size *nmemb);
	return size *nmemb;
}

string getRedirectedURL(string url)
{
    CURL* curl = curl_easy_init();
	if(!curl)
		print("curl error");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
    
	string got;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &got);

    CURLcode curl_res = curl_easy_perform(curl);
	if(curl_res != CURLE_OK)
	{
		print("perform error");
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_res));
	}

    char* redirectedUrl = NULL;
    if(curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &redirectedUrl) != CURLE_OK)
		print("getinfo error");

	string redirectedUrlStr = redirectedUrl;
	curl_easy_cleanup(curl);

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
		if(!contains(got, "has disabled comments"))
		{
			print("key completely used ! url: " + url);
			exit(42);
		}
		else
		{
		}
    }
	costConsumed++;
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

bool writeFile(string filePath, string option, string toWrite)
{
    FILE* file = fopen(filePath.c_str(), option.c_str());
    if(file != NULL)
    {
        fputs(toWrite.c_str(), file);
        fclose(file);
        return true;
    }
    return false;
}

string toString(vector<string> vec)
{
	string res = "";
	unsigned int vecSize = vec.size();
	for(unsigned int vecIndex = 0; vecIndex < vecSize; vecIndex++)
	{
		string el = vec[vecIndex];
		res += el;
		if(vecIndex < vecSize - 1)
			res += "\n";
	}
	return res;
}

bool writeFile(string filePath, string option, vector<string> toWrite)
{
	return writeFile(filePath, option, toString(toWrite));
}
