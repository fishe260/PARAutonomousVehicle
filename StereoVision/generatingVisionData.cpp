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
 Outputs: Mat depthPCIndex: the resulting matrix whose value at a certain (x, y) index at the depth image maps the corresponding point in the pointcloud domain.
*/
void convertDepthToPCindicdes(Mat depthImage, points pointcloudImage, double pointcloudSideLength, Mat depthPCIndex)
{
    depthPCIndex = Mat(depthImage.rows, depthImage.cols, CV_16U); // matrix that allows values from 0-65,535 (2^16 - 1) unsigned
                                                                      // Matrix is the same size as the depthImage, as it requires it as a reference for pointcloud indices
    
    for (int x = 0; x < depthImage.rows; x++)
    {
        for (int y = 0; y < depthImage.cols; y++)
        {
            // Each index for the depthPCIndex has a corresponding index for the point clouds, affected by the pointcloudSideLength.
            // Note that round returns a double, however we requested an int, so a cast was performed.
            // TODO: Incorporate the height of the point in question. Intel Realsense has a pixel to point decoder function, and looking into that may be a good idea.
            depthPCIndex.at<vector<int>>(x, y) = {(int) round(x / pointcloudSideLength), (int) round(y / pointcloudSideLength)};
        }
    }
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
