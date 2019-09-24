import numpy as np
import glob
import cv2

# Paths of calibration pictures from the left and right camera
pathLeft = "~/Desktop/StereoCamera/calibPics/pictures"
pathRight = "~/Desktop/StereoCamera/calibPics/picturesR"

def addObjAndImgPoints(path):
    # Arrays to hold all the object and image points from all the calibration images
    objectPoints = [] # Circle center coordinates relative to the circle board
    imagePoints = [] # Circle center coordinates in absolute positioning (relative to whole image).
    imageShape = None
    
    # We need to add data points for evaluating where a circle is on the grid
    # The circle pattern I used has a zip-zagging grid that up and down instead of being in a straight grid.
    # The circle grid I used can be found here:
    # In order to make sure the data points can fit this geometry, we must remove non-compatible data.
    # Also, the circle grid has a shape of 4 x 11.

    # Create a 3D array for holding circle center coordinates from a circle grid relative to the lower-leftest circle.
    objp = np.zeros((77, 3), np.float32) # The zero matrix is used as a placeholder, yet keeps a shape of 4 x 11.

    # Have a placeholder array to hold the indices of incompatible data points which need to be removed
    arraysToRemove = []

    # Replace filler data points with prospective data points to objp,
    # where the z component is zero and the x and y components
    # range from 0 to 3 and 0 to 5 respectively, with an increment of 0.5.
    objp[:,:2] = np.mgrid[0:3:7j, 0:5:11j].T.reshape(-1,2)

    # NOTE: Incompatible data points occur where one of the two components is a whole number,
    # and the other component has a decimal value added (from a whole number) of 0.5.
    # EX: x = 0.5, y = 1 does not exist on the circle pattern, so the array containing these points
    # must be removed.

    # Find and remove incompatible data points
    for i, x in enumerate(objp):
        if (abs(x[1] - x[0]) == 0.5) or (abs(x[1] - x[0]) == 1.5) or (abs(x[1] - x[0]) == 2.5) or (abs(x[1] - x[0]) == 3.5) or (abs(x[1] - x[0]) == 4.5): # Finds the incompatible data points to remove
            arraysToRemove.append(i) # Adds the indices of incompatible data point arrays for later use

    # Since the array we worked with in the for loop is an ndarray (numpy array),
    # we must convert the arraysToRemove array into an ndarray
    # so that we can remove the incompatible data points from objp.
    npArraysToRemove = np.array(arraysToRemove)

    # Remove the arrays whose data points cannot exist with the circle grid image
    objp = np.delete(objp, npArraysToRemove, 0)

    # Load calibration images
    imagePaths = glob.glob("{0}/*.jpg".format(path))

    # Find object and image points, and grab the shape of the image
    for file in imagePaths:
        img = cv2.imread(file)
        grayimg = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        imageShape = grayimg.shape[::-1]

        # Determine whether circles are found in the image and the coordinates of the centers
        circlesFound, centerCoords = cv2.findCirclesGrid(grayimg, (4, 11))

        # If circles are found, add object and image points
        if circlesFound:
            objectPoints.append(objp)
            imagePoints.append(centerCoords)

    return objectPoints, imagePoints, imageShape


# Find object and image points from both cameras (object points will be the same)
objectPointsL, imagePointsL, imageShapeL = addObjAndImgPoints(pathLeft)
objectPointsR, imagePointsR, imageShapeR = addObjAndImgPoints(pathRight)

# Grab initial stereo calibration data from both cameras (these are inputs for final calibration data).
isStereoCalibrated, cameraMatrixL, distCoeffsL, cameraMatrixR, distCoeffsR, rotationMatrixStereo, translationMatrixStereo, essentialMatrix, fundamentalMatrix = cv2.stereoCalibrate(objectPointsL, imagePointsL, imagePointsR, None, None, None, None, imageShapeL)

# Grab final stereo calibration data
if isStereoCalibrated:
    rotMatrixL, rotMatrixR, cameraMatrixNewL, cameraMatrixNewR, dispToDepthMap, roiL, roiR = cv2.stereoRectify(cameraMatrixL, distCoeffsL, cameraMatrixR, distCoeffsR, imageShapeL, rotationMatrixStereo, translationMatrixStereo)

    mapXL, mapYL = cv2.initUndistortRectifyMap(cameraMatrixL, distCoeffsL, rotMatrixL, cameraMatrixNewL, imageShapeL, cv2.CV_32FC1)
    mapXR, mapYR = cv2.initUndistortRectifyMap(cameraMatrixR, distCoeffsR, rotMatrixR, cameraMatrixNewR, imageShapeL, cv2.CV_32FC1)

    # Save the calibration data
    np.savez_compressed(calibrationData, imageShapeL = imageShapeL, imageShapeR = imageShapeR, mapXL=mapXL, mapYL = mapYL, roiL = roiL , mapXR = mapXR, mapYR = mapYR, roiR = roiR)
