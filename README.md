# PARAutonomousVehicle - The GitHub Repository for the Purdue Electric Vehicle Club - Autonomous Division!

NOTE: Purdue Electric Vehicle Club - Autonomous Division was also known as Purdue Autonomous Racing. 

We are Purdue Electric Vehicle Club - Autonomous Division! Our goal is to engineer an autonomous racing kart with the intention to win the Purdue Electric Racing Autonomous Vehicle competition. 

We intend to do complete this interdisciplinary project through three teams: the Mechanical, Electrical, and Software Team. The Mechanical Team's goal is to design the kart so that it has the ability to drive and steer at its maximum potential. The Electrical Team ensures that power is delivered to all subsystems of the kart; the team also ensures proper data communication is directed without interferences due to noise or hazards. The Software Team designs and implements algorithms relating to Computer Vision, Simultaneous Localization and Mapping (SLAM), and Neural Networks. Each team lead collaborates with the others to ensure overlap is minimized and that the subsystems of the kart work properly when implemented. 

## Team Leads

The team leads are as follows:

Project Lead: Tyler Mahlmamn.

Software Lead: Aidan Fisher.

Electrical Lead: Noah Pictor.

Mechanical Lead: Shao-Ting Huang. 

## Navigating Branches

If you are interested in navigating our respository, please follow our guide as follows below. 

master: The latest stable code. 

StereoVision: Holds the latest code relating to Object Detection (implemented via Stereo Vision). 

SLAM: Holds the latest code relating to Simultaneous Localization and Mapping. This works mainly with the ROS library (Robotic Operating System). 

RoadFollower: Holds the latest code relating to curve tracking, which when implemented navigates the track while on the kart's first lap in order to map out the track using Stereo Vision Object Detection. 

I2C: Holds the latest code relating to communication with applicable kart subsystems using the I2C protocol. 


## How the kart works - A step-by-step process

Interested in how our kart works? Our kart works as follows: 

0) Before the race, our stereo vision camera is calibrated using a checkerboard pattern to remove distortion and correctly position our cameras with respect to each other. We also can retrieve camera parameters such as vanishing point and focal point, which is useful in our later steps. 

1) A key is inserted and turned to enable power flow from the kart's batteries into the subsystems. The subsystems turn on, and our main board (as of the time of this commit, a Raspberry Pi 3. This will change to an NVIDIA Jetson in the future) executes the autonomy program executable to run our autonomy software. 

2) In the first lap, an RGB camera is used to look at the edges of the track to determine where to steer. An algorithm that detects where said edges are in relation to the image is implemented, and the image is transposed from a first-person view to a bird's eye, third-person view. We can accurately determine where the edges of the track turn (or if they are straight), and use a neural network to determine our steering output. We send the output via I2C to an Arduino board, which relays our steering output to a servo to change steering direction. 

3) While navigating around the first lap, a stereo vision camera implements an algorithm to detect objects (barriers and/or cones). The position of these objects is then fed into a map, which when the first lap finishes, will build the track completely. We can normalize where the objects are using the camera parameters we retrieved in step 0 (this means we can determine in real-world measurement units how far an object is). We also use I2C to retrieve data from a GPS, which allows us to verify where an object is with respect to the kart when step 4 is run. 

4) For the remaining laps, the kart navigates using the map generated in step 3. A modified version of the D* Lite algorithm is implemented which allows us to navigate the track while only moving forward (assuming that the kart does not crash / get stuck and has to reverse). Localization is used to determine where the kart is in relation to the track, along with GPS data that can be acquired through the use of I2C retrieved in step 3. 
