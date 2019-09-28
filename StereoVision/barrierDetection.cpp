#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*
 detectObjs
 Accepts: Mat disparity: a stereo disparity image
          int areaFilter: the minimum area to filter by (aka remove noise while detecting blobs)
 Returns: vector<Keypoint> keypoints: all keypoints detected by the blob detector
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

Mat computeDisparity(Mat frameL, Mat frameR, std::string filename)
{
  Mat disparity; // Initialize disparity image (will be filled later)

  // Initialize the stereo image
  static Ptr<StereoBM> stereoImage = StereoBM::create();

  // TODO: Filters, Parameter settings, import calibration parameters, etc.
  
  // Compute the stereo disparity and store it in disparity
  stereoImage->compute(frameL, frameR, disparity);

  return disparity;
}
