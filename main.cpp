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
    
    // Turn on the RealSense Camera
    rs2::pipeline realsensePipeline;
    realsensePipeline.start();
    
    // Declare post-processing filters
    rs2::hole_filling_filter holeFillingFilter;
    
    // Wait until a frame is retrieved from the camera
    rs2::frameset frames = realsensePipeline.wait_for_frames();
    
    // Get depth image from camera (NOTE: this is not as a Mat!)
    rs2::depth_frame depthFrame = frames.get_depth_frame();
    rs2::frame colorFrame = frames.get_color_frame();
    
    // Fill holes in the depth frame
    depthFrame = holeFillingFilter.process(depthFrame);
    
    
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
