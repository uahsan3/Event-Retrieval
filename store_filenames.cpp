#include <dirent.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>


using namespace std;

int main(void)
{
	unsigned char isFile = 0x8;
	DIR *d;
	struct dirent *dir;
	vector<string> dirlist;
	int i = 0;
	d = opendir("./Dataset_few/");

	if(d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			i++;
			dirlist.push_back(dir->d_name);
		}
		
		for (int i=0; i<dirlist.size();i++)
			cout << dirlist[i] << endl;
		ofstream output_file("./video_files.txt");
		ostream_iterator<string> output_iterator(output_file, "\n");
		copy(dirlist.begin(), dirlist.end(), output_iterator);
		closedir(d);
	}

	return(0);
}
