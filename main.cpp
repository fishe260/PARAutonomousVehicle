//
//  main.cpp
//  PARAutonomousVehicle
//
//  Created by Aidan Fisher on 10/3/19.
//  Copyright © 2019 Aidan Fisher. All rights reserved.
//

#include "main.hpp"

using namespace std;
using namespace cv;

int main(int argc, char ** argv)
{
    // TODO: Use live feed for our left and right images
    // argv[1] is our left image
    // argv[2] is our right image
    // argv[3] is our output path
    // argv[4] is our output debug path
    
    if (argc != 5)
    {
        return EXIT_FAILURE;
    }
    
    // Load our images
    Mat leftImg = imread(argv[1]);
    Mat rightImg = imread(argv[2]);
    
    // Convert to grayscale images
    cvtColor(leftImg, leftImg, COLOR_BGR2GRAY);
    cvtColor(rightImg, rightImg, COLOR_BGR2GRAY);
    
    // Compute the disparity
    Mat dispImg = computeDisparity(leftImg, rightImg);
    imwrite(argv[3], dispImg);
    
    // Detect blobs and draw them (DEBUG)
    dispImg.convertTo(dispImg, CV_8UC1);
    vector<KeyPoint> blobPoints = detectObjs(dispImg, 1);
    Mat dispImgShowingBlobs = dispImg.clone(); // output
    
    drawKeypoints(dispImg, blobPoints, dispImgShowingBlobs);
    imwrite(argv[4], dispImgShowingBlobs);
    
    
    return EXIT_SUCCESS;
}
