#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

using namespace cv;

Mat barrierDetecton(Mat disparity)
{
  // 
}

Mat computeDisparity(Mat frameL, Mat frameR, std::string filename)
{
  Mat disparity; // Initialize disparity image (will be filled later)

  // Initialize the stereo image
  static Ptr<StereoBM> stereoImage = StereoBM::create();

  // TODO: Filters, Parameter settings, import calibration parameters, etc.
  
  // Compute the stereo disparity and store it in disparity
  *stereoImage.compute(frameL, frameR, disparity);

  return disparity;
}
