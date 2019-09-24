import numpy as np
import glob
import cv2
import time
from sklearn.preprocessing import normalize
from math import sqrt

# Path to the calibration file
calibrationFilePath = ""

# Path to testing data
leftImgPath = ""
rightImgPath = ""

K = [[9.037596e+02, 0.000000e+00, 6.957519e+02], [0.000000e+00, 9.019653e+02, 2.242509e+02], [0.000000e+00, 0.000000e+00, 1.000000e+00]]
P = [[7.215377e+02, 0.000000e+00, 6.095593e+02, -3.395242e+02], [0.000000e+00, 7.215377e+02, 1.728540e+02, 2.199936e+00], [0.000000e+00, 0.000000e+00, 1.000000e+00, 2.729905e-03]]
focalPoint = P[0][0]
principalPointU = P[0][2]
principalPointV = P[1][2]
bl = abs(P[0][3] / focalPoint)

Rleft = [[9.999758e-01, -5.267463e-03, -4.552439e-03], [5.251945e-03, 9.999804e-01, -3.413835e-03], [4.570332e-03, 3.389843e-03, 9.999838e-01]]
Rright = [[9.037596e+02, 0.000000e+00, 6.957519e+02], [0.000000e+00, 9.019653e+02, 2.242509e+02], [0.000000e+00, 0.000000e+00, 1.000000e+00]]

Tleft = [5.956621e-02, 2.900141e-04, 2.577209e-03]
Tright = [-4.731050e-01, 5.551470e-03, -5.250882e-03]

# TODO: Connect the two cameras or video feeds

# Computes the stereo image
def computeDisparity(frameL, frameR):
	# Convert the frames to grayscale
	frameL = cv2.cvtColor(frameL, cv2.COLOR_BGR2GRAY)
	frameR = cv2.cvtColor(frameR, cv2.COLOR_BGR2GRAY)

	# Constant: window size
	sad_window_size = 3

	# Undistort the cameras
	# frameL = cv2.remap(frameL, mapXL, mapYL, cv2.INTER_LINEAR)
	# frameR = cv2.remap(frameR, mapXR, mapYR, cv2.INTER_LINEAR)

	stereoImage = cv2.StereoSGBM_create(preFilterCap = 63, 
blockSize = sad_window_size, 
P1 = sad_window_size*sad_window_size*4, 
P2 = sad_window_size*sad_window_size*32, 
minDisparity = 0, 
numDisparities = 128, 
uniquenessRatio = 10,
speckleWindowSize = 100,
speckleRange = 32, 
disp12MaxDiff = 1,
mode = 1)

	# Create and set parameters for a stereo image
	#stereoImage = cv2.StereoSGBM_create(
	#		minDisparity=0,
	#		numDisparities=80,             # max_disp has to be dividable by 16 f. E. HH 192, 256
	#		blockSize=5,
	#		P1= 24 * window_size ** 2,    # wsize default 3; 5; 7 for SGBM reduced size image; 15 for SGBM full size image (1300px and above); 5 Works nicely
	#		P2=96 * window_size ** 2,
	#		disp12MaxDiff=1,
	#		uniquenessRatio=15,
	#		speckleWindowSize=0,
	#		speckleRange=2,
	#		preFilterCap=63,
	#		mode=cv2.STEREO_SGBM_MODE_SGBM_3WAY)

#	stereoImage = cv2.StereoSGBM_create(
#	blockSize = 61,
#	numDisparities = 192,
#	preFilterCap = 31,
#	minDisparity = 0,
#	uniquenessRatio = 1,
#	speckleWindowSize = 100,
#	speckleRange = 4,
#	disp12MaxDiff = 10,
#	mode = False,
#	# P1 = 600,
#	# P2 = 2400,
#	)

	stereoImageR = cv2.ximgproc.createRightMatcher(stereoImage)
	# stereoImage.setROI1(roiL)
	# stereoImage.setROI2(roiR)

	# Compute the stereo image into a depth map with the two cameras
	disparity = stereoImage.compute(frameL, frameR)
	disparityR = stereoImageR.compute(frameR, frameL)

	return disparity, disparityR, stereoImage, stereoImageR

def find_if_close(cnt1,cnt2, maxDist):
	row1 = cnt1.shape[0]
	row2 = cnt2.shape[0]
	for i in range(row1):
		for j in range(row2):
			dist = np.linalg.norm(cnt1[i]-cnt2[j])
			if abs(dist) < maxDist:
				return True
			elif i==row1-1 and j==row2-1:
				return False
def filterDisparity(disp, dispR, stereo, stereoR, imgL):
	imgL = cv2.cvtColor(imgL, cv2.COLOR_BGR2GRAY)

	# FILTER Parameters
	lmbda = 80000
	sigma = 1.2
	visual_multiplier = 1.0

	wlsFilter = cv2.ximgproc.createDisparityWLSFilter(stereo)
	wlsFilter.setLambda(lmbda)
	wlsFilter.setSigmaColor(sigma)

	disparity = np.int16(disp)
	disparityR = np.int16(dispR)

	filteredImg = wlsFilter.filter(disp, imgL, None, dispR)

	filteredImg = cv2.normalize(src=filteredImg, dst=filteredImg, beta=0, alpha=255, norm_type=cv2.NORM_MINMAX);
	filteredImg = np.uint8(filteredImg)

	# Return the disparity image 
	return filteredImg

def findMaxOfDisp(disp):
	max = 0
	for i in disp:
		for j in i: 
			if j > max: 
				max = j
	return max

# Computes a slope map through matrix per-element arithmetic and sobel operators
def computeSlopeMap(disparity):
	dispPrime = cv2.Sobel(disparity, cv2.CV_32F, 0, 1); # Calculates the first order derivative of the disparity image w/ same depth
	#cv2.imwrite("test.png", dispPrime)

	# Get width of disparity image
	dispLength = len(disparity[0])

	for i in range(0, len(disparity)):
		for j in range(0, dispLength):
			Vp = i - principalPointV
			Up = j - principalPointU

			if j == 0:
				Vcoord = np.array(Vp)
				Ucoord = np.array(Up)
			else: 
				Vcoord = np.append(Vcoord, Vp)
				Ucoord = np.append(Ucoord, Up)
		if i == 0:
			G = np.array(Vcoord)
			U = np.array(Ucoord)
		else:
			G = np.vstack((G, Vcoord))
			U = np.vstack((U, Ucoord))
	#cv2.imwrite("U.png", abs(U))
	#cv2.imwrite("V.png", abs(G))

	dispPrime[dispPrime == 0] = 1e-20

	dispDivDeriv = np.float64(np.divide(np.float64(disparity), np.float64(dispPrime)))

	B = np.float64(abs(np.subtract(G, dispDivDeriv)))
	#cv2.imwrite("B.png", B)

	# Calculate slope map
	slopeMap = np.divide(np.float64(B), np.float64(np.sqrt(np.add((focalPoint ** 2), (np.multiply(U, U))))))

	return slopeMap 
def computeCostMap2(slopeMap, obsSlopeThres, costFree, costAvoid, costPassable, obsHeightThres, erodeIter, dilateIter):
	img = np.float32(abs(slopeMap))
	_, thresh = cv2.threshold(slopeMap, obsSlopeThres, 255, 0);
	thresh = np.uint8(thresh)
	thresh = cv2.erode(thresh, (10, 1), iterations=erodeIter)
	thresh = cv2.dilate(thresh, None, iterations=dilateIter)
	#cv2.imwrite("threshold.png", thresh)
	contours,hier = cv2.findContours(thresh, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_NONE)

	LENGTH = len(contours)
	status = np.zeros((LENGTH,1))

#	for i,cnt1 in enumerate(contours):
#		x = i
#		if i != LENGTH-1:
#			for j,cnt2 in enumerate(contours[i+1:]):
#				x = x+1
#				dist = find_if_close(cnt1,cnt2, 50)
#				print(i, j, dist)
#				if dist == True:
#					val = min(status[i],status[x])
#					status[x] = status[i] = val
#				else:
#					if status[x]==status[i]:
#						status[x] = i+1
#	unified = []
#	maximum = int(status.max())+1
#	for i in range(maximum):
#		pos = np.where(status==i)[0]
#		if pos.size != 0:
#			cont = np.vstack(contours[i] for i in pos)
#			hull = cv2.convexHull(cont)
#			unified.append(hull)

#	cv2.drawContours(img,unified,-1,(0,255,0),2)
#	cv2.drawContours(thresh,unified,-1,255,-1)

	return thresh
# Creates a cost map through a slope map and cost parameters 
def computeCostMap(slopeMap, obsSlopeThres, costFree, costAvoid, costPassable, obsHeightThres):
	slopeMap = np.float64(abs(slopeMap))
	pen15 = (slopeMap <= obsSlopeThres)
	slopeMap[slopeMap > obsSlopeThres] = costAvoid
	slopeMap[pen15] = costFree
	# slopeMap[(abs(slopeMap) < obsHeightThres) and (abs(slopeMap) <= obsSlopeThres)] = costPassable
	# slopeMap = np.uint8(slopeMap)
	# slopeMap = cv2.applyColorMap(slopeMap, cv2.COLORMAP_JET)
	return slopeMap

def grabCalibData(filepath):
	# Unpack calibration data
	calibrationFile = np.load(filepath, allow_pickle = false)
	imageShapeL = tuple(calibrationFile["imageShapeL"])
	imageShapeR = tuple(calibrationFile["imageShapeR"])
	mapXL = calibrationFile["mapXL"]
	mapYL = calibrationFile["mapYL"]
	mapXR = calibrationFile["mapXR"]
	mapYR = calibrationFile["mapYR"]
	roiL = calibrationFile["roiL"]
	roiR = calibrationFile["roiR"]

def edgeDetect(disparity):
	return cv2.Canny(disparity, 100, 200)

def combineDisparities(disparity1, disparity2):
	mergedDisp = np.float32(np.zeros((len(disparity1), len(disparity1[0]))))
	# print(len(mergedDisp), len(mergedDisp[0]))
	for i, x in enumerate(disparity1):
		for j, y in enumerate(x):
			if disparity2[i][j] > disparity1[i][j]:
				mergedDisp[i][j] = disparity2[i][j]
			elif disparity1[i][j] >= disparity2[i][j]:
				mergedDisp[i][j] = disparity1[i][j]

	mergedDisp = cv2.GaussianBlur(mergedDisp, (11, 11), 0)
	return mergedDisp



def findEndPoint(costMap):
	# Crop image to be aligned with the focal point
	costMap = costMap[round(principalPointV):(len(costMap)), 0:len(costMap[0])]
	startingPoint = (len(costMap[0]) / 2, 0)

	# Determine ending point
	costMap = abs(costMap)
	# thresh = cv2.erode(thresh, None, iterations=3)
	# thresh = cv2.dilate(thresh, None, iterations=14)
	costMap = cv2.flip(costMap, 0)
	#cv2.imwrite("cropped.png", costMap)


	minVal = 0
	maxXCoord = 0
	maxYCoord = 0

	for i, y in enumerate(costMap):
		for j, x in enumerate(y):
			if ((j > 5) and (j < (len(y) - 7))):
				window = [y[j - 6], y[j - 5], y[j - 4], y[j - 3], y[j - 2], y[j - 1], y[j], y[j + 1], y[j + 2], y[j + 3], y[j + 4], y[j + 5], y[j + 6]]
			else:
				window = x

			if (np.any(window) == False):
				#print(window)
				#print(np.any(window))
				maxXCoord = j
				maxYCoord = i

	# print(maxXCoord, maxYCoord)

	return costMap, (startingPoint[0], startingPoint[1]), (maxXCoord, maxYCoord)


from warnings import warn

class Node:
    """
    A node class for A* Pathfinding
    """

    def __init__(self, parent=None, position=None):
        self.parent = parent
        self.position = position

        self.g = 0
        self.h = 0
        self.f = 0

    def __eq__(self, other):
        return self.position == other.position


def return_path(current_node):
    path = []
    current = current_node
    while current is not None:
        path.append(current.position)
        current = current.parent
    return path[::-1]  # Return reversed path


def astar(maze, start, end, allow_diagonal_movement = False):
    """
    Returns a list of tuples as a path from the given start to the given end in the given maze
    :param maze:
    :param start:
    :param end:
    :return:
    """

    # Create start and end node
    start_node = Node(None, start)
    start_node.g = start_node.h = start_node.f = 0
    end_node = Node(None, end)
    end_node.g = end_node.h = end_node.f = 0

    # Initialize both open and closed list
    open_list = []
    closed_list = []

    # Add the start node
    open_list.append(start_node)

    # Adding a stop condition
    outer_iterations = 0
    max_iterations = (len(maze) // 2) ** 2 

    # what squares do we search
    adjacent_squares = ((0, -1), (0, 1), (-1, 0), (1, 0),)
    if allow_diagonal_movement:
        adjacent_squares = ((0, -1), (0, 1), (-1, 0), (1, 0), (-1, -1), (-1, 1), (1, -1), (1, 1),)

    # Loop until you find the end
    while len(open_list) > 0:
        outer_iterations += 1

        # Get the current node
        current_node = open_list[0]
        current_index = 0
        for index, item in enumerate(open_list):
            if item.f < current_node.f:
                current_node = item
                current_index = index

        if outer_iterations > max_iterations:
            # if we hit this point return the path such as it is
            # it will not contain the destination
            warn("giving up on pathfinding too many iterations")
            return return_path(current_node)

        # Pop current off open list, add to closed list
        open_list.pop(current_index)
        closed_list.append(current_node)

        # Found the goal
        if current_node == end_node:
            return return_path(current_node)

        # Generate children
        children = []

        for new_position in adjacent_squares: # Adjacent squares

            # Get node position
            node_position = (current_node.position[0] + new_position[0], current_node.position[1] + new_position[1])

            # Make sure within range
            if node_position[0] > (len(maze[0]) - 1) or node_position[0] < 0 or node_position[1] > (len(maze)-1) or node_position[1] < 0:
                continue

            # Make sure walkable terrain
            if maze[node_position[1]][node_position[0]] != 0:
                continue

            # Create new node
            new_node = Node(current_node, node_position)

            # Append
            children.append(new_node)

        # Loop through children
        for child in children:

            # Child is on the closed list
            if len([closed_child for closed_child in closed_list if closed_child == child]) > 0:
                continue

            # Create the f, g, and h values
            child.g = current_node.g + 1
            child.h = ((child.position[0] - end_node.position[0]) ** 2) + ((child.position[1] - end_node.position[1]) ** 2)
            child.f = child.g + child.h

            # Child is already in the open list
            if len([open_node for open_node in open_list if child == open_node and child.g > open_node.g]) > 0:
                continue

            # Add the child to the open list
            open_list.append(child)
#
## Begin a loop which constantly receives an image from another Pi,
## Remaps the images based on calibration data,
## and then creates a depth image, repeatedly until the program stops.
#leftFrame = cv2.imread("pictures/Left/000020_10.png")
#rightFrame = cv2.imread("pictures/Right/000020_10.png")
#leftFrame2 = cv2.imread("pictures/Left/000020_11.png")
#rightFrame2 = cv2.imread("pictures/Right/000020_11.png")
#
## Compute the disparity map and display it
#dispMap, dispMapR, stereoImage, stereoImageR = computeDisparity(leftFrame, rightFrame)
#cv2.imwrite("disparity.png", dispMap)
#
#dispMap2, dispMapR2, stereoImage2, stereoImageR2 = computeDisparity(leftFrame2, rightFrame2)
#mergedDisparity = combineDisparities(dispMap, dispMap2)
#cv2.imwrite("CombinedDisp.png", mergedDisparity)
#
## dispFiltered = filterDisparity(dispMap, dispMapR, stereoImage, stereoImageR, leftFrame)
## cv2.imwrite("resultsFiltered/dispFiltered.png", dispFiltered)
#
## dispFiltered2 = filterDisparity(dispMap2, dispMapR2, stereoImage2, stereoImageR2, leftFrame)
## filteredCombined = combineDisparities(dispFiltered, dispFiltered2)
#
## Compute the slope map and display it
#slopeImg = computeSlopeMap(np.float32(mergedDisparity))
#cv2.imwrite("slopeMap.png", abs(slopeImg))
#
#slopeImgFiltered = computeSlopeMap(np.float32(filteredCombined))
#cv2.imwrite("resultsFiltered/slopeMapFiltered.png", abs(slopeImgFiltered))
#
## Compute the cost map and display it
#costImg = computeCostMap(slopeImg, 0.25, 1.0, 1e6, 100, 50)
#costImg = np.uint8(costImg)
#costImg = cv2.applyColorMap(costImg, cv2.COLORMAP_JET)
#cv2.imwrite("costMap.png", costImg)
#
#costImgFiltered = computeCostMap2(slopeImgFiltered, 0.4, 1.0, 1e6, 100, 50, 4, 20)
#costImgFiltered2 = np.uint8(costImgFiltered)
#costImgFiltered2 = cv2.applyColorMap(costImgFiltered, cv2.COLORMAP_JET)
#cv2.imwrite("resultsFiltered/costMapFiltered.png", costImgFiltered2)
#
#newCostImg = computeCostMap2(slopeImg, 0.25, 1.0, 1e6, 100, 50, 4, 20)
#cv2.imwrite("threshold.png", newCostImg);
#
#(costMap, (xStart, yStart), (xEnd, yEnd)) = findEndPoint(newCostImg)
#(costMapFiltered, (xStartF, yStartF), (xEndF, yEndF)) = findEndPoint(costImgFiltered)
#
## Plot points of costMap
#costMapNew = costMap.copy()
#costMapNew = cv2.cvtColor(costMapNew, cv2.COLOR_GRAY2RGB)
#xStart = np.round(xStart).astype("int")
#cv2.circle(costMapNew, (xStart, yStart), 5, (255, 0, 0))
#cv2.circle(costMapNew, (xEnd, yEnd), 5, (0, 255, 0))
#
#costMapNF = costMapFiltered.copy()
#costMapNF = cv2.cvtColor(costMapNF, cv2.COLOR_GRAY2RGB)
#xStartF = np.round(xStartF).astype("int")
#cv2.circle(costMapNF, (xStartF, yStartF), 5, (255, 0, 0))
#cv2.circle(costMapNF, (xEndF, yEndF), 5, (0, 255, 0))
#
#pathPoints = astar(np.ndarray.tolist(costMap), (xStart, yStart), (xEnd, yEnd), True)
## pathPointsF = astar(np.ndarray.tolist(costMapFiltered), (xStartF, yStartF), (xEndF, yEndF), True)
#print(pathPoints)
#
#costMapNew2 = costMap.copy()
#for i in pathPoints:
#	cv2.circle(costMapNew, i, 1, (0, 0, 255))
#
##for j in pathPointsF:
#	#cv2.circle(costMapNF, j, 1, (0, 0, 255))
#
#cv2.imwrite("plotted.png", costMapNew)
#cv2.imwrite("resultsFiltered/plottedFiltered.png", costMapNF)
#
## shitty edge detect
#cv2.imwrite("edgeDetect.png", edgeDetect(rightFrame))
#
def processData(leftFrame, leftFrame2, rightFrame, rightFrame2, iteration):
	dispMap, dispMapR, stereoImage, stereoImageR = computeDisparity(leftFrame, rightFrame)
	cv2.imwrite(r"results/disparity/" + str(iteration) + ".png", dispMap)

	#dispMap2, dispMapR2, stereoImage2, stereoImageR2 = computeDisparity(leftFrame2, rightFrame2)
	#mergedDisparity = combineDisparities(dispMap, dispMap2)
	#cv2.imwrite(r"results/combinedDisparity/" + str(iteration) + ".png", mergedDisparity)

	# Compute the slope map and display it
	slopeImg = computeSlopeMap(np.float32(cv2.GaussianBlur(dispMap, (5, 5), 0)))
	cv2.imwrite(r"results/slopeMap/" + str(iteration) + ".png", focalPoint * abs(slopeImg))

	# Compute the cost map and display it
	newCostImg = computeCostMap2(slopeImg, 0.25, 1.0, 1e6, 100, 50, 4, 20)
	cv2.imwrite(r"results/threshold/" +  str(iteration) + ".png", newCostImg);

	(costMap, (xStart, yStart), (xEnd, yEnd)) = findEndPoint(newCostImg)

	# Plot points of costMap
	costMapNew = costMap.copy()
	costMapNew = cv2.cvtColor(costMapNew, cv2.COLOR_GRAY2RGB)
	xStart = np.round(xStart).astype("int")
	cv2.circle(costMapNew, (xStart, yStart), 5, (255, 0, 0))
	cv2.circle(costMapNew, (xEnd, yEnd), 5, (0, 255, 0))

	if ((costMap[yStart, xStart] == 0) and (costMap[yEnd, xEnd] == 0)):
		pathPoints = astar(np.ndarray.tolist(costMap), (xStart, yStart), (xEnd, yEnd), True)

		if (pathPoints != None):
			for i in pathPoints:
				cv2.circle(costMapNew, i, 1, (0, 0, 255))
	cv2.imwrite(r"results/astarResults/" + str(iteration) + ".png", costMapNew)


# Load all images
imagesLeft1 = [cv2.imread(file) for file in glob.glob("pictures/Left/*_10.png")]
imagesLeft2 = [cv2.imread(file) for file in glob.glob("pictures/Left/*_11.png")]
imagesRight1 = [cv2.imread(file) for file in glob.glob("pictures/Right/*_10.png")]
imagesRight2 = [cv2.imread(file) for file in glob.glob("pictures/Right/*_11.png")]

print(len(imagesLeft1))
print(len(imagesLeft2))
print(len(imagesRight1))
print(len(imagesRight2))

for i in range(len(imagesLeft1)):
	print(i)
	if ((imagesLeft1[i].shape == imagesRight1[i].shape) and (imagesLeft1[i].shape == imagesLeft2[i].shape) and (imagesRight1[i].shape == imagesRight2[i].shape) and (imagesLeft2[i].shape == imagesRight2[i].shape) and (imagesLeft1[i].shape == imagesRight2[i].shape) and (imagesLeft2[i].shape == imagesRight1[i].shape)):
		processData(imagesLeft1[i], imagesLeft2[i], imagesRight1[i], imagesRight2[i], i)
