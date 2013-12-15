#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include <fstream>

using namespace std;
using namespace cv;

static void showDetections(const vector<Rect>& found, Mat& imageData) {
    vector<Rect> found_filtered;
    size_t i, j;
    for (i = 0; i < found.size(); ++i) {
        Rect r = found[i];
        for (j = 0; j < found.size(); ++j)
        if (j != i && (r & found[j]) == r)
        break;
        if (j == found.size())
        found_filtered.push_back(r);
    }
    for (i = 0; i < found_filtered.size(); i++) {
        Rect r = found_filtered[i];
        rectangle(imageData, r.tl(), r.br(), Scalar(64, 255, 64), 3);
    }
}

static void detectTest(const HOGDescriptor& hog, Mat& imageData) {
    vector<Rect> found;
    int groupThreshold = 2;
    Size padding(Size(32, 32));
    Size winStride(Size(8, 8));
    double hitThreshold = 0.2; // tolerance
    hog.detectMultiScale(imageData, found, hitThreshold, winStride, padding, 1.05, groupThreshold);
    showDetections(found, imageData);
}

using namespace std;
using namespace cv;

Mat input_image;
Mat output_mask;
Mat output_image;
IplImage src_image;
IplImage *mask_image;

int main1(int argc, char ** argv){
    VideoCapture cam(0);
    if(!cam.isOpened()){
        printf("Error opening camera!\n");
        return EXIT_FAILURE;
    }
    
    int massMin = 5000, massMax = 24000;
    namedWindow("output image");
    cv::Scalar color( 100, 150, 255 ); //random color
    
    CvAdaptiveSkinDetector skin_detector(1, CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE_DILATE);
    while(true) {
        cam >> input_image;
        src_image = input_image.operator _IplImage();
        if(mask_image == NULL)
            mask_image = cvCreateImage(cvSize(src_image.width, src_image.height), IPL_DEPTH_8U, 1);    //一定要先分配一个内存
        skin_detector.process(&src_image, mask_image);
        output_mask = mask_image;
        input_image.copyTo(output_image, output_mask);
        
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::cvtColor(output_image, output_image, CV_BGR2GRAY);
        cv::findContours(output_image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1);
        
        int idx = 0;
        for (; idx >= 0; idx = hierarchy[idx][0]) {
            cv::Moments mom = cv::moments(cv::Mat(contours[idx]));
            int cMass = mom.m00;
            cv::RotatedRect rotRect = cv::minAreaRect( cv::Mat(contours[idx]) );
            cv::Rect bRect = rotRect.boundingRect();
            
            if (cMass > massMin && cMass < massMax) {
                cv::Mat contourImg(output_image.size(), output_image.type(), cv::Scalar(0));
                cv::drawContours(contourImg, contours, idx, color,CV_FILLED,8,hierarchy);
                output_image += contourImg;
                
                rectangle(output_image, bRect, cv::Scalar(255,255,255), 3);
            }
        }
        imshow("output image", output_image);

        output_image.setTo(0);
        
        if(27 == waitKey(30))
            EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}