#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
extern "C" {
		#include "gist.h"
		#include "standalone_image.h"
}
#include <cmath>
#include "gist-classifier.hpp"

using namespace cv;
using namespace std;

const static int GIST_SIZE = 128;
const static int feature_vector_length = 960;
const static int nblocks = 4;
const static int n_scale = 3;
const static int orientations_per_scale[50]={8,8,4};

/* Convert OpenCV IplImage into LEAR's desired color_image_t format. */
/* Direct access using a c++ wrapper using code found under topic
	    "Simple and efficient access" at link:*/
/* http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00053000000000000000*/
// Source: http://www.cs.cornell.edu/~yuhsin/opencv-stuff/gist-classifier.cpp
void convert_IplImage_to_LEAR(IplImage* src, color_image_t* dst) {
		  assert(src->width == GIST_SIZE && src->height == GIST_SIZE);
		  assert(src->depth == IPL_DEPTH_8U);
		  RgbImage imgA(src);
		  int x, y, i = 0;
		  for (y = 0; y < GIST_SIZE; y++) {
					for (x = 0; x < GIST_SIZE; x++) {
							dst->c1[i] = imgA[y][x].r;
				      dst->c2[i] = imgA[y][x].g;
							dst->c3[i] = imgA[y][x].b;
							i++;
				  }
		  }
			assert(i == (GIST_SIZE * GIST_SIZE));
}

/*
	  * Computes the GIST descriptor for a copy of img resized to 128x128
		 * (GIST_SIZE x GIST_SIZE).
		  */
float* my_compute_gist(IplImage* img, IplImage* rsz) {
		/* Resize image to 128x128 before calculating GIST descriptor. */
		assert(img);
	  assert(rsz);
		cvResize(img, rsz, CV_INTER_LINEAR);
		/* Lear's compute_gist takes a ppm and computes gist descriptor for it. */
		color_image_t *lear = color_image_new(GIST_SIZE, GIST_SIZE);
		assert(lear);
		convert_IplImage_to_LEAR(rsz, lear);
		float *desc = color_gist_scaletab(lear, nblocks, n_scale, orientations_per_scale);
		/* Cleanup. */
		color_image_delete(lear);
		return desc;
} 



int main()
{
				
				// Read filenames from text file into a vector of strings
				vector<string> VideoFilenames;
				ifstream myfile("/home/unaiza/Dropbox/EVVEstuff/video_files_final.txt");
				if (!myfile)
				{
								cout << "Error opening file!" << endl;
								system("pause");
								return -1;
				}
				string line;
				while (getline(myfile, line))
								VideoFilenames.push_back(line);
				
				
				// Open the video file
				// for (int i = 0; i <= VideoFilenames.size(); i++)
				for (int i = 0; i <= 0; i++)
				{
								string full_path = "../Dataset_few/" + VideoFilenames[i];
								cout << full_path << endl;
								cv::VideoCapture capture(full_path);

								// check if the video opened successfully
								if (!capture.isOpened())
								{
												cout << "Cannot open video file" << endl;
												return 1;
								}
								
								// get the frame rate
								double frame_rate = capture.get (CV_CAP_PROP_FPS);
								bool stop(false);
								capture.set (CV_CAP_PROP_FPS, 15); // set frame rate=15

								cv::Mat frame; // current video frame
								// cv::namedWindow ("Extracted frame");
							  vector<Mat> rgb; // vector for holding r g b values - of type Mat
								vector<float> gist_desc; // vector for storing descriptors for each frame [one column per frame]

								// for all frames in video
								// color_image_t *im;
								int k = 0;
								while (!stop) 
								{
												k++;
												// read next frame if any
												if (!capture.read(frame))
												{
																cout << "Can't read frame!" << endl;
																break;
												} else { cout << "frame captured!" << k << endl; }


												// convert to IplImage from cv::Mat	
												IplImage current_frame = frame;

												// trying out GIST only on frame 150
												if (k == 15)
												{
																IplImage* gist_img = cvCreateImage(cvSize(GIST_SIZE, GIST_SIZE), IPL_DEPTH_8U, 3);
																float* desc = my_compute_gist(&current_frame, gist_img);
																/* Should contain feature_vector_length (960) float values */
																for (int i = 0; i < feature_vector_length; i++) {
																		gist_desc.push_back(desc[i]);
																		printf("%d: %f\n", i, gist_desc[i]);

																}
																free(desc);
															 // cvReleaseImage(&current_frame);
																break;
												}



												
										  	/*if (k == 15) 
												{
																split(frame, rgb);
																namedWindow("b");
																namedWindow("g");
																namedWindow("r");
															  imshow("b", rgb[0]);
																imshow("g", rgb[1]);
																imshow("r", rgb[2]);
																waitKey(0);
																cout << "frame size : " << frame.size().width << ", " << frame.size().height << endl;
																cout << "R size : " << rgb[2].size().width << ", " << rgb[2].size().height << endl;
																cout << "G size : " << rgb[1].size().width << ", " << rgb[1].size().height <<  endl;
																cout << "B size : " << rgb[0].size().width << ", " << rgb[0].size().height <<  endl;
																break;
												}*/
												   // waitKey(0);
															//	namedWindow("Frame 0", CV_WINDOW_AUTOSIZE);
															//	imshow("Frames", frame);
															//	waitKey();//without this image won't be shown
														//return 0;//OK
							 
												
												// insert code to calculate GIST descriptor on frames
								}
				
			  // close the video file
				capture.release();
				}

}
								
