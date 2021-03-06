//
//  main.cpp
//  Autonomous Vehicle
//
//  Created by Aidan Fisher on 12/17/20.
//

#include <iostream>
#include <opencv.hpp>
#include <librealsense2/rs.hpp>
#include "StereoProcessor.hpp"
#include "i2c.h" // from libi2c

int main(int argc, const char * argv[])
{
    // create the pipeline
    std::shared_ptr<rs2::pipeline> pipeline(new rs2::pipeline());
    pipeline->start();
    
    // create a StereoProcessor, a class which processes data
    // then, start the StereoProcessor
    std::shared_ptr<StereoProcessor> stereoProcessor(new StereoProcessor());
    stereoProcessor->startProcessor();
    
    // compute a disparity frame
    stereoProcessor->computeDisparityFrame();
    
    // apply filters
    stereoProcessor->applyFilters(); // TODO: Add filters to the processor
    
    // convert the frame to a Mat object (OpenCV)
    stereoProcessor->computeDisparity();
}
