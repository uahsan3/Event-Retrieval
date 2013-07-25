#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>

using namespace cv;
using namespace std;

int main()
{
	string line;
	vector<string> VideoFilenames;
	vector<string> VideoFilenames_final;
	
	ifstream myfile("video_files.txt");

	if(!myfile)
	{
			cout << "Error opening file!" << endl;
			system("pause");
			return -1;
	}

	while (getline(myfile, line))
	{
			VideoFilenames.push_back(line);
	}

	for (int i = 0; i < VideoFilenames.size(); i++)
	{
					if (VideoFilenames[i].size() == 15)
									VideoFilenames_final.push_back(VideoFilenames[i]);
	}
	ofstream f("./video_files_final.txt");
	for (vector<string>::const_iterator i = VideoFilenames_final.begin();
									i != VideoFilenames_final.end(); ++i)
					f << *i << '\n';

	// open the video file
	for(int i = 2; i <= VideoFilenames_final.size(); i++)
	{
			string full_path = "./Dataset_few/" + VideoFilenames_final[i];
			cv::VideoCapture capture(full_path);

		// check if video opened successfully
		if (!capture.isOpened())
			{
				cout << "Cannot open the video file" << endl;
				return 1;
			}
		// Get the frame rate
		double frame_rate = capture.get(CV_CAP_PROP_FPS);
		bool stop(false);
		capture.set(CV_CAP_PROP_FPS, 15); // set frame rate to 15fps

		cv::Mat frame; // current video frame
		cv::namedWindow("Extracted Frame");

		// Delay between each frame in ms corresponds to video frame rate
		int delay = 1000/frame_rate;
		
		// for all frames in video
		while (!stop) {
			// read next frame if any
			if (!capture.read(frame))
				break;
			cv::imshow("Extracted Frame", frame);

			// introuduce a delay or press a key to stop
			if (cv::waitKey(delay) >= 0)
				stop = true;
			}
				
			// close the video file
			capture.release();
	}
}
