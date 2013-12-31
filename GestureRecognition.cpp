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

#include "opencv2/video/tracking.hpp"

#include "GestureRecognitionImage.h"

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );
void findFace( Mat frame );
void findHand( Mat frame );
void caculateHandHistogram();
void showHand( Mat showFrame);
void getHistogram( Mat frame);

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
Mat mask;

Mat handMat;

GestureRecognitionImage gestureRecognizer;

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
int main( void )
{
    CvCapture* capture;
    Mat frame;
    int input = -1;
    
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
            int tempInput = waitKey(10);
            if (tempInput == 'h' || tempInput == 'c' || tempInput == 'f' || tempInput == 'b' || tempInput == 's' || tempInput == 'g') {
                input = tempInput;
            }
            printf("tempInput is %c\n",tempInput);
            
            //-- 3. Apply the classifier to the frame
            if( !frame.empty())
            {
                if( (char)input == 'b' ) { break; }
                if( (char)input == 'f' )
                {
                    findFace(frame);
                }
                if ( (char)input == 'g') {
                    getHistogram(frame);
                }
                //计算手的histogram
                if ( (char)input == 'c') {
                    caculateHandHistogram();
                }
                if ( (char)input == 's'){
                    showHand(frame);
                }
                else {
                    findHand(frame);
//                    detectAndDisplay( frame );
                }
            }
            else
            { printf(" --(!) No captured frame -- Break!"); break; }
        }
    }
    return 0;
}

void findHand(Mat frame)
{
    Mat copyFrame = frame.clone();
    blur( copyFrame, copyFrame, Size(3,3) );
    Canny(copyFrame, copyFrame, 50, 150, 3);
    vector<vector <Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(copyFrame, contours , hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1);
    double maxArea(0);
    int maxIndex = 0;
    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > maxArea) {
            maxArea = area;
            maxIndex = i;
        }
    }
    cv::Scalar color( 255, 255, 255);
    cv::Rect bRect = cv::boundingRect( cv::Mat(contours[maxIndex]) );
    rectangle(copyFrame, bRect, color);
    handMat = frame(bRect);
    imshow("handImage", copyFrame);
}

void getHistogram(Mat frame)
{
    Mat hue,mask;
    Mat hsv;
    
    double smin = 70;
    double _vmin = 80;
    double _vmax = 256;
    
    cvtColor( frame, hsv, CV_BGR2HSV );
    
    inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
            Scalar(180, 256, MAX(_vmin, _vmax)), mask);
    
    /// Use only the Hue value
    hue.create( hsv.size(), hsv.depth() );
    int ch[] = { 0, 0 };
    mixChannels( &hsv, 1, &hue, 1, ch, 1 );
    
    Rect roiRect = Rect(frame.cols/2 - 50,frame.rows/2 - 50,100,100);
    Mat roiMat = hue(roiRect);
    Mat maskRoi = mask(roiRect);
    rectangle(frame, roiRect, Scalar(0,0,0));
    
    calcHist(&roiMat, 1, 0, maskRoi, hist, 1, &hsize, &phranges);
    normalize(hist, hist,0,255,CV_MINMAX);
    
//    calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
//    backproj &= mask;
//    
//    RotatedRect trackBox = CamShift(backproj, roiRect,
//                                    TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));
//    ellipse(frame, trackBox, Scalar(0,0,0));
}

void caculateHandHistogram()
{
    Mat hue,mask;
    Mat hsv;
    
    double smin = 70;
    double _vmin = 80;
    double _vmax = 256;
    
    cvtColor( handMat, hsv, CV_BGR2HSV );
    
    inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
            Scalar(180, 256, MAX(_vmin, _vmax)), mask);
    
    /// Use only the Hue value
    hue.create( hsv.size(), hsv.depth() );
    int ch[] = { 0, 0 };
    mixChannels( &hsv, 1, &hue, 1, ch, 1 );

    calcHist(&hue, 1, 0, mask, hist, 1, &hsize, &phranges);
    normalize(hist, hist,0,255,CV_MINMAX);
}

void showHand(Mat frame)
{
    Mat hue,mask;
    Mat hsv;
    
    double smin = 70;
    double _vmin = 80;
    double _vmax = 256;
    
    cvtColor( frame, hsv, CV_BGR2HSV );
    
    inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
            Scalar(180, 256, MAX(_vmin, _vmax)), mask);
    
    /// Use only the Hue value
    hue.create( hsv.size(), hsv.depth() );
    int ch[] = { 0, 0 };
    mixChannels( &hsv, 1, &hue, 1, ch, 1 );

    saveImage = frame;
    calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
    backproj &= mask;
    
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    
    cv::findContours(backproj, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1);
    
    int massMin = 4000;
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
            Mat contourMat = backproj(bRect);
            HandStruct hand = gestureRecognizer.caculateOneImage(contours[idx], contourMat);
            if (hand.crossNum >= 3) {
                rectangle(backproj, bRect,color);
            } else if(hand.crossNum == 1) {
                line(backproj, Point2f(bRect.x,bRect.y), Point2f(bRect.x+bRect.width,bRect.y), color);
                line(backproj, Point2f(bRect.x+bRect.width,bRect.y), Point2f(bRect.x+bRect.width/2,bRect.y + bRect.height), color);
                line(backproj, Point2f(bRect.x+bRect.width/2,bRect.y + bRect.height), Point2f(bRect.x,bRect.y), color);
            } else if(hand.crossNum == 0) {
                circle(backproj, center, bRect.size().width, color);
            }
            printf("peakNum is %d, valley is %d\n",hand.peakNum,hand.valleyNum);
        }
    }
    
    imshow(window_name, backproj);
}

void findFace(Mat frame)
{
    std::vector<Rect> faces;
    Mat frame_gray;
    Mat hue,mask;
    Mat hsv;
    
    double smin = 70;
    double _vmin = 80;
    double _vmax = 256;
    
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    cvtColor( frame, hsv, CV_BGR2HSV );
    
    inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
            Scalar(180, 256, MAX(_vmin, _vmax)), mask);
    
    /// Use only the Hue value
    hue.create( hsv.size(), hsv.depth() );
    int ch[] = { 0, 0 };
    mixChannels( &hsv, 1, &hue, 1, ch, 1 );
    
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
}

/**
 * @function detectAndDisplay
 */
void detectAndDisplay( Mat frame )
{
    //-- Detect faces
    if (!isFindFace) {
        findFace(frame);
    }
    else {
        showHand(frame);
    }
}
