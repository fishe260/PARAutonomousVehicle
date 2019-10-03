//
//  barrierDetection.hpp
//  PARAutonomousVehicle
//
//  Created by Aidan Fisher on 9/28/19.
//  Copyright © 2019 Aidan Fisher. All rights reserved.
//

#ifndef barrierDetection_h
#define barrierDetection_h

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef struct DetectedObj
{
    KeyPoint points;
    double averageDepth;
} DetectedObj;

typedef struct Point
{
    Point( double X, double Y ): x(X), y(Y) {};
    double x;
    double y;
} Point;

Mat computeDisparity(Mat frameL, Mat frameR);
vector<KeyPoint> detectObjs(Mat disparity, int areaFilter);

#endif /* barrierDetection_h */
