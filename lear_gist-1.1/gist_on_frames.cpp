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
#include <algorithm>
#include <utility>

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

// function for sorting values of frame differences in descending order
typedef std::pair<float,int> mypair;
bool comparator ( const mypair& l, const mypair& r)
		   { return l.first < r.first; }

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
				int num_frames;	
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
				for (int i = 19; i != 20; i++) //VideoFilenames.size(); i++) 
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
								
								bool stop(false);
								capture.set (CV_CAP_PROP_FPS, 20); // set frame rate=1
								double frame_rate = capture.get (CV_CAP_PROP_FPS);
								cout << frame_rate << endl;
								num_frames = capture.get (CV_CAP_PROP_FRAME_COUNT);

								cv::Mat frame; // current video frame
								// cv::namedWindow ("Extracted frame");
							  vector<Mat> rgb; // vector for holding r g b values - of type Mat
								vector< vector<float> >  gist_desc; // a two dimensional vector where each row is a vector of float values holding the descriptor for each frame

								// for all frames in video capture the frame, compute  GIST on it and store in gist_desc
								// color_image_t *im;
								int k=0;

								cv::Mat prev_frame;	
								vector<std::pair < float, int > >  sum_of_diffs; // create a vector of pairs where each pair is the diffs and its corresponding frame number 
								
								while (!stop) 
								{
										k++;

										// read next frame if any
										if (!capture.read(frame))
										{
												cout << "Can't read frame!" << endl;
												break;
										}

										cv::Mat diff_frame;
										cv::Scalar sum_of_diff;

										if (k != 1) // ensuring that frame 1 is not included - thus used for frame differencing
										{
														absdiff(prev_frame, frame, diff_frame); // calculating element wise difference between the two frames
														sum_of_diff = sum(diff_frame); // adding the differences to give scalar (3 channels)
										
														// calculating sum of three channels 
														float sum = sum_of_diff.val[0] + sum_of_diff.val[1] + sum_of_diff.val[2];	
														// cout << sum  << " "; // printing out the sums of differences for each frame with previous frame (except 1)
														sum_of_diffs.push_back(make_pair(sum, k)); // fills a vector of values of sums of diffs
										}
										

									  // Calculating average pixel intensity of each channel of the frame	
										/*
										cv::Scalar avgPixelIntensity = cv::mean(frame);

										// convert to IplImage from cv::Mat	
										IplImage current_frame = frame;
										
									  // create an image that's the destination image (resized)	
										IplImage* gist_img = cvCreateImage(cvSize(GIST_SIZE, GIST_SIZE), IPL_DEPTH_8U, 3);

										float* desc = my_compute_gist(&current_frame, gist_img);
										vector<float> row; // create an empty row
										for (int j = 0; j < feature_vector_length; j++)
										{
												// Checking to see if the frame has average pixel values in each channel above a threshold [this is to filter out blank frames]
												if (avgPixelIntensity.val[0] > 20 && avgPixelIntensity.val[1] > 20 && avgPixelIntensity.val[2] > 20)
														row.push_back(desc[j]);
										}
                    if (avgPixelIntensity.val[0] > 20 && avgPixelIntensity.val[1] > 20 && avgPixelIntensity.val[2] > 20)
												gist_desc.push_back(row); // Add the row to the main vector
										cout << "One frame descriptor size: " <<  row.size() << "  " << "Total descriptor size: " << gist_desc.size() << endl;
										free(desc);
										*/

										// checking if its the last frame
										if (k == num_frames) {
												cout << num_frames << endl;
												break;
										}

										prev_frame = frame.clone();  // cloning frame in another cv::Mat structure to be compared for frame differencing

										
										//break;*/
								}

								// sorting the vector of pairs in descending order - each pair is difference value (with prev. frame) and the corresponding frame no.
								sort (sum_of_diffs.begin(), sum_of_diffs.end(), comparator);

								// iterating through the vector of pairs and printing out the values in it
								typedef std::vector<std::pair<float, int> > vector_type;
							  for (vector_type::const_iterator pos = sum_of_diffs.begin(); pos != sum_of_diffs.end(); ++pos)
										cout << pos -> first << " " << pos -> second << endl;
												


								/*

								// writing the descriptor to a text file - each text file for one video - each row in a text file corresponding to a frame
								string full_file_name = VideoFilenames[i] + ".txt";
								ofstream output_file(full_file_name.c_str());
								ostream_iterator<float> output_iterator(output_file, ",");
								for ( int i = 0 ; i < gist_desc.size() ; i++ ) 
								{
												copy(gist_desc.at(i).begin(), gist_desc.at(i).end(), output_iterator);
												output_file << "\n";
								}
								*/


								// close the video file
								capture.release();
				}

}

