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

typedef struct detectedObj
{
    KeyPoint points;
    int averageArea;
} detectedObj;

#endif /* barrierDetection_h */
