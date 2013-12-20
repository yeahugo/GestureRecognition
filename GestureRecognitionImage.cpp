//
//  GestureRecognitionImage.cpp
//  MyGestureRecognition
//
//  Created by yeahugo on 13-12-16.
//
//

#include "GestureRecognitionImage.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "slic.h"

#include "GraphUtils.h"

#include <iostream>
#include <sys/types.h>
#include <dirent.h>

using namespace cv;
using namespace std;

struct ContourPoint
{
    int index;
    float angle;
    float value;
    float derivative;
    float dotValue;
    float crossValue;
    int isUsed;
} ;

string dirString = "hand/";
string toDirString = "newHand/";
string cannyString = "cannyImage/";
string failString = "failImage/";

String face_profile_name = "haarcascade_profileface.xml";
String face_front_name = "haarcascade_frontalface_default.xml";

string sourceFileName;

CascadeClassifier face_profile_cascade;
CascadeClassifier face_front_cascade;

Mat handFrame;

float hisRanges[] = {0,180};
int hisSize = 16;
const float* ranges = hisRanges;

vector<Point > contour;
Mat contourMat;

void caculateBackProject(Mat frame_hue,Mat hist,Mat frame_mask)
{
    Mat backproj;
    calcBackProject(&frame_hue, 1, 0, hist, backproj, &ranges);
    backproj &= frame_mask;
    cvtColor(backproj, backproj, CV_GRAY2BGR);
    
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    
    cv::cvtColor(backproj, backproj, CV_BGR2GRAY);
    
    cv::findContours(backproj, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1);
    
    int massMin = 15000;
    cv::Scalar color( 255, 255, 255 );
    
    int idx = 0;
    for (; idx >= 0; idx = hierarchy[idx][0]) {
        cv::Moments mom = cv::moments(cv::Mat(contours[idx]));
        int cMass = mom.m00;
        cv::Rect bRect = cv::boundingRect( cv::Mat(contours[idx]) );
        
        if (cMass > massMin) {
            cv::Mat contourImg(backproj.size(), backproj.type(), cv::Scalar(0));
            contourImg = backproj(bRect);
            contourImg.copyTo(handFrame);
            contour = contours[idx];
//            imshow("handFrame", handFrame);
            break;
        }
    }
}

//肤色提取，skinArea为二值化肤色图像
void skinExtract(const Mat &frame, Mat &skinArea)
{
	Mat YCbCr;
	vector<Mat> planes;
    
    blur( frame, frame, Size( 4, 4 ), Point(-1,-1));
    
	//转换为YCrCb颜色空间
	cvtColor(frame, YCbCr, CV_RGB2YCrCb);
	//将多通道图像分离为多个单通道图像
	split(YCbCr, planes);
    
	//运用迭代器访问矩阵元素
	MatIterator_<uchar> it_Cb = planes[1].begin<uchar>(),
    it_Cb_end = planes[1].end<uchar>();
	MatIterator_<uchar> it_Cr = planes[2].begin<uchar>();
	MatIterator_<uchar> it_skin = skinArea.begin<uchar>();
    
	//人的皮肤颜色在YCbCr色度空间的分布范围:100<=Cb<=127, 138<=Cr<=170
	for( ; it_Cb != it_Cb_end; ++it_Cr, ++it_Cb, ++it_skin)
	{
		if (138 <= *it_Cr &&  *it_Cr <= 170 && 100 <= *it_Cb &&  *it_Cb <= 127)
			*it_skin = 255;
		else
			*it_skin = 0;
	}
    
	//膨胀和腐蚀，膨胀可以填补凹洞（将裂缝桥接），腐蚀可以消除细的凸起（“斑点”噪声）
	dilate(skinArea, skinArea, Mat(10, 10, CV_8UC1), Point(-1, -1));
	erode(skinArea, skinArea, Mat(10, 10, CV_8UC1), Point(-1, -1));
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //寻找轮廓
    Mat copyContour;
    skinArea.copyTo(copyContour);
    
    findContours(copyContour, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    
    // 找到最大的轮廓
    int index;
    double area, maxArea(0);
    for (int i=0; i < contours.size(); i++)
    {
        area = contourArea(Mat(contours[i]));
        if (area > maxArea)
        {
            maxArea = area;
            index = i;
        }
    }
    
    cv::Scalar color( 100, 150, 255 );
    cv::drawContours(copyContour, contours, index, color,CV_FILLED,8,hierarchy);
    
    cv::Rect bRect = cv::boundingRect( cv::Mat(contours[index]) );
    contourMat = skinArea;
//    contourMat = skinArea(bRect);
    blur( contourMat, contourMat, Size(3,3) );
    Canny(contourMat, contourMat, 50, 150, 3);
    contour = contours[index];
}


int findFace(Mat frame)
{
    std::vector<Rect> faces;
    
    Mat frame_gray,frame_hsv,frame_hue,frame_mask,hist;
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    cvtColor( frame, frame_hsv, CV_BGR2HSV );

    frame_hue.create( frame_hsv.size(), frame_hsv.depth() );
    int ch[] = { 0, 0 };
    mixChannels( &frame_hsv, 1, &frame_hue, 1, ch, 1 );
    
    face_profile_cascade.detectMultiScale( frame, faces);
    
    double smin = 50;
    double _vmin = 80;
    double _vmax = 256;
    inRange(frame_hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
            Scalar(180, 256, MAX(_vmin, _vmax)), frame_mask);
    
    int returnResult = -1;
    bool isFindFace = false;
    for( size_t i = 0; i < faces.size(); i++ )
    {
        Mat faceROI = frame_gray(faces[i]);
        std::vector<Rect> eyes;
        
        face_front_cascade.detectMultiScale( faceROI, eyes);
        if( eyes.size() > 0)
        {
            Mat roi(frame_hue,faces[i]);
            Mat maskroi(frame_mask,faces[i]);
            calcHist(&roi, 1, 0, maskroi, hist, 1, &hisSize, &ranges);
            normalize(hist, hist,0,255,CV_MINMAX);
            
            returnResult = 0;
            isFindFace = true;
            break;
        }
    }
    
    if (isFindFace) {
        caculateBackProject(frame_hue,hist,frame_mask);
    }
    
    return returnResult;
}

bool lessValue(const ContourPoint &p1,const ContourPoint &p2)
{
    return p1.value > p2.value;
}

bool compareDotValue(const ContourPoint &p1,const ContourPoint &p2)
{
    return p1.dotValue < p2.dotValue;
}

bool compareCrossValue(const ContourPoint &p1,const ContourPoint &p2)
{
    return p1.crossValue > p2.crossValue;
}

bool compareAngle(const ContourPoint &p1,const ContourPoint &p2)
{
    return p1.angle > p2.angle;
}

bool compareDistance(const ContourPoint  &p1,const ContourPoint &p2)
{
    return p1.value > p2.value;
}

bool findFingerPoints(map<int,ContourPoint> *contourPoints,int peakIndex)
{
    ContourPoint peakPoint = (*contourPoints)[peakIndex];
    int upPointNum = 30;
    int upPointNumThreshold = upPointNum * 0.9;
    
    int upPointCount = 0;
    for (int i = 0; i < upPointNum; i++) {
        int leftIndex = peakPoint.index - i;
        ContourPoint leftPoint;
        if (leftIndex < 0) {
            leftIndex = contourPoints->size()-i;
        }
        leftPoint = (*contourPoints)[leftIndex];
        if (leftPoint.derivative > 0) {
            upPointCount ++;
        }
    }
    
    int downPointNum = 20;
    int downPointNumThreshold = downPointNum * 0.9;
    
    int downPointCount = 0;
    for (int i = 0; i< downPointNum; i++) {
        int rightIndex = peakPoint.index + i;
        ContourPoint rightPoint = (*contourPoints)[rightIndex];
        if (rightPoint.derivative < 0) {
            downPointCount ++;
        }
    }
    
    bool findFingerResult = false;
    if (upPointCount >= upPointNumThreshold && downPointCount >= downPointNumThreshold) {
        printf("peakCount is %d\n",peakIndex);
        printf("upPointCount is %d downPointCount is %d\n",upPointCount,downPointCount);
        peakPoint.isUsed = 1;
        for (int i = 0; i < upPointNum; i++) {
            ContourPoint *leftPoint = &(*contourPoints)[peakIndex-i];
            int leftIndex = peakIndex-i;
            while (leftPoint->isUsed == 1) {
                leftIndex --;
                if (leftIndex > 0) {
                    leftPoint = &(*contourPoints)[leftIndex];
                } else {
                    leftPoint = &(*contourPoints)[contourPoints->size()];
                    break;
                }
            }
            leftPoint->isUsed = 1;
        }
        for (int i = 0; i < downPointNum; i++) {
            ContourPoint *rightPoint = &(*contourPoints)[peakIndex+i];
            int rightIndex = peakIndex+i;
            while (rightPoint->isUsed == 1) {
                rightIndex ++;
                if (rightIndex < contourPoints->size()) {
                    rightPoint = &(*contourPoints)[rightIndex];
                } else {
                    break;
                }
            }
            rightPoint->isUsed = 1;
        }
        findFingerResult = true;
    }
    return findFingerResult;
}


int caculateOneImage()
{
//    //寻找轮廓
    cv::Rect bRect = cv::boundingRect( cv::Mat(contour) );
    
//    cv::Point center = Point2f(bRect.x+ bRect.size().width/2 *1.2,bRect.y+bRect.size().height/2 * 1.5);
//    cv::Point contourCenter = Point2f(bRect.width/2,bRect.height/2);
    cv::Moments mu = moments(contour,false);
    cv::Point center =  Point2f( bRect.x+bRect.width/2, bRect.y+bRect.height*0.8);
    
    cv::Point contourCenter = Point2f(bRect.width/2 ,bRect.height*0.8);
    cv::Scalar color( 100, 150, 255 );
    circle(contourMat, contourCenter, 10, color);
//    imshow("contourMat", contourMat);
    
    float maxL = 0;
//    vector<contourPoint> contourPoints;
    int contourSize = contour.size();
//    map<float,float> contourPoints;
//    vector<ContourPoint> contourPoints;
    map<int,ContourPoint> contourPoints;
    vector<ContourPoint> contourArray;
    for (int i = 0; i < contourSize; i++) {
        float Y = (contour[i].y - center.y);
        float X = (contour[i].x - center.x);
        float L =  sqrt(X*X + Y*Y);
        float angle = M_PI + atan2(Y, X);
        ContourPoint point;
        point.value = L;
        point.isUsed = 0;
        point.angle = angle;
        if (L > maxL) {
            maxL = L;
        }
        point.index = i;
        contourPoints.insert(pair<int, ContourPoint>(i,point));
        contourArray.push_back(point);
//        contourPoints.push_back(point);
    }
    printf("maxL is %f\n",maxL);
    
    for (int i = 0;i<contourSize;i++) {
        float derivate = 0;
        float dotValue = 0;
        float crossValue = 0;
        if (i > 0 && i < contourSize-1) {
            derivate = (contourPoints[i+1].value-contourPoints[i-1].value)/2;
            dotValue = (contour[i-1].x-contour[i].x)*(contour[i+1].x-contour[i].x)+(contour[i-1].y-contour[i].y)*(contour[i+1].y-contour[i].y);
            crossValue = (contour[i-1].x-contour[i].x)*(contour[i+1].x-contour[i].x)-(contour[i-1].y-contour[i].y)*(contour[i+1].y-contour[i].y);
            crossValue = (contourPoints[i-1].angle-contourPoints[i+1].angle)*(contourPoints[i-1].angle-contourPoints[i].angle)-(contourPoints[i-1].value-contourPoints[i].value)*(contourPoints[i+1].value-contourPoints[i].value);
        } else if(i == 0){
            derivate = (contourPoints[1].value - contourPoints[0].value);
        } else if (i == contourSize) {
            derivate = (contourPoints[contourSize].value - contourPoints[contourSize-1].value);
        }
        contourPoints[i].derivative = derivate;
        contourPoints[i].dotValue = dotValue;
        contourPoints[i].crossValue = crossValue;
    }
    
    float distance[contourSize];
    memset(distance, 0.0, sizeof(distance));

    float derivativePoints[contourSize];
    memset(derivativePoints, 0.0, sizeof(derivativePoints));
    
    for (int i = 0; i < contourSize; i++) {
        distance[i] = contourPoints[i].value;
    }
    
//    float xValue[contourSize];
//    for (int i = 0; i < contourSize; i++) {
//        
//        xValue[i] = contourPoints[i].dotValue;
//    }
//
//    float yValue[contourSize];
//    for (int i = 0; i < contourSize; i++) {
//        yValue[i] = contourPoints[i].crossValue;
//    }
//    
    for (int i = 0; i < contourSize;i++) {
        derivativePoints[i] = contourPoints[i].derivative;
    }
    
    showFloatGraph("index-Distance", distance, contourSize);
    
    showFloatGraph("theTa-Distance-Derivative", derivativePoints, contourSize);
    
//    showFloatGraph("angle-Distance", angles, contourSize);
    
//    showFloatGraph("cross-Distance", yValue, contourSize);

    
    sort(contourArray.begin(), contourArray.end(), compareDistance);
    int peakIndex = contourArray[0].index;
    int fingerNum = 0;
    vector<int> peakPoints;
    
    int peakCount = 0;
    ContourPoint peakPoint;
    while(peakIndex >= 0)
    {
        Point point = contour[peakIndex];
        if (findFingerPoints(&contourPoints,peakIndex) && (contourMat.rows - point.y) > 10) {
            fingerNum++;
            imshow("contourMat", contourMat);
            peakPoints.push_back(peakIndex);
        }

        peakCount++;
        if (peakCount < contourSize) {
            peakPoint = contourPoints[contourArray[peakCount].index];
            while (peakPoint.isUsed == 1) {
                peakCount++;
                if (peakCount < contourSize) {
                    peakPoint = contourPoints[contourArray[peakCount].index];
                } else{
                    peakIndex = -1;
                    break;
                }
            }
            peakIndex = peakPoint.index;
        } else {
            peakIndex = -1;
        }
    }
    
    for (int i = 0; i < peakPoints.size(); i++) {
        Point point = contour[peakPoints[i]];
        ContourPoint contourPoint = contourPoints[peakPoints[i]];
        printf("angle is %f\n",contourPoint.angle);
        circle(contourMat, point , 20, color);
        imshow("contourMat", contourMat);
    }

    printf("finger num is %d\n",fingerNum);
    
    

    return fingerNum;
}



int main(int argc, char** argv)
{
    DIR *dp;
    struct dirent *dirp;
    
    if( !face_profile_cascade.load( face_profile_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if( !face_front_cascade.load( face_front_name ) ){ printf("--(!)Error loading\n"); return -1; };
    
    if ((dp = opendir(dirString.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dirString << endl;
        return errno;
    }
    
    int imageNum = 0;
    int failImageNum = 0;
    while ((dirp = readdir(dp)) != NULL) {
        if( strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0 )
        {
            sourceFileName = dirp->d_name;
            string sourceFilePath = dirString + sourceFileName;
            cout << sourceFilePath << endl;
            
            Mat frame = imread(sourceFilePath);
            
            if (frame.empty()) {
                continue;
            }
        
            imageNum ++;
//            if (findFace(frame) < 0) {
//                Mat skinFrame;
//                skinFrame.create(frame.rows, frame.cols, CV_8UC1);
//                skinExtract(frame, skinFrame);
//            }
            contour.clear();
            contourMat.setTo(0);
            Mat skinFrame;
            skinFrame.create(frame.rows, frame.cols, CV_8UC1);
            skinExtract(frame, skinFrame);

            int finger = caculateOneImage();
            
            string failImageString = failString + sourceFileName;
            if (finger != 5) {
                imwrite(failImageString, frame);
                printf("finger is %d\n",finger);
                failImageNum++;
            } else{
                printf("find finger success!!\n");
            }
//            imshow("skinImage", contourMat);
        }
    }
    
    printf("recall is %f",float(imageNum-failImageNum)/imageNum);
    
    closedir(dp);

    
    waitKey();
    
    return 0;
}
