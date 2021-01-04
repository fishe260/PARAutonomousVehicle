//
//  StereoProcessor.cpp
//  Autonomous Vehicle
//
//  Created by Aidan Fisher on 12/29/20.
//

#include "StereoProcessor.hpp"

StereoProcessor::StereoProcessor() : pipeline(rs2::pipeline()) {}

void StereoProcessor::startProcessor()
{
    // start streaming frames with only the depth frame being captured
    rs2::config configuration;
    configuration.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    pipeline.start(configuration);
    
    // grab intrinsics
    intrinsics = pipeline.get_active_profile().get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();
}

void StereoProcessor::computeDisparityFrame()
{
    rs2::frameset frames = pipeline.wait_for_frames();
    disparityFrame = frames.get_depth_frame();
}

void StereoProcessor::applyFilters()
{
    for (std::shared_ptr<rs2::filter> filter : filters)
    { 
        disparityFrame = filter->process(disparityFrame);
    }
}

void StereoProcessor::addFilter(std::shared_ptr<rs2::filter> filter)
{
    filters.push_back(filter);
}

void StereoProcessor::computeDisparity()
{
    // Query frame size (width and height)
    width = disparityFrame.as<rs2::video_frame>().get_width();
    height = disparityFrame.as<rs2::video_frame>().get_height();

    // Create OpenCV matrix of size (w,h) from the depth image
    disparity = cv::Mat(cv::Size(width, height), CV_8UC3, (void*)disparityFrame.get_data(), cv::Mat::AUTO_STEP);
}

void StereoProcessor::computeSlopeMap()
{
    // Generate G 2D array
    int ppy = intrinsics.ppy;
    int** g = new int*[width];
    for (int i = 0; i < height; i++)
    {
        g[i] = new int[height];
        for (int j = 0; j < height; j++)
        {
            g[i][j] = abs(height - ppy);
        }
    }
    
    // convert the G 2D array to a Mat type
    cv::Mat gMat = cv::Mat(width, height, CV_8UC3, g, cv::Mat::AUTO_STEP);
    
    // generate the slope map
    // slope map = G - disparity / partial derivative with respect to x of disparity
    // the / represents an element-wise divide
    cv::Mat disparityPrime = disparity; // copy
    cv::Sobel(disparity, disparityPrime, CV_8UC3, 0, 1);
    slopeMap = gMat - (disparity / disparityPrime); // TODO: Make sure the divide sign does element-wise division
    
    // delete G 2D array
    for (int i = 0; i < height; i++)
    {
        delete g[i];
    }
    delete[] g;
}

void StereoProcessor::computeRoadBoundary(double threshMin, double threshMax, double cannyMin, double cannyMax)
{
    // apply the threshold
    cv::threshold(slopeMap, roadBoundary, threshMin, threshMax, CV_8UC3);
    
    // TODO: If there are multiple shapes in the image, select the largest one
    
    // Apply edge detection to get "two" lines
    cv::Canny(roadBoundary, roadBoundary, cannyMin, cannyMax);
}

cv::Mat StereoProcessor::getDisparity()
{
    return disparity;
}

void StereoProcessor::setDisparity(cv::Mat _disparity)
{
    disparity = _disparity;
}

rs2::frame StereoProcessor::getDisparityFrame()
{
    return disparityFrame;
}

void StereoProcessor::setDisparityFrame(rs2::frame _disparityFrame)
{
    disparityFrame = _disparityFrame;
}
