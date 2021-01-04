//
//  StereoProcessor.hpp
//  Autonomous Vehicle
//
//  Created by Aidan Fisher on 12/29/20.
//

#ifndef StereoProcessor_hpp
#define StereoProcessor_hpp

#include <stdio.h>
#include <opencv.hpp>
#include <vector>
#include <memory>
#include <librealsense2/rs.hpp>

class StereoProcessor
{
public:
    StereoProcessor();
    
    virtual void startProcessor();
    
    virtual void computeDisparity();
    virtual void computeDisparityFrame(); 
    
    virtual cv::Mat getDisparity(); // to be used for testing purposes
    virtual void setDisparity(cv::Mat _disparity); // to be used for testing purposes
    
    virtual rs2::frame getDisparityFrame(); // to be used for testing purposes
    virtual void setDisparityFrame(rs2::frame _disparityFrame); // to be used for testing purposes
    
    virtual void addFilter(std::shared_ptr<rs2::filter> filter);
    //virtual rs2::filter getFilters();
    virtual void applyFilters();
    
    virtual void computeSlopeMap();
    
    virtual void computeRoadBoundary(double threshMin, double threshMax, double, double);
private:
    rs2::pipeline pipeline;
    rs2::frame disparityFrame;
    rs2_intrinsics intrinsics;
    std::vector<std::shared_ptr<rs2::filter>> filters;
    cv::Mat disparity;
    cv::Mat slopeMap;
    cv::Mat roadBoundary;
    int width;
    int height;
};

#endif /* StereoProcessor_hpp */
