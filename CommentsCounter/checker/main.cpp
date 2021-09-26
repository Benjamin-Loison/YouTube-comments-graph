#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

vector<string> getFileContent(string path);

int main()
{
	string youtuberId = "UCpWaR3gNAQGsX48cIlQC0qw",
	       path = /*"/mnt/f/YouTube/CommentsGraph/details/"*/"/mnt/c/Users/Benjamin/Desktop/BensFolder/DEV/Candco/CPP/Projects/YouTubeCommentsGraph/CommentsCounter/checker/fastData/" + youtuberId + ".txt";
	/*vector<string> lines = getFileContent(path);
	unsigned int linesSize = lines.size();
	cout << linesSize << endl;*/

	return 0;
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
