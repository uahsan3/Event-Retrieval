// reading a file
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
	string line;
	ifstream myfile ("./annots/egypt+Tahrir+Square+protestors.dat");
	string videoURLPrefix = "http://www.youtube.com/watch?v=";

	if (myfile.is_open())
	{
		while (myfile.good())
		{
			string videoURL = videoURLPrefix;
			getline (myfile, line);
			cout << line << endl;
			videoURL.append (line, 0, 11);
		//	cout << videoURL << endl;
		
		}
		myfile.close();
	}

	else cout << "Unable to open file";
	return 0;
}

