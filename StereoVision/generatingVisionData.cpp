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
 Input: pipeline realsensePipeline - main controller for the realsense camera
 Output: depth_frame* depthFramePointer - a pointer to the depth frame
         color_frame* colorFramePointer - a pointer to the color frame
         These two outputs are accessed outside the function by dereferencing them. We pass them as pointers to allow two outputs.
 */
void getFrames(pipeline realsensePipeline, depth_frame* depthFramePointer, frame* colorFramePointer)
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

/*
 Inputs: Mat depthImage: an OpenCV-compatible (stereo) depth image
         points pointcloudImage: pointclouds corresponding to the depth image
         double pointcloudSideLength: the side length of a single pointcloud (they are all uniformly the same in theory).
 Outputs: vector<vector<int>>*: reference of the resulting array whose value at a certain (x, y) index at the depth image maps the corresponding point in the pointcloud domain.
*/
vector<vector<int>>* convertDepthToPCIndices(Mat depthImage, points pointcloudImage, double pointcloudSideLength)
{
    
    vector<vector<int>> depthPCIndex; // a 2D array which holds the corresponding pointcloud to depth image indices
    
    float pixel[2] = {0}; // a C-style array which holds two floats to corresponding x, y pixel values. All values declared 0 for now.
    float point[3] = {0}; // a C-style array which holds three floats corresponding to x, y, z pointcloud values. All values declared 0 for now.
    
    const vertex* vertices = pointcloudImage.get_vertices();
    for (int i = 0; i < pointcloudImage.size(); i++)
    {
        point[0] = (float) vertices->x;
        point[1] = (float) vertices->y;
        point[2] = (float) vertices->z;
        
        // rs2_project_point_to_pixel(pixel, <#const struct rs2_intrinsics *intrin#>, point) // TODO: add intrinsics
        
    }
    
    return NULL; // TODO: put in return type 
}

Mat buildHeightMap (points pointcloudImage, int numRows, int numCols, int depth, Mat depthToPCIndex)
{
    // Index the point cloud image
    
    // Declare and allocate a height map, as a Mat type
    Mat heightMap = Mat(numRows, numCols, CV_8U); // height map whose values are 8-bit (0-255) unsigned integers.
    
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            vector<int> pcIndexVector = depthToPCIndex.at<vector<int>>(i, j);
            // heightMap.at<int>(i, j) = depthToPCIndex[; // Access the depth of the pointcloud at this location and write it as the value at said specific index for the height map. TODO: Retrieve the height at the point.
        }
    }
    
    // TODO: Convert from int to Mat, either directly or through some BS means
    
    return heightMap;
}

