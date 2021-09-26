#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

// Python is too slow

template<typename T>
string convertNbToStr(const T& number)
{
    ostringstream convert;
    convert << number;
    return convert.str();
}

void print(string s);
vector<string> getFileContent(string path);

string path = "/mnt/f/YouTube/CommentsGraph/details/";

int main()
{
	chdir(path.c_str());

	//vector<string> l;
	map<string, bool> l; // improves speed

	string filePath = "UCWeg2Pkate69NFdBeuRFTAw-names.txt";
	vector<string> lines = getFileContent(filePath);
	unsigned int linesSize = lines.size();
	print(convertNbToStr(linesSize));
	for(unsigned int linesIndex = 0; linesIndex < linesSize; linesIndex++)
	{
		if(linesIndex % 10000 == 0)
			print(convertNbToStr(linesIndex));
		string line = lines[linesIndex];
		if(l.find(line) == l.end())
			l[line] = true;
		//if(find(l.begin(), l.end(), line) == l.end())
		//	l.push_back(line);
	}

	filePath = "cppmap.txt";
    string option = "w";
	unsigned int lSize = l.size();
	print(convertNbToStr(lSize));
	FILE* file = fopen(filePath.c_str(), option.c_str());
    if(file != NULL)
    {
		/*for(unsigned int lIndex = 0; lIndex < lSize; lIndex++)
		{
			string lEl = l[lIndex];
			if(lIndex < lSize - 1)
				lEl += "\n";
        	if(fputs(lEl.c_str(), file) == EOF)
				print("fputs error");
		}*/
		unsigned int lIndex = 0;
		for(map<string, bool>::iterator it = l.begin(); it != l.end(); it++)
		{
			string toWrite = it->first;
			if(lIndex < lSize - 1)
				toWrite += "\n";
			if(fputs(toWrite.c_str(), file) == EOF)
				print("fputs error");
			lIndex++;
		}
        if(fclose(file) == EOF)
			print("fclose error");
    }
	else
		print("fopen error");

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

void print(string s)
{
	cout << s << endl;
}
