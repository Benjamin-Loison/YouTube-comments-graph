#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <curl/curl.h>
#include <json.hpp>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>

//#include <gcrypt.h>
//#include <errno.h>

using namespace std;
using json = nlohmann::json;

/*static pthread_mutex_t* lockarray;

GCRY_THREAD_OPTION_PTHREAD_IMPL;

void init_locks(void)
{
    gcry_control(GCRYCTL_SET_THREAD_CBS); // maybe required second argument cf https://www.gnupg.org/documentation/manuals/gcrypt-devel/Multi_002dThreading.html
}*/
// this code doesn't help for SSL bug and nor domain resolving bug

// maybe need gcry_check_version

#define kill_locks()

//#define YOUTUBE_DATA_API_V3_URL "https://youtube.googleapis.com/youtube/v3/"
// the compiler is bruhing me

#define UNSIGNED_INT_MAX 4294967295
#define UNSIGNED_LONG_MAX 18446744073709551615

// used to be 100 but with current 195 keys with a speed of two requests per second per thread only 11 threads can run continuously without in theory exceeds all keys - now 13 (have to divide number of keys per 17.28 to know how many threads can support)
// well with my little program averageRequestTime.py it's clear that it's more 3 requests per second and not 2 - so need to divide by 25.92
//#define THREADS_NB 10
// using variable because keys number depends a lot of Google keys suspension etc and i'm fed with computing by hand every time this important value

// \\u003c <
// \\u003e >

//#define PEPPA_PIG_PATTERN "has disabled comments."
//#define PEPPA_PIG_PATTERN ">videoId</a></code> parameter has disabled comments."
#define PEPPA_PIG_PATTERN " parameter has disabled comments."
// original without modification: The video identified by the <code><a href=\"/youtube/v3/docs/commentThreads/list#videoId\">videoId</a></code> parameter has disabled comments.
//#define QUOTA_EXCEEDED_PATTERN ">quota</a>."
#define QUOTA_EXCEEDED_PATTERN "The request cannot be completed because you have exceeded your "
// original without modification: The request cannot be completed because you have exceeded your <a href=\"/youtube/v3/getting-started#quota\">quota</a>.
#define TRANSIENT_ERROR_PATTERN "The API server failed to successfully process the request. While this can be a transient error, it usually indicates that the request's input is invalid. Check the structure of the "
// original without modification: The API server failed to successfully process the request. While this can be a transient error, it usually indicates that the request's input is invalid. Check the structure of the \u003ccode\u003ecommentThread\u003c/code\u003e resource in the request body to ensure that it is valid.
#define BACKEND_ERROR_PATTERN "The service is currently unavailable."
// when it's not a problem from my program it's one from YouTube - c'est honteux
#define SUSPENSION_ERROR_PATTERN "' has been suspended."
// original without modification: Permission denied: Consumer 'api_key:YOUR_API_KEY' has been suspended.
#define API_KEY_NOT_VALID_PATTERN "API key not valid. Please pass a valid API key."
// even if it's valid u_u just doing it sometimes randomly
// seems also to have "The request is missing a valid API key." sometimes but why ?

#define MULTITHREAD_GET_NAMES
//#define MULTITHREAD_VIDEOS
// don't use multithread videos while using multithread channels otherwise it's not going to work

template<typename T>
string convertNbToStr(const T& number)
{
    ostringstream convert;
    convert << number;
    return convert.str();
}

vector<vector<string>> slicing(vector<string>* vec, unsigned int sliceSize);
vector<string> getFileContent(string path),
	           split(string s, string delimiter = " "),
			   getVideosYTDL(string youtuberId, unsigned int threadsIndex);
void print(string s, unsigned int threadsIndex = 0),
	 log(string s, unsigned int threadsIndex),
	 workChannel(string youtuberId, unsigned int threadsIndex, bool force = false, bool isResuming = false),
	 work(unsigned int threadsIndex),
	 workGetNames(map<string, string>* names, vector<vector<string>>* slices, unsigned int lowerBound, unsigned int upperBound, unsigned int threadsIndex);
bool isFrench(string youtuberId, unsigned int threadsIndex),
	 contains(string subject, string find),
	 writeFile(string filePath, string option, string toWrite, unsigned int threadsIndex),
	 writeFile(string filePath, string option, vector<string> toWrite, unsigned int threadsIndex),
	 doesFileExist(string name),
	 startsWith(string subject, string test),
	 checkWikipedia(string title, unsigned int threadsIndex),
	 removeFile(string filePath);
string getHTTPS(string url, unsigned int threadsIndex, bool needFake = false, bool needFollow = false, bool doesWebpageExist = false, bool verbose = true),
	   replace(string, const string&, const string& = ""),
	   exec(string cmd, unsigned int threadsIndex),
	   toString(map<string, unsigned int>),
	   toString(map<string, string>*),
	   toString(vector<string>),
	   replaceAll(string, const string&, const string& = ""),
	   scrap(string youtuberId, FILE* detailsFile, string nextPageToken, unsigned int callsIndex, map<string, unsigned int>* relations, map<string, string>* names, map<string, string>* commentsToChannels, unsigned int* channelCommentsNumber, unsigned int threadsIndex, string videoId = ""),
	   escape(string url),
	   join(vector<string>, unsigned int = 0, int = -1, string = " "),
	   replaceLast(string subject, string search, string replace = "");

mutex threadPoolMutex,
	  threadPoolChannelMutex;
vector<string> KEYS,
			   channelsIds,
			   channelsNames,
			   videosIds,
			   wasTreating,
			   linesLogs;
unsigned int keyIndex = 0, // could if used all restart and if restart to quick or check all keys and then stop ? - done
			 costConsumed = 0,
			 channelsIdsSize,
			 channelsIdsIndex = 0,
			 videosIdsSize,
			 videosIdsIndex = 0,
			 treated = 0,
			 threadsNumber = 0,
			 threadsChannelNumber = 0,
			 webRequests = 0,
			 treatedWithComments = 0,
			 commentsTreated = 0,
			 THREADS_NB = 1,
			 linesLogsSize,
			 wasTreatingSize;
string KEY = "KEY",
	   REAL_KEY,
	   separator = " ";

string SqueezieId = "UCWeg2Pkate69NFdBeuRFTAw",
	   GameMixTreize = "UCNVMW8UDDZYVxXJqVs3lnUA",
	   BTSId = "UC3IZKseVpdzPSBaWxBxundA",
	   BenjaminLoisonId = "UCF_UozwQBJY4WHZ7yilYkjA",
	   TheVoiceFR = "UCQRELbX0H5FCokIFxOAsHFA",
	   DaftPunk = "UC_kRDKYrUlrbtrSiyu5Tflg",
	   authenticPl = "UCdeIkFMrH0N_feYzyFcIBUw";
int convertStrToInt(string str);

bool increaseKeyCalled = false;// home made mutex

// a problem of this algorithm is that storing in variables huge comments details up to some Go for each thread is just impossible to correctly manage, - well in fact details are written while reading
// should reduce this amount of data or just write in raw - so no such need
// however have to store namesBis content until write and multiplying by THREADS_NB it can goes up to one or two Go could not write UC and delimiter for instance to save a bit of space (~ 10%) and not writing destination interaction if he is the vide youtuber - well that's channels folder not namesBis which can be written while reading from youtube
// - well in fact namesBis should still save vector of ids treated so it would "only" save by a factor 1.25 the RAM usage (channel name is not very long compared to its id)
// or could write in raw or just get it from details analyze
// is there any other reason why this algorithm use a "lot of" ram ? après les appels avec le shell à des instances de curl ce n'est pas ouf ^^'
// doesn't seem to have recursive function (which would cost a lot for RAM if misused)

int main()
{
	KEYS = getFileContent("keys.txt");
	REAL_KEY = KEYS[0];
	THREADS_NB = ((double)KEYS.size()) / 25.92;
	//THREADS_NB = 1;
	print("THREADS_NB: " + convertNbToStr(THREADS_NB)); // 7 currently 29/08/21
	//return 0;

	if(curl_global_init(CURL_GLOBAL_ALL) != 0) // useless if only use only the shell
	{
		print("curl_global_init error");
		return 1;
	}

	//init_locks();

	//print(convertNbToStr(isFrench(SqueezieId)));
	//print(convertNbToStr(isFrench(BTSId)));

	//workChannel(SqueezieId, 0);
	//return 0;
	
	// SEXION D'ASSAUT was involved in a crash because of the ' while checking if there is any wikipedia webpage associated and was using ' around url for curl
	
	/*wasTreating.push_back("UCWeg2Pkate69NFdBeuRFTAw"); // SQUEEZIE
	wasTreating.push_back("UCNVMW8UDDZYVxXJqVs3lnUA"); // GameMixTreize
	wasTreating.push_back("UCo3i0nUzZjjLuM7VjAVz4zA"); // Michou
	wasTreating.push_back("UCgvqvBoSHB1ctlyyhoHrGwQ"); // Amixem
	wasTreating.push_back("UCCCPCZNChQdGa9EkATeye4g"); // FRANCE 24
	//wasTreating.push_back("UC_ATXX0ACFwsGvJsFJBoLMQ"); // Les Anges - La chaîne officielle
	//wasTreating.push_back("UCsOmgZNGdaKkU1-TlPYoVeQ"); // SEXION D'ASSAUT
	wasTreating.push_back("UC704-gdkAeYFEjudxRCEAJA"); // TheKAIRI78
	wasTreating.push_back("UC3rxwrZSiTp6Kk2RXcyHtCA"); // L'atelier de Roxane
	wasTreating.push_back("UCDPK_MTu3uTUFJXRVcTJcEw");*/ // Mcfly et Carlito
	// l'idée était bonne mais en vrai c'est prise de tête du turfu
	// could retry to work on multithread on a single channel to improve speed because using allThreadsRelatedToChannelId is really slow it seems while it shouldn't for videos (at least we can multithread)
	wasTreatingSize = wasTreating.size();
	
	// should be inside but that's exceptionnal (could use a boolean to make a step or not)
	
	if(false)//!wasTreating.empty())
	{
		string CPPFolder = "/mnt/c/Users/Benjamin/Desktop/BensFolder/DEV/Candco/CPP/Projects/YouTubeCommentsGraph/CPP/",
			resumePath = CPPFolder + "resume/";
	   
		print("reading logs");
		linesLogs = getFileContent(CPPFolder + "logs/0.txt");
		print("read logs");
		linesLogsSize = linesLogs.size();
		
		// generate these files now because otherwise will have multithreading inside each thread possibly ^^'
		// if managed to write channel name of commenters while treating we wouldn't do that
		for(unsigned int wasTreatingIndex = 0; wasTreatingIndex < wasTreatingSize; wasTreatingIndex++)
		{
			string youtuberId = wasTreating[wasTreatingIndex],
				   resumeFile = resumePath + youtuberId + ".txt",
				   basicPath = "/mnt/f/YouTube/CommentsGraph/",
				   commonPath = basicPath + "details/" + youtuberId,
				   detailsPath = commonPath + ".txt";
			map<string, string> names;
			// i don't know whether or not details contains more channels ids than relations file but we only care about relations so let's only consider this for speed reasons ?
			print("reading details of " + youtuberId);
			vector<string> linesDetails = getFileContent(detailsPath);//,
						   //namesToGet; // otherwise might be slow
			print("read details of " + youtuberId);
			map<string, bool> namesToGet; // is there a data structure such that being like a map but without storing any data ?
			unsigned int linesDetailsSize = linesDetails.size();
			for(unsigned int linesIndex = 0; linesIndex < linesDetailsSize; linesIndex++)
			{
				string line = linesDetails[linesIndex];
				vector<string> lineParts = split(line);
				unsigned short linePartsSize = lineParts.size();
				if(linePartsSize >= 5)
				{
					string commentId = lineParts[0],
						   src = lineParts[1],
						   dest = lineParts[2];
					if(startsWith(commentId, "Ug") && startsWith(src, "UC") && startsWith(dest, "UC"))
					{
						namesToGet[src] = true;
						namesToGet[dest] = true;
					}
				}
			}
			unsigned int namesToGetSize = namesToGet.size();
			print("namesToGet filled for " + youtuberId + " ! size: " + convertNbToStr(namesToGetSize));
			vector<string> namesToGetVec; // likewise we can parallelize
			for(map<string, bool>::iterator it = namesToGet.begin(); it != namesToGet.end(); it++)
			{
				string nameToGet = it->first;
				namesToGetVec.push_back(nameToGet);
			}
			unsigned int namesToGetVecSize = namesToGetVec.size();
			print("namesToGetVec filled for " + youtuberId + " ! size: " + convertNbToStr(namesToGetVecSize));
			vector<vector<string>> slices = slicing(&namesToGetVec, 50);
			unsigned int slicesSize = slices.size(),
						 slicesSlice = slicesSize / THREADS_NB; // could change this THREADS_NB because requests are maybe answered faster
			print("slicesSize: " + convertNbToStr(slicesSize));
			#ifdef MULTITHREAD_GET_NAMES
				thread threads[THREADS_NB];
				for(unsigned int threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++)
				{
					threadsNumber++;
					threads[threadsIndex] = thread(workGetNames, &names, &slices, threadsIndex * slicesSlice, (threadsIndex + 1) * slicesSlice, threadsIndex);
				}
				unsigned int lastNamesSize = 0;
				while(threadsNumber > 0)
				{
					unsigned int namesSize = names.size();
					print("namesSizeDifference (" + convertNbToStr(namesSize - lastNamesSize) + ") namesSize (" + convertNbToStr(namesSize) + ")");
					lastNamesSize = namesSize;
					sleep(1);
				}
				for(unsigned int threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++) // might seem necessary
				{
					threads[threadsIndex].join();
				}
			#else
				workGetNames(&names, &slices, 0, slicesSize, 0);
			#endif
			unsigned int namesSize = names.size();
			print("names filled for " + youtuberId + " ! size: " + convertNbToStr(namesSize));
			/*string namesStr = toString(&names);
			print("namesStr filled !");
			writeFile(resumeFile, "w", namesStr, 0);*/
			
			FILE* file = fopen(resumeFile.c_str(), "a");
			if(file != NULL)
			{
				bool isFirst = true;
				for(map<string, string>::iterator it = names.begin(); it != names.end(); it++)
				{
					string toWrite = "";
					toWrite = it->first + separator + it->second;
					if(!isFirst)
						toWrite += "\n";
					isFirst = false;
					if(fputs(toWrite.c_str(), file) == EOF)
					{
						print("fputs error");
					}
				}
				if(fclose(file) != 0)
				{
					print("fclose error");
				}
			}
			else
			{
				print("fopen error");
			}
			//break;
		}
		return 0;
	}
	
	// we lost commenters names because crashed while proceeding, can try to recover them using the API ? but it is maximum 50 per API call :S
	// combien de commentaires en moyenne les gens laissent ils ?
	// et combien d'appels supplémentaires pour les plus de 5 réponses on doit faire ?
	// la question reformulée dans le cas de Squeezie est dans quelle sens est l'inégalité entre 56 978 (le callsIndex actuel avant crash) et 43 927 (le nombre de commentateurs actuel avant crash divisé par 50)
	// on a donc envie de juste récupérer les pseudonymes des commentateurs mais ça parait pas super worth le temps de coder le système puisque les 13 051 appels en plus prendraient 1h10
	// après on pourrait multithreadé puisqu'il n'y a pas de pageToken l'empêchant
	// et à ce moment là on peut passer de refaire tout au grand minimum 5h10 à quelque chose comme moins d'une heure
	// le principal gain dépend de si le multithreading marche bien

	channelsIds = getFileContent("../CommentsCounter/french100Ids.txt");
	channelsNames = getFileContent("../CommentsCounter/names.txt");
	channelsIdsSize = channelsIds.size();
	//workChannel(authenticPl, 0, true);
	//return 0;
	/*for(unsigned int channelsIndex = 0; channelsIndex < /*27*//*channelsIdsSize; channelsIndex++)
	{
		string channelId = channelsIds[channelsIndex],
		       channelName = channelsNames[channelsIndex];
		print("working on: " + channelName + " (" + channelId + ")");
		//workChannel(channelId, 0);
		work(0);
	}*/
	//workChannel(BenjaminLoisonId, 0);
	//return 0;
	
	thread threads[THREADS_NB];
	for(unsigned int threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++)
	{
		threadsNumber++;
		threads[threadsIndex] = thread(work, threadsIndex);
	}
	while(threadsNumber > 0)
	{
		print("treated (" + convertNbToStr(treated) + ") web requests (" + convertNbToStr(webRequests) + ") treatedWithComments (" + convertNbToStr(treatedWithComments) + ") commentsTreated (" + convertNbToStr(commentsTreated) + ")");
		treated = 0;
		webRequests = 0;
		treatedWithComments = 0;
		commentsTreated = 0;
		sleep(1);
	}
	/*for(unsigned int threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++)
    {
        threads[threadsIndex].join();
    }*/

	curl_global_cleanup();

	//kill_locks();

	return 0;
}

void workGetNames(map<string, string>* names, vector<vector<string>>* slices, unsigned int lowerBound, unsigned int upperBound, unsigned int threadsIndex)
{
	unsigned int slicesSize = slices->size();
	for(unsigned int slicesIndex = lowerBound; slicesIndex < upperBound; slicesIndex++)
	{
		#ifndef MULTITHREAD_GET_NAMES
			log(convertNbToStr(slicesIndex) + " / " + convertNbToStr(slicesSize) + " | " + convertNbToStr(names->size()), threadsIndex);
		#endif
		vector<string> slice = slices->at(slicesIndex);
		string concatenated = join(slice, 0, -1, ","),
			   url = "https://youtube.googleapis.com/youtube/v3/channels?part=snippet&id=" + concatenated + "&key=" + KEY,
			   content = getHTTPS(url, threadsIndex, false, false, false, false);
		json data = json::parse(content),
			 items = data["items"];
		// hope there won't be any problem
		unsigned int itemsIndex = 0,
		             itemsSize = items.size();
		for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
		{
			json item = *itemsIt,
				 snippet = item["snippet"];
			string channelName = snippet["title"],
				   commenterId = item["id"];//slice[itemsIndex];
			names->insert(make_pair(commenterId, channelName));
			itemsIndex++;
		}
		if(itemsSize == 0)
		{
			log("not having any item: " + url + ": " + REAL_KEY + ": " + content + " !", threadsIndex);
		}
		/*if(itemsSize != 50)
		{
			log("having only " + convertNbToStr(itemsSize) + " items: " + url + ": " + content + " !", threadsIndex);
		}*/
	}
	threadsNumber--;
}

void work(unsigned int threadsIndex)
{
	while(true)
	{
		threadPoolMutex.lock();
		if(channelsIdsIndex >= channelsIdsSize)
		{
			log("no more channel to work on", threadsIndex);
			break;
		}
		string channelId = channelsIds[channelsIdsIndex],
		       channelName = channelsNames[channelsIdsIndex];
		log("working on " + channelName + " (" + channelId + ") " + convertNbToStr(channelsIdsIndex), threadsIndex);
		channelsIdsIndex++;
		bool wasTreatingChannel = find(wasTreating.begin(), wasTreating.end(), channelId) != wasTreating.end();
		if(doesFileExist("channels/" + channelId + ".txt") && !wasTreatingChannel)
		{
			log("already done channel, skipping", threadsIndex);
			threadPoolMutex.unlock();
			continue;
		}
		if(wasTreatingChannel)
			log("resuming channel work", threadsIndex);
		threadPoolMutex.unlock();
		workChannel(channelId, threadsIndex, false, wasTreatingChannel);
	}
	threadsNumber--;
}

void increaseRelation(string src, string dest, map<string, unsigned int>* relations)
{
	string key = src + separator + dest;
	if(relations->find(key) != relations->end())
		relations->at(key)++;
	else
		relations->insert(make_pair(key, 1));
}

void workComment(string youtuberId, FILE* detailsFile, json comment, string commentId, string videoId, string answering, map<string, unsigned int>* relations, map<string, string>* names, map<string, string>* commentsToChannels, unsigned int* channelCommentsNumber, unsigned int threadsIndex) // is first argument really required ?
{
	(*channelCommentsNumber)++;
	commentsTreated++;
	string commenterId;
	if(!comment.contains("authorChannelId"))
	{
		if(comment.contains("channelId"))
			commenterId = comment["channelId"];
		else
		{
			commenterId = commentId; // likewise have a unique identity
			//log("not containing channelId (" + commentId + ", " + answering + "): " + comment.dump(4), threadsIndex); // it happens
		}
	}
	else
	{
		json authorChannelId = comment["authorChannelId"];
		if(!authorChannelId.contains("value"))
		{
			log("no value: " + comment.dump(4), threadsIndex);
			return;
		}
		commenterId = authorChannelId["value"];
	}
	increaseRelation(commenterId, youtuberId, relations);
	//if(commentId[0] == ' ')
	//	commentId = commentId.substr(1, commentId.length() - 1);
	// just an idea to make youtube snapshot without having to store them, just ask the beginning of something big like all comments of a channel and keep the pageToken/nextPageToken and if it doesn't expire should check before get delete some comment and see if still there
	commentsToChannels->insert(make_pair(commentId, commenterId));
	// TOD: check if reply a specific comment change parentId to specific reply ?
	string text = comment["textOriginal"],
	       date = comment["publishedAt"],
		   authorDisplayName = comment["authorDisplayName"],
	       toWrite = commentId + separator + commenterId + separator + youtuberId + separator + videoId + separator + date + separator + text + "\n";
	names->insert(make_pair(commenterId, authorDisplayName));
	if(answering != "")
	{
		string channelId = commentsToChannels->at(answering);
		increaseRelation(commenterId, channelId, relations);
		//if(commentId[0] == ' ')
		//	log("HEY", threadsIndex);
		toWrite += commentId + separator + commenterId + separator + channelId + separator + videoId + separator + date + separator + text + "\n"; // could just be = no ? that would be more clean no ?
	}
	if(fputs(toWrite.c_str(), detailsFile) == EOF)
		log("fputs detailsFile error", threadsIndex);
}

// could also from Prénom Nom of someone try to guess which is his youtube channel and likewise get all his comments
string scrapComment(string youtuberId, FILE* detailsFile, string commentId, string nextPageToken, unsigned int callsIndex, map<string, unsigned int>* relations, map<string, string>* names, map<string, string>* commentsToChannels, unsigned int* channelCommentsNumber, unsigned int threadsIndex, string videoId = "")
{
	log("callsIndex for comments: " + convertNbToStr(callsIndex), threadsIndex);
	// not available at YOUTUBE_DATA_API_V3_URL ?
	string url = "https://www.googleapis.com/youtube/v3/comments?part=snippet&parentId=" + commentId + "&textFormat=plainText&maxResults=100"; // can't use multiple parentId and I think that otherwise the maxResults would be global
	if(nextPageToken != "")
        url += "&pageToken=" + nextPageToken;
	url += "&key=" + KEY; // otherwise replace last may fail (it was the case)
	string content = getHTTPS(url, threadsIndex);
	json data = json::parse(content); // no memory leak within json library ?
	//content.clear(); // to save RAM ? - or could use a scope
	if(data.contains("nextPageToken"))
		nextPageToken = data["nextPageToken"];
	json items = data["items"];
	for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
	{
		json item = *itemsIt,
			 snippet = item["snippet"];
		workComment(youtuberId, detailsFile, snippet/*could send a pointer*/, item["id"], videoId, commentId, relations, names, commentsToChannels, channelCommentsNumber, threadsIndex);
	}
	treatedWithComments++;
	return nextPageToken;
}

string scrap(string youtuberId, FILE* detailsFile, string nextPageToken, unsigned int callsIndex, map<string, unsigned int>* relations, map<string, string>* names, map<string, string>* commentsToChannels, unsigned int* channelCommentsNumber, unsigned int threadsIndex, string videoId)
{
	string url = "https://youtube.googleapis.com/youtube/v3/commentThreads?part=snippet,replies&" + (videoId != "" ? "videoId=" + videoId : "allThreadsRelatedToChannelId=" + youtuberId) + "&maxResults=100&textFormat=plainText";
	if(nextPageToken != "")
		url += "&pageToken=" + nextPageToken;
	url += "&key=" + KEY;
	string content = getHTTPS(url, threadsIndex);
	/*if(contains(content, TRANSIENT_ERROR_PATTERN))
	{
		log("transient error detected for url: " + url + ": content: " + content, threadsIndex);
		return scrap(youtuberId, detailsFile, nextPageToken, callsIndex, relations, names, commentsToChannels, channelCommentsNumber, threadsIndex, videoId);
		// because stackoverflow at least before 523 992 recursive calls with 3 requests per second
		// need less than 2 days to crash if it's not that much a transient error
	}
	if(contains(content, BACKEND_ERROR_PATTERN))
	{
		log("backend error detected for url: " + url + ": content: " + content, threadsIndex);
		return scrap(youtuberId, detailsFile, nextPageToken, callsIndex, relations, names, commentsToChannels, channelCommentsNumber, threadsIndex, videoId);
	}*/
	unsigned int contentLength = content.length(),
	             relationsSize = relations->size(),
				 namesSize = names->size();
	// 112 047 callsIndex for Squeezie
	log("callsIndex: " + convertNbToStr(callsIndex) + " " + convertNbToStr(*channelCommentsNumber) + " " + convertNbToStr(relationsSize) + " " + convertNbToStr(namesSize) + " " + convertNbToStr(costConsumed) + " " + nextPageToken + " (" + convertNbToStr(contentLength) + ") " + videoId + " !", threadsIndex);
	if(callsIndex == 0 && contains(content, PEPPA_PIG_PATTERN))
	{
		return "PEPPA PIG";
	}
	json data = json::parse(content);
	//string originalPageToken = nextPageToken;
	if(data.contains("nextPageToken"))
	{
		if(nextPageToken == data["nextPageToken"])
		{
			log("same page token found url: " + url + ": content: " + content, threadsIndex);
		}
		nextPageToken = data["nextPageToken"];
	}
	else
	{
		log("no page token found url: " + url + ": content: " + content, threadsIndex); // seems to be normal at the end of whole scraping
	}
	json items = data["items"];
	unsigned int itemsIndex = 0;
	for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
	{
		json item = *itemsIt,
			 snippet = item["snippet"],
			 topLevelComment = snippet["topLevelComment"],
			 commentSnippet = topLevelComment["snippet"];
		string videoId = snippet.contains("videoId") ? snippet["videoId"] : snippet["channelId"];
		workComment(youtuberId, detailsFile, commentSnippet, topLevelComment["id"], videoId, "", relations, names, commentsToChannels, channelCommentsNumber, threadsIndex);
		if(item.contains("replies"))
		{
			json replies = item["replies"]["comments"];
			unsigned int repliesSize = replies.size();
			if(repliesSize <= 4)
			{
				for(json::iterator repliesIt = replies.begin(); repliesIt != replies.end(); repliesIt++)
				{
					json reply = *repliesIt,
					 	 replySnippet = reply["snippet"];
					workComment(youtuberId, detailsFile, replySnippet, /*""*/reply["id"], videoId, replySnippet["parentId"], relations, names, commentsToChannels, channelCommentsNumber, threadsIndex);
				}
			}
			else
			{
				string commentId = item["id"],
				       originalPageToken = "ignition",
					   nextPageToken = "";
				unsigned int callsIndex = 0;
				while(nextPageToken != originalPageToken)
				{
					originalPageToken = nextPageToken;
					nextPageToken = scrapComment(youtuberId, detailsFile, commentId, nextPageToken, callsIndex, relations, names, commentsToChannels, channelCommentsNumber, threadsIndex, videoId);
					callsIndex++;
				}
				log("got " + convertNbToStr(callsIndex) + " page(s) of replies to " + commentId + " !", threadsIndex);
			}
		}
		itemsIndex++;
	}
	treated++;
	treatedWithComments++;
	return nextPageToken;
}

void workVideo(string youtuberId, FILE* detailsFile, map<string, unsigned int>* relations, map<string, string>* names, map<string, string>* commentsToChannels, unsigned int* channelCommentsNumber, unsigned int threadsIndex, string id)
{
	string originalPageToken = "ignition",
           nextPageToken = "";
    unsigned int callsIndex = 0;
    bool isVideoPeppaPig = false;
    while(nextPageToken != originalPageToken)
    {
    	originalPageToken = nextPageToken;
		/*pair<string, unsigned int> nextPageTokenCommentsToChannels = scrap(youtuberId, detailsFile, nextPageToken, callsIndex, relations, names, commentsToChannels, channelCommentsNumber, threadsIndex, id);
        nextPageToken = nextPageTokenCommentsToChannels.first;
		commentsToChannels = nextPageTokenCommentsToChannels.second;*/
		nextPageToken = scrap(youtuberId, detailsFile, nextPageToken, callsIndex, relations, names, commentsToChannels, channelCommentsNumber, threadsIndex, id);
        if(nextPageToken == "PEPPA PIG")
        {
        	log("peppa pig video", threadsIndex);
            isVideoPeppaPig = true;
            break;
        }
        callsIndex++;
    }
    if(!isVideoPeppaPig)
    {
    	log("video isn't peppa pig !", threadsIndex);
    }
}

void workForChannel(string youtuberId, FILE* detailsFile, map<string, unsigned int>* relations, map<string, string>* names, map<string, string>* commentsToChannels, unsigned int* channelCommentsNumber, unsigned int threadsIndex)
{
	while(true)
	{
		threadPoolChannelMutex.lock();
		if(videosIdsIndex >= videosIdsSize)
		{
			log("no more video to work on", threadsIndex);
			break;
		}
		string videoId = videosIds[videosIdsIndex];
		log("working on " + videoId + " " + convertNbToStr(videosIdsIndex) + " / " + convertNbToStr(videosIdsSize), threadsIndex);
		videosIdsIndex++;
		threadPoolChannelMutex.unlock();
		workVideo(youtuberId, detailsFile, relations, names, commentsToChannels, channelCommentsNumber, threadsIndex, videoId);
	}
	threadsChannelNumber--;
}

void workChannel(string youtuberId, unsigned int threadsIndex, bool force, bool isResuming)
{
	if(force || isFrench(youtuberId, threadsIndex))
	{
		map<string, unsigned int> relations;
    	map<string, string> names,
			                commentsToChannels;

		string CPPFolder = "/mnt/c/Users/Benjamin/Desktop/BensFolder/DEV/Candco/CPP/Projects/YouTubeCommentsGraph/CPP/",
		       resumePath = CPPFolder + "resume/",
		       basicPath = "/mnt/f/YouTube/CommentsGraph/",
		       commonPath = basicPath + "details/" + youtuberId,
			   detailsPath = commonPath + ".txt",
			   namesBisPath = commonPath + "-namesBis.txt",
			   optionDetails = "w", // can't just append on resume because may have not written whole line etc - just assume that was progressing on a non peppa pig channel and one with at least a comment with five answers - well in fact last hypothesis isn't required (was planning to use commentId looking for more than 5 answers to make a link with details from logs but we can just ask to YouTube from last call logged)
			   originalPageToken = "ignition",
			   nextPageToken = "";
			   
		unsigned int callsIndex = 0,
		             channelCommentsNumber = 0;
			   
		if(isResuming)
		{
			log("reading details", threadsIndex);
			vector<string> linesDetails = getFileContent(detailsPath);
			unsigned int linesDetailsSize = linesDetails.size();
			log("read details " + convertNbToStr(linesDetailsSize), threadsIndex);
			string pattern = ": callsIndex: ";
			bool isFirst = true; // otherwise might found something that it is not even first line completely well written
			for(unsigned int linesIndex = linesLogsSize - 1; linesIndex >= 0; linesIndex--)
			{
				string line = linesLogs[linesIndex];
				if(contains(line, pattern))
				{
					// format on 290821
					// time: callsIndex: callsIndex relationsSize namesSize costConsumed nextPageToken (contentLength) videoId ! - threadsIndex
					// TODO: check if recover fine sizes etc
					// example:
					// 29-08-21#08-33-26-281: callsIndex: 56978 3655886 2196332 1373182 CENSORED (167185)  ! - 0
					vector<string> lineParts = split(line);
					unsigned short linePartsSize = lineParts.size();
					if(linePartsSize >= 11)
					{
						if(isFirst)
						{
							isFirst = false;
							continue;
						}
						callsIndex = convertStrToInt(lineParts[2]);
						channelCommentsNumber = convertStrToInt(lineParts[3]); // not managing no name users (even if could) because this is only an indicator
						costConsumed += convertStrToInt(lineParts[5]);
						originalPageToken = lineParts[6];
						
						// now reducing details file by removing this call results
						
						string url = "https://youtube.googleapis.com/youtube/v3/commentThreads?part=snippet,replies&allThreadsRelatedToChannelId=" + youtuberId + "&textFormat=plainText&pageToken=" + originalPageToken + "&key=" + KEY,
						       content = getHTTPS(url, threadsIndex);

						// could work from more than 5 answers calls but that would be more complicated and should be sure that statistically have more than one comment with 5 answers every 100 top level comments
						json data = json::parse(content),
						     items = data["items"],
							 item = items[0],
							 snippet = item["snippet"],
			                 topLevelComment = snippet["topLevelComment"];
			            string commentId = topLevelComment["id"];
						for(/*unsigned*/ int linesDetailsIndex = linesDetailsSize - 1; linesDetailsIndex >= 0; linesDetailsIndex--) // "no problem" with this and it is faster
						//for(unsigned int linesDetailsIndex = 0; linesDetailsIndex < linesDetailsSize; linesDetailsIndex++)
						{
							string lineDetails = linesDetails[linesDetailsIndex];
							vector<string> lineParts = split(lineDetails);
							if(lineParts[0] == commentId)
							{
								linesDetails.resize(linesDetailsIndex); // should do the job
								linesDetails.push_back(""); // manage new line here 4242
								writeFile(detailsPath, "w", linesDetails, threadsIndex);
								break;
							}
						}
						
						break;
					}
				}
			}
			
			string resumeFile = resumePath + youtuberId + ".txt"; /// TODO: generate all these files at start
			vector<string> linesResume = getFileContent(resumeFile);
			unsigned int linesResumeSize = linesResume.size();
			for(unsigned int linesResumeIndex = 0; linesResumeIndex < linesResumeSize; linesResumeIndex++)
			{
				string lineResume = linesResume[linesResumeIndex];
				vector<string> lineParts = split(lineResume);
				names[lineParts[0]] = join(lineParts, 1);
			}
			removeFile(resumeFile);
			
			/// we have to cut details before treating for anything else otherwise not going to be "perfect" - done
			for(unsigned int linesIndex = 0; linesIndex < linesDetailsSize; linesIndex++)
			{
				string line = linesDetails[linesIndex];
				vector<string> lineParts = split(line);
				unsigned short linePartsSize = lineParts.size();
				if(linePartsSize >= 5)
				{
					string commentId = lineParts[0],
					       src = lineParts[1],
					       dest = lineParts[2];
					if(startsWith(commentId, "Ug") && startsWith(src, "UC") && startsWith(dest, "UC"))
					{
						increaseRelation(src, dest, &relations);
					}
					commentsToChannels.insert(make_pair(commentId, src));
				}
			}
			optionDetails = "a"; /// have to pay attention to new line - done at 4242
		}

		FILE* detailsFile = fopen(detailsPath.c_str(), optionDetails.c_str());
    	if(detailsFile == NULL)
			log("detailFile fopen was NULL", threadsIndex);
					 
		// just have to correctly reassign previous  declared variables for resuming - writing last comment id would help after each "scrap" function call
					 
		#ifdef MULTITHREAD_VIDEOS
			bool isPeppaPig = true;
		#else
			bool isPeppaPig = false; // most of channels aren't peppa pig
		#endif
		while(!isPeppaPig && nextPageToken != originalPageToken)
		{
			originalPageToken = nextPageToken;
			nextPageToken = scrap(youtuberId, detailsFile, nextPageToken, callsIndex, &relations, &names, &commentsToChannels, &channelCommentsNumber, threadsIndex);
			if(callsIndex == 0 && nextPageToken == "PEPPA PIG")
			{
				isPeppaPig = true;
				break;
			}
			callsIndex++;
		}
		
		if(isPeppaPig)
		{
			// most of channels aren't TheVoiceFR like (having uploads playlist in private) so let's first try this way
			// imagine unclear french channel, peppa pig channel, and private uploads playlist u_u
			// could also treat here channel comments using channelId of commentThreads/list
			vector<string> ids = getVideosYTDL(youtuberId, threadsIndex);
			unsigned int idsSize = ids.size();
			#ifdef MULTITHREAD_VIDEOS
				videosIds = ids;
				videosIdsSize = idsSize;
				thread threads[THREADS_NB];
				for(unsigned int threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++)
				{
					threadsChannelNumber++;
					threads[threadsIndex] = thread(workForChannel, youtuberId, detailsFile, &relations, &names, &commentsToChannels, channelCommentsNumber, threadsIndex);
				}
				/*while(threadsChannelNumber > 0)
				{
					log("videosTreated " + convertNbToStr(videosTreated), threadsIndex);
					videosTreated = 0;
					sleep(1);
				}*/
				for(unsigned int threadsIndex = 0; threadsIndex < THREADS_NB; threadsIndex++)
				{
					threads[threadsIndex].join();
				}
			#endif
			for(unsigned int idsIndex = 0; idsIndex < idsSize; idsIndex++)
			{
				string id = ids[idsIndex];
				log("video: " + id + " " + convertNbToStr(idsIndex) + " / " + convertNbToStr(idsSize), threadsIndex);
				workVideo(youtuberId, detailsFile, &relations, &names, &commentsToChannels, &channelCommentsNumber, threadsIndex, id);
				/*originalPageToken = "ignition";
				nextPageToken = "";
				callsIndex = 0;
				bool isVideoPeppaPig = false;
				while(nextPageToken != originalPageToken)
        		{
            		originalPageToken = nextPageToken;
            		nextPageToken = scrap(youtuberId, detailsFile, nextPageToken, callsIndex, &relations, &names, &commentsToChannels, threadsIndex, id);
            		if(nextPageToken == "PEPPA PIG")
					{
						log("peppa pig video", threadsIndex);
						isVideoPeppaPig = true;
						break;
					}
					callsIndex++;
        		}
				if(!isVideoPeppaPig)
				{
					log("video isn't peppa pig !", threadsIndex);
				}*/
			}
		}
		//return;
		//exit(42);

		log("before closing detailsFile", threadsIndex);
		log("closing detailsFile", threadsIndex);
		if(fclose(detailsFile) == EOF)
			log("fclose detailsFile error", threadsIndex);

		log("getting relations string", threadsIndex);
    	string toWrite = toString(relations); // could use a pointer
    	log("writing relations", threadsIndex);
		writeFile("channels/" + youtuberId + ".txt", "w", toWrite, threadsIndex);

		log("getting names string", threadsIndex);
		FILE* namesBisFile = fopen(namesBisPath.c_str(), "w");
		log("namesBis opened", threadsIndex);
		if(namesBisFile == NULL)
			log("namesBisFile fopen was NULL", threadsIndex);
		
		bool isFirst = true;
		for(map<string, string>::iterator it = names.begin(); it != names.end(); it++)
		{
			string key = it->first,
				   value = it->second,
				   toAdd = (isFirst ? "" : "\n") + key + separator + value;
			isFirst = false;
			if(fputs(toAdd.c_str(), namesBisFile) == EOF)
				log("fputs namesBisFile error", threadsIndex);
		}
		
		log("closing namesBisFile", threadsIndex);
		if(fclose(namesBisFile) == EOF)
			log("fclose namesBisFile error", threadsIndex);
		log("namesBisFile closed", threadsIndex);
	}
	else
	{
		log("not french", threadsIndex);
	}
}

// if really need to not consume a lot of tokens, could mutualize youtube api calls using promises or something like this
string getDescription(string videoId, unsigned int threadsIndex)
{

}

string getMostViewedVideo(string youtuberId, unsigned int threadsIndex)
{
	string url = "https://youtube.googleapis.com/youtube/v3/search?channelId=" + youtuberId + "&type=video&maxResults=1&order=viewCount&key=" + KEY, // if don't precise type it gives a playlist for joueur du grenier
		   content = getHTTPS(url, threadsIndex);
	json data = json::parse(content),
		 items = data["items"],
		 video = items[0],
		 id = video["id"];
	//log("id: " + id.dump(4), threadsIndex);
	string videoId = id["videoId"];
	return videoId;
}

bool treatWebsite(string link, unsigned int threadsIndex)
{
	string cmd = "whois -h whois.verisign-grs.com " + link,
		   res = exec(cmd, threadsIndex);
	bool isOVH = contains(res, "ovh.com");
	if(!isOVH)
	{
		log("treatWebsite failed: " + cmd + " !", threadsIndex);
	}
	return isOVH;
}

bool isANaturalNumber(string part)
{
	unsigned short partLength = part.length();
	for(unsigned short partIndex = 0; partIndex < partLength; partIndex++)
	{
		char c = part[partIndex];
		if(c < '0' || c > '9')
			return false;
	}
	return true;
}

string extractDigits(string url, unsigned short number)
{
	unsigned int urlLength = url.length();
	if(urlLength < number) return "";
	for(unsigned int urlIndex = 0; urlIndex < urlLength - number + 1; urlIndex++)
	{
		string part = url.substr(urlIndex, number);
		if(isANaturalNumber(part))
		{
			return part;
		}
	}
	return "";
}

string keepBefore(string subject, string delimiter)
{
	if(contains(subject, delimiter))
    {
        vector<string> parts = split(subject, delimiter);
		string part = parts[0];
        return part;
    }
	return subject;
}

// maybe if a foreigner posted multiple comments should save the fact that it is a foreigner otherwise might spend many credits (if use search for instance)
// currently doing if at least one france somewhere it's france but should check if majority is french then france
// could return a score or a boolean or a country repartition or what ?
// we don't really bother with this function memory usage because it is executed once for a channel and not every 100 comments (currently not in discovery mode so not a problem)
bool treatCountry(string link, unsigned int threadsIndex)
{
	string url = replace(link, "http://");
	url = replace(url, "https://");
	//url = replace(url, "www."); // doesn't used to be here
	//log("url: " + url + " !", threadsIndex);
	if(startsWith(url, "www.facebook.com/")) // warning might not be a facebook page, people can put whatever link don't forget, www.
	{
		//log("!" + link + "!", threadsIndex);
		link = keepBefore(link, "%"); // doesn't used to be a function
		link = keepBefore(link, "?");
		link = replace(link, "http://", "https://"); // otherwise have problem
		//link = "https://" + replaceAll(url, "//", "/");
		// -L required for oggy
		string cmdId = "curl -s -L '" + escape(link) + "' -H 'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0' -H 'Accept: text/html' -H 'Cookie: datr=CENSORED; wd=1920x927; c_user=CENSORED; xs=CENSORED'", // cookie wasn't required for oggy but it is for tiboinshape - I promise it's the most reduced to be ok in all cases
		       contentId = exec(cmdId, threadsIndex); //getHTTPS(link, threadsIndex, /*false*/true, true); // curl 'https://www.facebook.com/Oggy.fanpage' doesn't give same output as in C++ with same parameters u_u
		//log("cmd: " + cmd + " !", threadsIndex);
		//log(content, threadsIndex);
		vector<string> parts = split(contentId, "\"entity_id\":\"");//"page/");
		unsigned short partsSize = parts.size();
		//log("partsSize: " + convertNbToStr(partsSize), threadsIndex);
		//log("content " + cmd + ": " + content + " !", threadsIndex);
		string part;
		if(partsSize < 2)
		{
			//vector<string> parts = split(content, "\"fb://profile/"); // tiboinshape - not the right id
			//unsigned short partsSize = parts.size();
			//string cmdPage = "curl -s '" + link + "/about_profile_transparency' -H 'Accept: application/html' -H 'Cookie: wd=1920x927'",
			string cmdPage = "curl -s 'https://" + replaceAll(escape(url) + "/about_profile_transparency", "//", "/") + "' -H 'Accept: application/html' -H 'Cookie: wd=1920x927'", // maybe already have "/" after link ? - yes at least sometimes ^^'
			       contentPage = exec(cmdPage, threadsIndex);
			vector<string> parts = split(contentPage, "\"profile_transparency_id\":\"");
			unsigned short partsSize = parts.size();
			if(partsSize < 2)
			{
				//log("contentId: " + cmdId + ": " + contentId + " !", threadsIndex);
				//log("contentPage: " + cmdPage + ": " + contentPage + " !", threadsIndex);
				// I don't know where ' can be used in various URLs but even if the user can put even if doesn't work it makes me crash so have to patch everywhere
				string cmdAbout = "curl -s 'https://" + replaceAll(escape(url) + "/about_contact_and_basic_info", "//", "/") + "' -H 'Accept: text/html' -H 'Cookie: wd=1920x927; c_user=CENSORED; xs=CENSORED'",
					   contentAbout = exec(cmdAbout, threadsIndex);
				vector<string> parts = split(contentId, "\"message_box_id\":\""); // used to be commentAbout
				unsigned short partsSize = parts.size();
				if(partsSize < 2)
				{
					part = extractDigits(url, 15);
					if(part == "")
					{
						vector<string> parts = split(contentId, "\"fb://page/");
						unsigned short partsSize = parts.size();
						if(partsSize < 2)
						{
							vector<string> parts = split(contentId, "\"pageID\":\"");
							unsigned short partsSize = parts.size();
                        	if(partsSize < 2)
							{
								log("cmdId: " + cmdId + ": " + contentId + " !", threadsIndex);
								log("contentAbout link: " + link + ": url: " + url + ": " + cmdAbout + ": " + contentAbout + " !", threadsIndex); // si faisait un code d'erreur au moins on pourrait rapidement voir s'il y a une erreur
								return false;
							}
							else
							{
								part = split(parts[1], "\"")[0];
							}
						}
						else
						{
							part = split(parts[1], "?")[0];
						}
					}
				}
				else
				{
					vector<string> partsParts = split(parts[1], "\"");
					unsigned short partsPartsSize = partsParts.size();
					part = partsParts[0];
				}
			}
			else
			{
				vector<string> partsParts = split(parts[1], "\"");
            	unsigned short partsPartsSize = partsParts.size();
            	part = partsParts[0];
			}
		}
		else
		{
			vector<string> partsParts = split(parts[1], /*"?"*/"\"");
			unsigned short partsPartsSize = partsParts.size();
			//log("partsPartsSize: " + convertNbToStr(partsPartsSize), threadsIndex);
			part = partsParts[0];
		}
		log(part, threadsIndex);
		string cmdAjax = "curl -s 'https://www.facebook.com/api/graphql' -H 'Cookie: c_user=CENSORED; xs=CENSORED' --data-raw 'fb_dtsg=CENSORED&variables={\"pageID\":\"" + part + "\"}&doc_id=CENSORED'";
		//log(cmd, threadsIndex);
		string contentAjax = exec(cmdAjax, threadsIndex);
		//log(content, threadsIndex);
		json dataRaw = json::parse(contentAjax),
			 items = dataRaw["data"]["page"]["pages_transparency_info"]["admin_locations"]["admin_country_counts"]; // sorted decreasing order ouf
    	for(json::iterator itemsIt = items.begin(); itemsIt != items.end(); itemsIt++)
    	{
        	json item = *itemsIt;
            string country = item["country"]["iso_name"];
			log("country: " + country, threadsIndex);
			if(country == "France")
				return true;
			break; // could just get first instead of loop and breaking
		}
		if(items.size() == 0)
		{
			log("no item: " + cmdAjax + ": " + contentAjax, threadsIndex);
		}

		parts = split(contentId/*Ajax*/, "<title>");
		partsSize = parts.size();
		if(partsSize < 2)
		{
			//log("contentAjax not 2 parts: " + cmdAjax + ": " + contentAjax + " !", threadsIndex);
			log("contentId not 2 parts: " + cmdId + ": " + contentId + " !", threadsIndex);
			return false;
		}
		string title = split(parts[1], "</title>")[0];
		bool isFrenchWikipedia = checkWikipedia(title, threadsIndex);
		if(isFrenchWikipedia)
			return true;
		else
			log("isFrenchWikipedia 695 failed", threadsIndex);
	}
	else if(startsWith(url, "www.twitter.com/")) // www.
	{
		string username = replace(url, "www.twitter.com/"), // www.
			   cmd = "t whois @" + username,
			   res = exec(cmd, threadsIndex);
		vector<string> lines = split(res, "\n");
		unsigned short linesSize = lines.size();
		//log("res: " + res, threadsIndex);
		//log("linesSize: " + convertNbToStr(linesSize), threadsIndex);
		for(unsigned short linesIndex = 0; linesIndex < linesSize; linesIndex++)
		{
			string line = lines[linesIndex];
			//log("line: " + line + " !", threadsIndex);
			if(startsWith(line, "Location"))
			{
				string location = replace(line, "Location");
				unsigned short locationLength = location.length();
				for(unsigned short locationIndex = 0; locationIndex < locationLength; locationIndex++)
				{
					char c = location[locationIndex];
					if(c != ' ')
					{
						location = location.substr(locationIndex, locationLength - locationIndex);
						break;
					}
				}
				log("Location: " + /*line*/location + " !", threadsIndex);
				//if(contains(/*line*/location, "Paris") || contains(location, "Marseille"))
				if(location == "France" || location == "Paris" || location == "Marseille")
				{
					return true;
				}
				break;
			}
		}
	}
	else if(startsWith(url, "soundcloud.com/"))
	{
		string content = getHTTPS(link, threadsIndex);
		if(contains(content, "og:country-name\" content=\"France\""))
			return true;
		vector<string> parts = split(content, " g-type-shrinkwrap-large-secondary\">"); // didn't worked as expected for sound cloud
		unsigned short partsSize = parts.size();
		if(partsSize > 1)
		{
			string part = parts[2], // can be first ?
				   location = split(part, "</h3>")[0];
			if(contains(location, "France"))
				return true;
		}
		else
		{
			log("soundcloud " + link + ": " + content, threadsIndex);
		}
	}
	else
	{
		url = replace(url, "www."); // if replace for others have problem for facebook pages for instance u_u
		if(treatWebsite(/*link*/url, threadsIndex))
			return true;
	}
	return false;
}

bool isFrench(string youtuberId, unsigned int threadsIndex)
{
	// defaultLanguage parameter also at this URL (en by default for OGGY)
	// various localization en and fr in the OGGY case, en understandable but if only both consider as french because en is a standard
	string url = "https://youtube.googleapis.com/youtube/v3/channels?part=snippet&id=" + youtuberId + "&key=" + KEY, // if not such variable isn't show in GDB on crash
	       content = getHTTPS(url, threadsIndex),
		   pattern = "\"country\": \"FR\"";
	bool isCountryFrance = contains(content, pattern);
	if(!isCountryFrance)
	{
		json data = json::parse(content),
			 items = data["items"],
			 firstEntry = items[0];
		string title = firstEntry["snippet"]["title"];
		vector<string> titleParts = split(title);
		unsigned short titlePartsSize = titleParts.size();
		for(unsigned short titlePartsIndex = 0; titlePartsIndex < titlePartsSize; titlePartsIndex++)
		{
			string titlePart = titleParts[titlePartsIndex];
			if(titlePart == "FR") // should maybe manage lowercase too with an equalsIgnoreCase
			{
				log("FR in channel name", threadsIndex);
				return true;
			}
		}

		content = getHTTPS("https://www.youtube.com/channel/" + youtuberId + "/about", threadsIndex, true);
		//log(convertNbToStr(contains(content, "TikTok")), threadsIndex);
		//log(content, threadsIndex);
		vector<string> parts = split(content, "u0026q="),
			           links;
		unsigned short partsSize = parts.size();
		for(unsigned short partsIndex = 0; partsIndex < partsSize; partsIndex++)
		{
			string part = parts[partsIndex],
			       realPart = split(part, "\"")[0];
			//log(realPart, threadsIndex);
			if(startsWith(realPart, "http"))
			{
				realPart = replaceAll(realPart, "%3A", ":");
				realPart = replaceAll(realPart, "%2F", "/");
				if(find(links.begin(), links.end(), realPart) == links.end())
				{
					links.push_back(realPart);
					log(realPart, threadsIndex);
				}
			}
		}
		unsigned short linksSize = links.size();
		for(unsigned short linksIndex = 0; linksIndex < linksSize; linksIndex++)
		{
			string link = links[linksIndex];
			if(treatCountry(link, threadsIndex))
				return true;
		}

		string videoId = getMostViewedVideo(youtuberId, threadsIndex),
			   url = "https://youtube.googleapis.com/youtube/v3/videos?part=contentDetails,id,liveStreamingDetails,localizations,player,recordingDetails,snippet,statistics,status,topicDetails&id=" + videoId + "&key=" + KEY;
		content = getHTTPS(url, threadsIndex);
		data = json::parse(content);
		if(data.contains("items"))
		{
			json items = data["items"];
			if(items.size() > 0)//.contains(0))
			{
				json item = items[0];
				if(item.contains("snippet"))
				{
					json snippet = item["snippet"];
					if(snippet.contains("defaultLanguage"))
					{
						string defaultLanguage = snippet["defaultLanguage"];
						//string defaultLanguage = data["items"][0]["snippet"]["defaultLanguage"]; // crashed here for DaftPunk
						if(defaultLanguage == "fr")
						{
							return true;
						}
						else
						{
							log("defaultLanguage: " + defaultLanguage, threadsIndex);
						}
					}
				}
			}
		}

		bool isFrenchWikipedia = checkWikipedia(title, threadsIndex);
		if(isFrenchWikipedia)
			return true;
		else
			log("checkWikipedia 846 failed", threadsIndex); // why 846 ? ^^
	}
	return isCountryFrance;
}

unsigned int isAnyKeyWorking(unsigned int threadsIndex)
{
	unsigned int KEYSSize = KEYS.size();
	for(unsigned int KEYSIndex = 0; KEYSIndex < KEYSSize; KEYSIndex++)
	{
		string KEYSEl = KEYS[KEYSIndex],
		       url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=UCzYC9ss2P77Ry2LzIDL5Xsw&key=" + KEYSEl,
			   content = getHTTPS(url, threadsIndex);
		if(contains(content, "UUzYC9ss2P77Ry2LzIDL5Xsw"))
		{
			return KEYSIndex;
		}
	}
	return UNSIGNED_INT_MAX;
}

// maybe using a mutex would allow not two getHTTPS thread increasing too much the keyIndex
void changeKey(unsigned int threadsIndex)
{
	/*if(increaseKeyCalled)
	{
		log("change key being called, abort", threadsIndex);
		return;
	}
	increaseKeyCalled = true;
	log("i'm in", threadsIndex);*/
	unsigned int KEYSSize = KEYS.size();
	if(keyIndex >= KEYSSize)
	{
		unsigned int KEYSIndex = isAnyKeyWorking(threadsIndex);
		if(KEYSIndex != UNSIGNED_INT_MAX)
		{
			keyIndex = KEYSIndex;
			REAL_KEY = KEYS[KEYSIndex];
			log("keysIndex: " + convertNbToStr(KEYSIndex) + " still working !", threadsIndex); // could make a print like function just adding the "- N"
			return;
		}
		else
		{
			unsigned int waitSeconds = 3600 * 24 / KEYSSize;
			log("all keys exhausted for the moment let's wait " + convertNbToStr(waitSeconds) + " seconds...", threadsIndex);
			sleep(waitSeconds);
			log("wake up", threadsIndex);
			changeKey(threadsIndex);
			return;
			//log("all keys exhausted", threadsIndex); // could just terminate thread if multithreaded and say on what youtube channel it was working in order to delete it and restart from it
			//exit(42);
		}
	}
	log("key " + convertNbToStr(keyIndex) + " consumed, increasing !", threadsIndex);
	keyIndex++;
	REAL_KEY = KEYS[keyIndex];
	//increaseKeyCalled = false;
}

string getVideosPlaylist(string youtuberId, unsigned int threadsIndex) // like below can get up to 50 uploads playlist ids from channels
{
	string url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=" + youtuberId + "&key=" + KEY,
           content = getHTTPS(url, threadsIndex),
           uploadsPlaylist = split(split(content, "\"uploads\": \"")[1], "\"")[0];
	return uploadsPlaylist;
}

string scrapVideosAPI(string nextPageToken, unsigned int callsIndex, vector<string>* videosIds, string uploadsPlaylist, unsigned int threadsIndex) // could check up to 50 channels if have any videos given their playlist ids use playlists if want to check, playlistItems can only have a single playlistId
{
	log("callsIndex playlist: " + convertNbToStr(callsIndex), threadsIndex);
	string url = "https://www.googleapis.com/youtube/v3/playlistItems?part=contentDetails&playlistId=" + uploadsPlaylist; // items[n]["status"]["itemCount"] gives playlist size
	if(nextPageToken != "")
		url += "&pageToken=" + nextPageToken;
	url += "&key=" + KEY;
	string content = getHTTPS(url, threadsIndex);
	json data = json::parse(content),
		 items = data["items"];
	if(data.contains("nextPageToken"))
		nextPageToken = data["nextPageToken"];
	unsigned short itemsSize = items.size();
	for(unsigned short itemsIndex = 0; itemsIndex < itemsSize; itemsIndex++)
	{
		json item = items[itemsIndex];
		string videoId = item["contentDetails"]["videoId"];
		videosIds->push_back(videoId);
	}
	return nextPageToken;
}

vector<string> getVideosAPI(string youtuberId, unsigned int threadsIndex)
{
	string uploadsPlaylist = getVideosPlaylist(youtuberId, threadsIndex),
		   originalPageToken = "ignition",
           nextPageToken = "";
    unsigned int callsIndex = 0;
	vector<string> videosIds;
	while(nextPageToken != originalPageToken)
    {
    	originalPageToken = nextPageToken;
        nextPageToken = scrapVideosAPI(nextPageToken, callsIndex, &videosIds, uploadsPlaylist, threadsIndex);
        callsIndex++;
    }
	return videosIds;
}

vector<string> getVideosYTDL(string youtuberId, unsigned int threadsIndex)
{
	string uploadsPlaylist = getVideosPlaylist(youtuberId, threadsIndex),
           cmd = "youtube-dl -j --flat-playlist \"https://www.youtube.com/playlist?list=" + uploadsPlaylist + "\" | jq -r '.id'",
           idsStr = exec(cmd, threadsIndex);
    unsigned int idsStrLength = idsStr.length();
	if(idsStrLength > 0)
		idsStr = idsStr.substr(0, idsStrLength - 1);
	vector<string> ids = split(idsStr, "\n");
	unsigned int idsSize = ids.size();
	if(idsSize == 0)
		return getVideosAPI(youtuberId, threadsIndex); // in case of the voice fr like
	return ids;
}

bool checkWikipedia(string title, unsigned int threadsIndex)
{
    string titleReplaced = replaceAll(title, " ", "_"), // used to have escape here too
	       url = "https://fr.wikipedia.org/wiki/" + titleReplaced,
           content = getHTTPS(url, threadsIndex, false, false, true); // could also check for french language page - used to be en
		   
	// it's exceptionnal
	/*content = replaceAll(content, "\n");
	content = replaceAll(content, "\r");
	content = replaceAll(content, "<br/>"); // just in case you know
	content = replaceAll(content, "\t");
	content = replaceAll(content, " ");
	string countryPattern = "Paysd'origine</th><td><spanclass=\"datasortkey\"data-sort-value=\"France",
	       nationalityPattern = "Nationalité</th><td><spandata-sort-value=\"France";
	log("url: " + url + " !", threadsIndex);
	log("countryPattern: " + countryPattern + "|", threadsIndex);
	log("nationalityPattern: " + nationalityPattern + "|", threadsIndex);
	log("content: " + content + "|", threadsIndex);*/
	
    if(content != "404")
    {
        //if(contains(content, "Origin</th><td class=\"infobox-data\"><a href=\"/wiki/Paris\" title=\"Paris\">Paris"))
		content = replaceAll(content, "\n");
		content = replaceAll(content, "\r");
		content = replaceAll(content, "<br/>"); // just in case you know
		content = replaceAll(content, "\t");
		content = replaceAll(content, " ");
        //if(contains(content, "Pays d'origine </th><td><span class=\"datasortkey\" data-sort-value=\"France") || contains(content, "Nationalité </th><td><span data-sort-value=\"France"))
		string countryPattern = "Paysd'origine</th><td><spanclass=\"datasortkey\"data-sort-value=\"France",
	           nationalityPattern = "Nationalité</th><td><spandata-sort-value=\"France";
        if(contains(content, countryPattern) || contains(content, nationalityPattern))
        {
            return true;
        }
        else
        {
            log("url: " + url + " !"/* + ": " + content*/, threadsIndex); // Alonzo ambiguities
			log("countryPattern: " + countryPattern + "|", threadsIndex);
			log("nationalityPattern: " + nationalityPattern + "|", threadsIndex);
			log("content: " + content + "|", threadsIndex);
			return false; // doesn't used to be there
        }
    }
	else
	{
		log("checkWikipedia got 404", threadsIndex);
	}
	return false; // doesn't used to be there
}

string replaceAll(string str, const string& from, const string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

bool startsWith(string subject, string test)
{
    return !subject.compare(0, test.size(), test);
}

bool doesFileExist(string name)
{
    struct stat buffer;
    return stat(name.c_str(), &buffer) == 0;
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

unsigned long long getMillis(unsigned int threadsIndex)
{
    struct timeval tp;
    if(gettimeofday(&tp, NULL) == -1)
	{
		log("gettimeofday error", threadsIndex);
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

string getDate(unsigned int threadsIndex)
{
    time_t t = time(0);
    if(t == (time_t)(-1))
		log("time error", threadsIndex);
	struct tm *now = localtime(&t);

    unsigned long long ms = getMillis(threadsIndex);
    
	return getNbZero(now->tm_mday) + "-" + getNbZero(now->tm_mon + 1) + "-" + convertNbToStr<int>(now->tm_year - 100) + "#" + getNbZero(now->tm_hour) + "-" + getNbZero(now->tm_min) + "-" + getNbZero(now->tm_sec) + "-" + getNbZero(ms % 1000, 3);
}

bool writeFile(string filePath, string option, string toWrite, unsigned int threadsIndex)
{
    FILE* file = fopen(filePath.c_str(), option.c_str());
    if(file != NULL)
    {
        if(fputs(toWrite.c_str(), file) == EOF)
		{
			log("fputs error", threadsIndex);
		}
        if(fclose(file) != 0)
		{
			log("fclose error", threadsIndex);
		}
        return true;
    }
	else
	{
		log("fopen error", threadsIndex);
	}
    return false;
}

bool writeFile(string filePath, string option, vector<string> toWrite, unsigned int threadsIndex)
{
	return writeFile(filePath, option, toWrite, threadsIndex);
}

string toString(map<string, unsigned int> myMap)
{
	bool isFirst = true;
	string res = "";
	for(map<string, unsigned int>::iterator it = myMap.begin(); it != myMap.end(); it++)
	{
		if(!isFirst)
			res += "\n";
		isFirst = false;
		res += it->first + separator + convertNbToStr(it->second);
	}
	return res;
}

string toString(map<string, string>* myMap)
{
	bool isFirst = true;
	string res = "";
	for(map<string, string>::iterator it = myMap->begin(); it != myMap->end(); it++)
	{
		if(!isFirst)
			res += "\n";
		isFirst = false;
		res += it->first + separator + it->second;
	}
	return res;
}

string exec(string cmd, unsigned int threadsIndex)
{
    array<char, 128> buffer;
    string result = "";
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if(!pipe)
	{
        //throw runtime_error("popen() failed!");
		log("popen() failed !", threadsIndex);
	}
	else
		while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
			result += buffer.data();
	if(result == "")
	{
		// may have infinite loop if looking for empty website etc :S ?
		log("got empty result for cmd: " + cmd + " !", threadsIndex);
		return exec(cmd, threadsIndex);
	}
    return result;
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

// et je fais quoi moi quand il n'arrive même plus à un resolve un nom de domaine ?! "curl: (6) Could not resolve host: youtube.googleapis.com" bon redémarrons WSL - pas suffisant - j'ai du redémarrer windows u_u
string getHTTPS(string url, unsigned int threadsIndex, bool needFake, bool needFollow, bool doesWebpageExist, bool verbose)
{
	string originURL = url,
	       currentDate = getDate(threadsIndex);
	url = replaceLast(url, KEY, REAL_KEY);

	// may Alan Turing forgive me for using the shell that way
	// this escape seems useful only for wikipedia and soundcloud but let's use it there shouldn't be any escape of an escape
	string cmd = "curl -s -L '" + escape(url) + "'";//, // SSL bug or have to implement lock system which isn't easy with my code - disabling verification peer and host ?
	       //realCmd = cmd;
	if(doesWebpageExist)
	{
		cmd += " -I";
	}
	//log("before", threadsIndex);
	unsigned long long beforeTime = getMillis(threadsIndex);
	string content = exec(cmd, threadsIndex);
	/*if(content == "")
	{
		log("got content empty for cmd: " + cmd, threadsIndex);
	}*/
	//log("after", threadsIndex);
	if(verbose)
		log("request took " + convertNbToStr(getMillis(threadsIndex) - beforeTime) + " ms !", threadsIndex);
	costConsumed++; // could check if really requesting google api
	webRequests++;
	if(contains(content, TRANSIENT_ERROR_PATTERN))
	{
		log("transient error detected for url: " + url + ": content: " + content, threadsIndex);
		return getHTTPS(originURL, threadsIndex, needFake, needFollow, doesWebpageExist, verbose);
	}
	if(contains(content, BACKEND_ERROR_PATTERN))
	{
		log("backend error detected for url: " + url + ": content: " + content, threadsIndex);
		return getHTTPS(originURL, threadsIndex, needFake, needFollow, doesWebpageExist, verbose);
	}
	/*if(contains(content, API_KEY_NOT_VALID_PATTERN)) // maybe due to too long URL ? // no in fact a user was having "KEY" in his channel id u_u
	{
		log("api key not valid error detected for url: " + url + " originURL: " + originURL + " KEY: " + KEY + " REAL_KEY: " + REAL_KEY/* + ": content: " + content*//*, threadsIndex); // happen quite often so it would spam the well known same error message
		return getHTTPS(originURL, threadsIndex, needFake, needFollow, doesWebpageExist, verbose);
	}*/
	/*else
	{
		log("worked with url: " + url + " !", threadsIndex);
	}*/
	
	if(contains(content, PEPPA_PIG_PATTERN))
	{
		log("peppa pig like detected", threadsIndex);
	}
	else
	{
		bool isQuotaExceeded = contains(content, QUOTA_EXCEEDED_PATTERN),
		     isKeySuspended = contains(content, SUSPENSION_ERROR_PATTERN),
			 //isKeyIncorrect = contains(content, API_KEY_NOT_VALID_PATTERN), // it's a temporary error so have to treat it as this
		     isKeyNotUsable = isQuotaExceeded || isKeySuspended/* || isKeyIncorrect*/;
		if(isKeyNotUsable)
		{
			if(threadsIndex == 0)
			{
				if(isKeySuspended)
				{
					log("key is suspended !", threadsIndex); // could also make a set of keys which are unique per account (warning multiple NUSHAX name in accounts.txt while it should be different accounts)
				}
				log("need change key because got for url: " + url, threadsIndex);
				changeKey(threadsIndex);
			}
			else
			{
				log("gonna sleep", threadsIndex);
				sleep(1);
			}
			return getHTTPS(originURL, threadsIndex, needFake, needFollow, doesWebpageExist, verbose);
		}
	}
	// else might be all other normal cases
	if(doesWebpageExist)
	{
		if(!contains(content, "HTTP/1.1 200 OK") && !contains(content, "HTTP/2 200"))
		{
			log("webpage: " + url + " " + originURL + " doesn't return 200 !", threadsIndex);
			return "404";
		}
		else
		{
			//log("checking again originURL: " + originURL + " url: " + url + " content: " + content, threadsIndex);
			return getHTTPS(originURL, threadsIndex, needFake, needFollow, false/*doesWebpageExist*/, verbose);
			//return content;
		}
	}
	return content;

	CURL* curl = curl_easy_init();
    if(curl == NULL)
	{
		log("curl_easy_init error", threadsIndex);
		return "";
	}
	string got;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if(needFake)
	{
		// was originally designed for youtube fake
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko");
		//curl_easy_setopt(curl, CURLOPT_COOKIE, "CONSENT=YES+");
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0");
	}
	if(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0) != CURLE_OK)
	{
		log("curl_easy_setopt CURLOPT_SSL_VERIFYPEER error", threadsIndex);
		return "";
	}
    if(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0) != CURLE_OK) // used to be 1
	{
		log("curl_easy_setopt CURLOPT_SSL_VERIFYHOST error", threadsIndex);
		return "";
	}
	//if(needFollow) // may be necessary sometimes so let's use it everytime if doesn't change other cases ^^
	{
		if(curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L) != CURLE_OK)
		{
			log("curl_easy_setopt CURLOPT_FOLLOWLOCATION error", threadsIndex);
			return "";
		}
	}
    if(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback) != CURLE_OK)
	{
		log("curl_easy_setopt CURLOPT_WRITEFUNCTION error", threadsIndex);
		return "";
	}
    if(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &got) != CURLE_OK)
	{
		log("curl_easy_setopt CURLOPT_WRITEDATA error", threadsIndex);
		return "";
	}
	log("before", threadsIndex);
	CURLcode code = curl_easy_perform(curl);
    if(code != CURLE_OK) // 304 ms while other stuff at start take 18 ms - seems to have stuck here for 5 minutes at least cf 150821 4:29 AM in curl_multi_wait
	{
		log("curl_easy_perform error " + url + " " + currentDate + " !", threadsIndex); // https://curl.se/libcurl/c/CURLOPT_TIMEOUT.html
		const char* error = curl_easy_strerror(code);
		//printf("!%s!");
		log("error (" + convertNbToStr(code) + "): " + error + " !", threadsIndex);
		curl_easy_cleanup(curl);
		return /*""*/getHTTPS(url, threadsIndex, needFake, needFollow, doesWebpageExist); // ça ne marche pas en boucle comme ça :'( que ce soit SSL ou hostname resolution
	}
	log("after", threadsIndex);
    long responseCode = UNSIGNED_LONG_MAX;
	if(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode) != CURLE_OK)
	{
		log("curl_easy_getinfo error", threadsIndex);
		return "";
	}
	if(responseCode != 200)
    {
        //log("responseCode: " + convertNbToStr(responseCode) + " (" + url + ")", threadsIndex); // if reverse-engineer this algorithm, someone could just put "KEY" in the URL of its youtube channel and get my youtube data api v3 key ^^'
        //log("got: " + got, threadsIndex);
		if(doesWebpageExist)
		{
			log("webpage: " + url + " " + originURL + " doesn't return 200 !", threadsIndex);
			return "404";
		}
        if(!contains(got, PEPPA_PIG_PATTERN)) // second condition is required for peppa pig channel
        {
            if(threadsIndex == 0)
			{
				log("need change key because got " + convertNbToStr(responseCode) + " for " + url/* + ": " + got*/, threadsIndex);
                changeKey(threadsIndex);
			}
            else
                sleep(1);
            return getHTTPS(originURL, threadsIndex);
        }
        else
        {
            log("peppa pig like detected", threadsIndex);
        }
    }
    /*else
    {
        if(needFollow && startsWith(url, "https://www.facebook.com/"))
        {
            char* url = NULL;
            curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
            string urlStr = url;
            log("urlStr: " + urlStr + " !", threadsIndex);
        }
    }*/
	curl_easy_cleanup(curl); // no return value
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

string reverseStr(string str)
{
	reverse(str.begin(), str.end());
	return str;
}

string replaceLast(string subject, string search, string replaceS)
{
    string subjectReversed = reverseStr(subject),
           searchReversed = reverseStr(search),
           replaceSReversed = reverseStr(replaceS),
           subjectReversedReplaced = replace(subjectReversed, searchReversed, replaceSReversed/*search, replaceS*/),
           subjectReversedReplacedReversed = reverseStr(subjectReversedReplaced);
    return subjectReversedReplacedReversed;
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

void print(string s, unsigned int threadsIndex)
{
	cout << getDate(threadsIndex) << ": " << s << endl;
}

void log(string s, unsigned int threadsIndex)
{
	print(s + " - " + convertNbToStr(threadsIndex));
}

string escape(string url)
{
	return replaceAll(url, "'", "%27");
	//return replaceAll(url, "'", "\\'"); // \' doesn't do the job :'( two times crashed because of this
	// wait \\' puts really both slashes for real u_u maybe because of double escape, could make a check here that it's not already the case
}

bool removeFile(string filePath)
{
    return remove(filePath.c_str());
}

string join(vector<string> parts, unsigned int i, int j, string delimiter) // j excluded
{
    if(j == -1)
        j = parts.size();
    string res = "";
    for(unsigned int part = i; part < j; part++)
    {
        res += parts[part];
        if(part < j - 1)
            res += delimiter;
    }
    return res;
}

vector<vector<string>> slicing(vector<string>* vec, unsigned int sliceSize)
{
	vector<vector<string>> res;
	vector<string> workingOn;
	unsigned int vecSize = vec->size();
	for(unsigned int vecIndex = 0; vecIndex < vecSize; vecIndex++)
	{
		string el = vec->at(vecIndex);
		if(workingOn.size() >= sliceSize)
		{
			res.push_back(workingOn);
			workingOn.clear();
		}
		workingOn.push_back(el);
	}
	if(!workingOn.empty())
		res.push_back(workingOn);
	return res;
}

int convertStrToInt(string str)
{
    int number;
    sscanf(str.c_str(), "%d", &number);
    return number;
}

