#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "barrierDetection.hpp"

#define PI 3.14159

using namespace std;
using namespace cv;

/*
 computeDisparity
 Accepts: Mat frameL: left image
          Mat frameR: right image
 Returns: Mat dispairty: the 3D stereo image (disparity)
 */
Mat computeDisparity(Mat frameL, Mat frameR)
{
  Mat disparity; // Initialize disparity image (will be filled later)

  // Initialize the stereo image
  static Ptr<StereoBM> stereoImage = StereoBM::create();

  // TODO: Filters, Parameter settings, import calibration parameters, etc.
  
  // Compute the stereo disparity and store it in disparity
  stereoImage->compute(frameL, frameR, disparity);

  return disparity;
}

/*
 detectObjs
 Accepts: Mat disparity: a stereo disparity image
          int areaFilter: the minimum area to filter by (aka remove noise while detecting blobs)
 Returns: vector<KeyPoint> keypoints: all keypoints detected by the blob detector
 */
vector<KeyPoint> detectObjs(Mat disparity, int areaFilter)
{
    // Set parameters for the blob detector
    // TODO: Find good parameters
    SimpleBlobDetector::Params blobDetectorParameters;
    
    // Enable filtering by area, allowing us to remove noise
    blobDetectorParameters.filterByArea = true;
    blobDetectorParameters.minArea = areaFilter;
    
    // Instantiate the blob detector with parameters enabled
    Ptr<SimpleBlobDetector> blobDetector = SimpleBlobDetector::create(blobDetectorParameters);
    
    // Detect keypoints (where blobs are)
    vector<KeyPoint> keypoints; // Will hold all keypoints detected
    blobDetector->detect(disparity, keypoints);
    
    return keypoints;
}

/*
 computeObjDistance
 Accepts: Mat disparity: stereo image disparity
          vector<KeyPoint> keypoints: all keypoints of detected objects
 Returns: vector<detectedObj> detectedObjs: all detected objects (data type which will hold keypoint data + average distance)
 */
