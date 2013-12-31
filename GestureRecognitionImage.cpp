//
//  GestureRecognitionImage.cpp
//  MyGestureRecognition
//
//  Created by yeahugo on 13-12-16.
//
//

#include "GestureRecognitionImage.h"

struct ContourPoint
{
    int index;
    float angle;
    float value;
    float derivative;
    int isUsed;
} ;

struct ExtreamPoint
{
    int index;
    int pointType;
    int isUsed;
    float angle;
};

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

void findLargestContour(const Mat &frame, Mat &skinArea,vector<Point> &contour);

//肤色提取，skinArea为二值化肤色图像
void skinExtract(const Mat &frame, Mat &skinArea,vector<Point> &contour)
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
    
    findContours(copyContour, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1);
    
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
    skinArea = copyContour;
//    contourMat = skinArea;
//    contourMat = skinArea(bRect);
    blur( skinArea, skinArea, Size(3,3) );
    Canny(skinArea, skinArea, 50, 150, 3);
    contour = contours[index];
}

void findLargestContour(const Mat &frame, Mat &skinArea,vector<Point> &contour)
{
    //寻找轮廓
//    blur( frame, frame, Size( 4, 4 ), Point(-1,-1));
    Mat frame_hsv;
    vector<Mat> channels;
    cv::cvtColor(frame, frame_hsv, CV_BGR2HSV);
    cv::split(frame_hsv, channels);
    frame_hsv = channels[1];
    
    imshow("frame_hsv", frame_hsv);
    
    Mat copy_hsv = frame_hsv.clone();
    
    cv::Canny(copy_hsv, copy_hsv, 50, 150);
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

//    //膨胀和腐蚀，膨胀可以填补凹洞（将裂缝桥接），腐蚀可以消除细的凸起（“斑点”噪声）
    dilate(copy_hsv, copy_hsv, Mat(3,3,CV_8UC1), Point(-1, -1));
//	erode(copy_hsv, copy_hsv, Mat(5, 5, CV_8UC1), Point(-1, -1));
    
    findContours(copy_hsv, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
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
    cv::drawContours(copy_hsv, contours, index, color,CV_FILLED,8,hierarchy);
    
    skinArea = copy_hsv;
//    blur( skinArea, skinArea, Size(3,3) );
//    Canny(skinArea, skinArea, 50, 150, 3);
    
    if (contours.size() > 0) {
        contour = contours[index];
    }
}
    
bool lessValue(const ContourPoint &p1,const ContourPoint &p2)
{
    return p1.value > p2.value;
}

bool compareAngle(const ContourPoint &p1,const ContourPoint &p2)
{
    return p1.angle > p2.angle;
}

bool compareDistance(const ContourPoint  &p1,const ContourPoint &p2)
{
    return p1.value > p2.value;
}

bool sortByPointIndex(const ExtreamPoint &p1,const ExtreamPoint &p2)
{
    return p1.index < p2.index;
}

int findFingerPoints(map<int,ContourPoint> *contourPoints,int peakIndex)
{
    ContourPoint peakPoint = (*contourPoints)[peakIndex];
    int contourSize = contourPoints->size();
    int leftPointNum = contourSize*0.03;
    int leftPointNumThreshold = leftPointNum * 0.7;
    
    int rightPointNum = contourSize*0.03;
    int rightPointNumThreshold = rightPointNum * 0.7;
    
    int leftUpPointCount = 0;
    int leftDownPointCount = 0;
    for (int i = 1; i <= leftPointNum; i++) {
        int leftIndex = peakPoint.index - i;
        ContourPoint leftPoint;
        if (leftIndex < 0) {
            leftIndex = contourPoints->size()-i+1;
        }
        leftPoint = (*contourPoints)[leftIndex];
        if (leftPoint.derivative > 0) {
            leftUpPointCount ++;
        }
        if (leftPoint.derivative < 0) {
            leftDownPointCount ++;
        }
    }
    
    int rightDownPointCount = 0;
    int rightUpPointCount = 0;
    for (int i = 1; i <= rightPointNum; i++) {
        int rightIndex = peakPoint.index + i;
        if (rightIndex > contourSize) {
            rightIndex = i-1;
        }
        ContourPoint rightPoint = (*contourPoints)[rightIndex];
        if (rightPoint.derivative < 0) {
            rightDownPointCount ++;
        }
        if (rightPoint.derivative > 0) {
            rightUpPointCount ++;
        }
    }
    
    int findFingerResult = 0;
    if (leftUpPointCount >= leftPointNumThreshold && rightDownPointCount >= rightPointNumThreshold) {
        peakPoint.isUsed = 1;
        findFingerResult = 1;
    }
    else if (leftDownPointCount >= leftPointNumThreshold && rightUpPointCount >= rightPointNumThreshold) {
        peakPoint.isUsed = 1;
        findFingerResult = 2;
    }
    if (peakPoint.isUsed == 1 ) {
        for (int i = 0; i < leftPointNum; i++) {
            ContourPoint *leftPoint = &(*contourPoints)[peakIndex-i];
            int leftIndex = peakIndex-i;
            while (leftPoint->isUsed == 1) {
                leftIndex --;
                if (leftIndex == peakIndex) {
                    break;
                }
                if (leftIndex < 0) {
                    leftIndex = contourSize-i;
                }
                leftPoint = &(*contourPoints)[leftIndex];
            }
            leftPoint->isUsed = 1;
        }
        for (int i = 0; i < rightPointNum; i++) {
            ContourPoint *rightPoint = &(*contourPoints)[peakIndex+i];
            int rightIndex = peakIndex+i;
            while (rightPoint->isUsed == 1 ) {
                rightIndex ++;
                if (rightIndex == peakIndex) {
                    break;
                }
                if (rightIndex > contourSize) {
                    rightIndex = i;
                }
                rightPoint = &(*contourPoints)[rightIndex];
            }
            rightPoint->isUsed = 1;
        }
    }
    return findFingerResult;
}


HandStruct GestureRecognitionImage::caculateOneImage(vector<Point > contour,Mat contourMat)
{
//    //寻找轮廓
    cv::Rect bRect = cv::boundingRect( cv::Mat(contour) );
    
//    cv::Point center = Point2f(bRect.x+ bRect.size().width/2 *1.2,bRect.y+bRect.size().height/2 * 1.5);
//    cv::Point contourCenter = Point2f(bRect.width/2,bRect.height/2);
    cv::Moments mu = moments(contour,false);
    cv::Point center =  Point2f( bRect.x+bRect.width/2, bRect.y+bRect.height/2);
    
    cv::Point contourCenter = Point2f(bRect.width/2 ,bRect.height/2);
    cv::Scalar whiteColor( 255, 255, 255);
    cv::Scalar color( 255, 255, 255);
    cv::Scalar blackColor( 0, 0, 0);
    circle(contourMat, contourCenter, 50, whiteColor);
    
    float maxL = 0;
    int contourSize = contour.size();
    map<int,ContourPoint> contourPoints;
    vector<ContourPoint> contourArray;
    for (int i = 0; i < contourSize; i++) {
        float Y = (contour[i].y - center.y);
        float X = (contour[i].x - center.x);
        float L =  sqrt(X*X + Y*Y);
        float angle = asinf(Y/L);
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
    }
    printf("maxL is %f\n",maxL);
    
    for (int i = 0;i<contourSize;i++) {
        float derivate = 0;
        if (i > 0 && i < contourSize-1) {
            derivate = (contourPoints[i+1].value-contourPoints[i-1].value)/2;
        } else if(i == 0){
            derivate = (contourPoints[1].value - contourPoints[0].value);
        } else if (i == contourSize) {
            derivate = (contourPoints[contourSize].value - contourPoints[contourSize-1].value);
        }
        contourPoints[i].derivative = derivate;
    }
    
    float distance[contourSize];
    memset(distance, 0.0, sizeof(distance));

    float derivativePoints[contourSize];
    memset(derivativePoints, 0.0, sizeof(derivativePoints));
    
    for (int i = 0; i < contourSize; i++) {
        distance[i] = contourPoints[i].value;
    }
    
    for (int i = 0; i < contourSize;i++) {
        derivativePoints[i] = contourPoints[i].derivative;
    }
    
//    showFloatGraph("index-Distance", distance, contourSize);
//    showFloatGraph("theTa-Distance-Derivative", derivativePoints, contourSize);
    
    sort(contourArray.begin(), contourArray.end(), compareDistance);
    int peakIndex = contourArray[0].index;
    int fingerNum = 0;
    int valleyNum = 0;
    vector<int> peakPoints;
    vector<int> valleyPoints;
    
    int peakCount = 0;
    ContourPoint peakPoint;
    while(peakIndex >= 0)
    {
        Point point = contour[peakIndex];
        int fingerResult = findFingerPoints(&contourPoints,peakIndex);
        if (fingerResult == 1 ) {
            printf("angle is %f\n",contourPoints[peakIndex].angle);
            fingerNum++;
            peakPoints.push_back(peakIndex);
        }
//        if (fingerResult == 2) {
//            valleyNum++;
//            valleyPoints.push_back(peakIndex);
//        }

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
    
    map<int, ContourPoint>::iterator it;
    for (it = contourPoints.begin(); it != contourPoints.end(); it++) {
        it->second.isUsed = 0;
    }
    
    //处理波谷
    int valleyCount = contourArray.size();
    ContourPoint valleyPoint;
    int valleyIndex = contourArray.back().index;
    while(valleyIndex >= 0)
    {
        Point point = contour[valleyIndex];
        int fingerResult = findFingerPoints(&contourPoints,valleyIndex);
        if (fingerResult == 2 ) {
            valleyNum++;
            valleyPoints.push_back(valleyIndex);
        }
        
        valleyCount--;
        if (valleyCount > 0) {
            valleyPoint = contourPoints[contourArray[valleyCount].index];
            while (valleyPoint.isUsed == 1) {
                valleyCount--;
                if (valleyCount > 0) {
                    valleyPoint = contourPoints[contourArray[valleyCount].index];
                } else{
                    valleyIndex = -1;
                    break;
                }
            }
            valleyIndex = valleyPoint.index;
        } else {
            valleyIndex = -1;
        }
    }
    
    vector<ExtreamPoint > peakValleyPoint;  //波峰、波谷
    for (int i = 0; i < peakPoints.size(); i++) {
        int peakIndex = peakPoints[i];
        assert(peakIndex < contour.size());
        Point point = contour[peakIndex];
//        point = Point2f(point.x-bRect.x,point.y-bRect.y);
        ExtreamPoint extreamPoint;
        extreamPoint.index = peakPoints[i];
        extreamPoint.pointType = 1;
        extreamPoint.isUsed = 0;
        extreamPoint.angle = contourPoints[peakPoints[i]].angle;
        peakValleyPoint.push_back(extreamPoint);
    }
    for (int i = 0; i < valleyPoints.size(); i++) {
        int valleyIndex = valleyPoints[i];
        assert(valleyIndex < contour.size());
        Point point = contour[valleyIndex];
        ExtreamPoint extreamPoint;
        extreamPoint.index = valleyPoints[i];
        extreamPoint.pointType = 0;
        extreamPoint.isUsed = 0;
        extreamPoint.angle = contourPoints[valleyPoints[i]].angle;
        peakValleyPoint.push_back(extreamPoint);
    }
    
    int extreamPointSize = peakValleyPoint.size();
    if (extreamPointSize > 0) {
        sort(peakValleyPoint.begin(), peakValleyPoint.end(), sortByPointIndex);
    }

    HandStruct returnHand;
    returnHand.peakNum = fingerNum;
    returnHand.valleyNum = valleyNum;

    int crossNum = 0;
    
    for (int i = 0; i < extreamPointSize; i++) {
        Point point = contour[peakValleyPoint[i].index];
        printf("point index is %d type is %d\n",i,peakValleyPoint[i].pointType);
        if (peakValleyPoint[i].pointType == 1) {
            rectangle(contourMat, Point2f(point.x-10,point.y-10), Point2f(point.x+10,point.y+10+i*5), whiteColor);
        } else {
            circle(contourMat, point,i+5, whiteColor);
        }
//        circle(contourMat, point,i+5, whiteColor);
    }
    
    for (int i = 0; i < extreamPointSize; i++) {
        ExtreamPoint extreamPoint = peakValleyPoint[i];
        if (extreamPoint.pointType == 1) {
            bool haveValley = false;
            ExtreamPoint nextPeakPoint;
            nextPeakPoint.index = -1;
            int j = i + 1;
            while (j != i) {
                if (peakValleyPoint[j].pointType == 1) {
                    nextPeakPoint = peakValleyPoint[j];
                    break;
                } else {
                    haveValley = true;
                }
                j= (j+1)%extreamPointSize;
            }

            if (nextPeakPoint.index != -1 && haveValley) {
                Point P = contour[extreamPoint.index];
                Point nP = contour[nextPeakPoint.index];
                Point V = contour[peakValleyPoint[(i+1)%extreamPointSize].index];
                float b = pow((P.x-V.x), 2) + pow((P.y-V.y), 2);
                float c = pow((nP.x-V.x), 2) + pow(nP.y - V.y,2);
                float a = pow((P.x-nP.x),2) + pow(P.y-nP.y,2);

                printf("b is %f c is %f a is %f\n",b,c,a);
                
                if (b + c > a) {
//                    line(contourMat, P, V, whiteColor);
//                    line(contourMat, nP, V, whiteColor);
                    crossNum++;
                }
            }
        }
    }
    returnHand.crossNum = crossNum;
    imshow("contourMat", contourMat);
    
    printf("crossNum is %d\n",crossNum);
    
    return returnHand;
}



int main2(int argc, char** argv)
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
            
            vector<Point > contour;
            Mat skinFrame;
            skinFrame.create(frame.rows, frame.cols, CV_8UC1);
            
            skinExtract(frame, skinFrame,contour);
//            findLargestContour(frame, skinFrame, contour);
            GestureRecognitionImage gestureRecognizer;
            int crossNum = 0;
            int peakNum = 0;
            if (contour.size() > 0) {
                HandStruct hand = gestureRecognizer.caculateOneImage(contour,skinFrame);
                crossNum = hand.crossNum;
                peakNum = hand.peakNum;
            }
            
            string failImageString = failString + sourceFileName;
            if (crossNum < 3) {
                imwrite(failImageString, frame);
                printf("finger is %d\n",crossNum);
                
                failImageNum++;
            } else{
                printf("find finger success!!\n");
            }
        }
    }
    
    printf("recall is %f",float(imageNum-failImageNum)/imageNum);
    
    closedir(dp);

    
    waitKey();
    
    return 0;
}
