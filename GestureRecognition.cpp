//
//  GestureRecognition.cpp
//  MyGestureRecognition
//
//  Created by yeahugo on 13-12-14.
//
//

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
String face_cascade_name = "haarcascade_profileface.xml";
String eyes_cascade_name = "haarcascade_frontalface_default.xml";
String palm_cascade_name = "palm.xml";
String fist_cascade_name = "fist.xml";
String cross_cascade_name = "lbpcascade_cross.xml";

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier palm_cascade;
CascadeClassifier cross_cascade;
CascadeClassifier fist_cascade;

string window_name = "Capture - Face detection";

RNG rng(12345);
bool isFindFace;
Mat hist;
Mat histimg = Mat::zeros(200, 320, CV_8UC3);
Mat backproj;
Mat saveImage;

const char* trackbar_name = "on_or_off" ;

float hranges[] = {0,180};
int hsize = 16;
const float* phranges = hranges;

void on_or_off(int pos)
{
    if ( 0 == pos  )
	{
        cv::imshow("saveImage", saveImage);
	}
	if( 1 == pos )
	{
        IplImage saveIplImage = saveImage.operator _IplImage();
        cvSaveImage("saveImage1.jpg", &saveIplImage);
//        imwrite("saveImage.jpg", saveImage);
	}
}
/**
 * @function main
 */
int main3( void )
{
    CvCapture* capture;
    Mat frame;
    
    isFindFace = false;
    //-- 1. Load the cascade
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if( !palm_cascade.load( palm_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if (!cross_cascade.load(cross_cascade_name)) {printf("--(!)Error loading\n"); return -1; };
    if (!fist_cascade.load(fist_cascade_name)) { printf("--(!)Error loading\n"); return -1;}
    
    //-- 2. Read the video stream
    capture = cvCaptureFromCAM( -1 );
    if( capture )
    {
        for(;;)
        {
            frame = cvQueryFrame( capture );
            
            //-- 3. Apply the classifier to the frame
            if( !frame.empty())
            {
                detectAndDisplay( frame );
            }
            else
            { printf(" --(!) No captured frame -- Break!"); break; }
            
            int c = waitKey(10);
            if( (char)c == 'c' ) { break; }
            
        }
    }
    return 0;
}

/**
 * @function detectAndDisplay
 */
void detectAndDisplay( Mat frame )
{
    std::vector<Rect> faces;
    Mat frame_gray;
    Mat hue,mask;
    Mat hsv;
    
    double smin = 50;
    double _vmin = 80;
    double _vmax = 256;
    
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
//    equalizeHist( frame_gray, frame_gray );
    
    cvtColor( frame, hsv, CV_BGR2HSV );
    
    inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
            Scalar(180, 256, MAX(_vmin, _vmax)), mask);
    
    /// Use only the Hue value
    hue.create( hsv.size(), hsv.depth() );
    int ch[] = { 0, 0 };
    mixChannels( &hsv, 1, &hue, 1, ch, 1 );
    
    //-- Detect faces
    if (!isFindFace) {
        face_cascade.detectMultiScale( frame_gray, faces);
        
        for( size_t i = 0; i < faces.size(); i++ )
        {
            Mat faceROI = frame_gray( faces[i] );
            std::vector<Rect> eyes;
            
            eyes_cascade.detectMultiScale( faceROI, eyes);
            if( eyes.size() > 0)
            {
                Mat roi(hue,faces[i]);
                Mat maskroi(mask,faces[i]);
                calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                normalize(hist, hist,0,255,CV_MINMAX);
                
                isFindFace = true;
            }
        }
    } else {
        saveImage = frame;
        calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
        backproj &= mask;
        cvtColor(backproj, backproj, CV_GRAY2BGR);
        
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        
        cv::cvtColor(backproj, backproj, CV_BGR2GRAY);
        
        cv::findContours(backproj, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1);
        
        int massMin = 5000;
        cv::Scalar color( 255, 255, 255 );
        
        std::vector<Rect> faces;
       
        int value = 0;
        cvCreateTrackbar(trackbar_name, window_name.c_str(), &value, 1,on_or_off);
        
        int idx = 0;
        for (; idx >= 0; idx = hierarchy[idx][0]) {
            cv::Moments mom = cv::moments(cv::Mat(contours[idx]));
            int cMass = mom.m00;
            Point center = Point2d(mom.m10/mom.m00,mom.m01/mom.m00);
            cv::Rect bRect = cv::boundingRect( cv::Mat(contours[idx]) );

            if (cMass > massMin) {
                cv::Mat contourImg(backproj.size(), backproj.type(), cv::Scalar(0));
                cv::drawContours(backproj, contours, idx, color,CV_FILLED,8,hierarchy);
            }
        }
        
        imshow(window_name, backproj);
    }
}
