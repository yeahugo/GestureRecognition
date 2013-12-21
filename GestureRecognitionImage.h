//
//  GestureRecognitionImage.h
//  MyGestureRecognition
//
//  Created by yeahugo on 13-12-16.
//
//

#ifndef __MyGestureRecognition__GestureRecognitionImage__
#define __MyGestureRecognition__GestureRecognitionImage__

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#include "GraphUtils.h"
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <sys/types.h>
#include <dirent.h>

using namespace cv;
using namespace std;

struct HandStruct
{
    int peakNum;
    int valleyNum;
} ;

class GestureRecognitionImage
{
    public:
    HandStruct caculateOneImage(vector<Point > contour,Mat contourMat);
};


#endif /* defined(__MyGestureRecognition__GestureRecognitionImage__) */


