//
//  CaptureData.cpp
//  PARAutonomousVehicle
//
//  Created by Aidan Fisher on 9/1/20.
//  Copyright © 2020 Aidan Fisher. All rights reserved.
//

#include "CaptureData.hpp"

int main (int argc, char* argv[])
{
    // Turn on the RealSense Camera
    rs2::pipeline realsensePipeline;
    realsensePipeline.start();
    
    // Start a serial communication
    

    // Wait until a frame is retrieved from the camera
    rs2::frameset frames = realsensePipeline.wait_for_frames();
    
    
}
