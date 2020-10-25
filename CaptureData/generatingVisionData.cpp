//
//  generatingVisionData.cpp
//  PARAutonomousVehicle
//
//  Created by Aidan Fisher on 12/23/19.
//  Copyright © 2019 Aidan Fisher. All rights reserved.
//

#include "generatingVisionData.hpp"
#include "main.hpp"

using namespace rs2;
using namespace cv;
using namespace std;

pipeline turnOnCamera()
{
    pipeline realsensePipeline;
    realsensePipeline.start();
    
    return realsensePipeline;
}

/*
 Input: pipeline& realsensePipeline - reference to the main controller for the realsense camera
 Output: depth_frame* depthFramePointer - a pointer to the depth frame
         color_frame* colorFramePointer - a pointer to the color frame
         These two outputs are accessed outside the function by dereferencing them. We pass them as pointers to allow two outputs.
 */
void getFrames(pipeline& realsensePipeline, depth_frame* depthFramePointer, frame* colorFramePointer)
{
    frameset frames = realsensePipeline.wait_for_frames(); // lets us access stereo and color frames. We wait to get frames to ensure proper capture.
    
    depth_frame depthFrame = frames.get_depth_frame();
    frame colorFrame = frames.get_color_frame();
    
    // Get the addresses of the depth and color frame. This allows for deferencing later.
    depthFramePointer = &depthFrame;
    colorFramePointer = &colorFrame;
}

// NOTE: Set parameters for the hole filling filter BEFORE using this function.
depth_frame fillDepthImageHoles(depth_frame depthFrame, hole_filling_filter holeFillingFilter)
{
    return holeFillingFilter.process(depthFrame);
}

// Uses a depth frame to generate 3d points (voxels) as pointclouds.
points getPointclouds(depth_frame depthFrame)
{
    pointcloud pc; // Nothing on its own, but it allows us to generate individual "pointclouds"
    
    return pc.calculate(depthFrame);
}

