#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <algorithm>
//#include "cvplot.h"
//#include "cvplot.cpp"
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

vector<Mat> hist (Mat src, int ii = -1){
	Mat dst;

  /// Separate the image in 3 places ( B, G and R )
  vector<Mat> bgr_planes, hist_result;
  split( src, bgr_planes );

  /// Establish the number of bins
  int histSize = 256;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, 256 } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat b_hist, g_hist, r_hist;

  /// Compute the histograms:
  calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
  
  // Draw the histograms for B, G and R
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 8, 0  );
  }

  /// Display
  string name;
  if (ii == -1)
  	name = "Img Window";
  else
  	name = ii;
  //namedWindow(name, CV_WINDOW_AUTOSIZE );
  //imshow(name, histImage );

  hist_result.push_back(r_hist);
  hist_result.push_back(g_hist);
  hist_result.push_back(b_hist);
  return hist_result;
}

int main (int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Insufficient number of arguments." << endl;
    }

    VideoCapture cap(argv[1]);
    if (!cap.isOpened()) return -1;
    int temp = 1;
    namedWindow ("image",1);
    Mat frame, frame2, diif_frame;
    vector<Mat> old_hist,new_hist;
    vector<double> diff_sum;
    for(int ii = 0;ii < cap.get(CV_CAP_PROP_FRAME_COUNT); ii+=8) {
        // set video frame to ii.
        vector<Mat> diff_hist;
        cap.set (CV_CAP_PROP_POS_FRAMES, ii);

        if (ii != 0){
        	frame2 = frame;
        	old_hist = new_hist;
        }
		cap >> frame;
        new_hist = hist(frame);
        if (ii != 0){
        	diff_hist.push_back(abs(new_hist[0] - old_hist[0]));
        	diff_hist.push_back(abs(new_hist[1] - old_hist[1]));
			diff_hist.push_back(abs(new_hist[2] - old_hist[2]));
			double sum = cv::sum(diff_hist[0])[0] + cv::sum(diff_hist[1])[0] + cv::sum(diff_hist[2])[0];
			diff_sum.push_back(sum);
        	diif_frame = frame2 - frame;
        	cout << sum << endl;
        	int width = ceil(cap.get(CV_CAP_PROP_FRAME_COUNT)/8);
        	if (sum >= 26000){
        		imshow("image",frame);
        	}
        	/*CvPlot::plot("RGB", diff_hist[0], width,3);
			CvPlot::label("B");
			CvPlot::plot("RGB", diff_hist[1], width, 3, 255, 0, 0);
			CvPlot::label("G");
			CvPlot::plot("RGB", diff_hist[2], width, 3, 0, 0, 255);
			CvPlot::label("R");
			CvPlot::plot("RGB", sum, width, 3, 100, 100, 100);
			CvPlot::label("sum");*/
        }
        //cout << histogram[0] << endl;
        //imshow ("image", frame2);
        if (waitKey(1) >= 0) break;
    }
    int w = diff_sum.size();
    cout << "----------------------------------------------" << endl;
    cout << *max_element(diff_sum.begin(),diff_sum.end()) << endl;
    cout << *min_element(diff_sum.begin(),diff_sum.end()) << endl;
    cout << (accumulate(diff_sum.begin(),diff_sum.end(),0.0))/diff_sum.size() << endl;
    //waitKey(40000);
    return 0;
}
