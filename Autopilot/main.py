import cv2
import numpy as np
from collections import deque
import picamera
import picamera.array
from smbus2 import SMBusWrapper	#I2c #has to be installed
from I2C import writeData, readData #import functions from the I2C file

# Aidan's todos:
# Work w/ Tyler to ensure proper I2C communication
# Get RNN training for throttle and steering

# Constants
numWindows = 9
l_windows = []
r_windows = []
address = 0x04 # Slave Address, set on Arduino
channel = 1 
class Window(object):
    """
    Represents a scanning window used to detect points likely to represent lane edge lines.
    """

    def __init__(self, y1, y2, x, m=100, tolerance=50):
        """
        Initialises a window object.
        Parameters
        ----------
        y1          : Top y axis coordinate of the window rect.
        y2          : Bottom y axis coordinate of the window rect.
        x           : X coordinate of the center of the window rect
        m           : X axis span, e.g. window rect width would be m*2..
        tolerance   : Min number of pixels we need to detect within a window in order to adjust its x coordinate.
        """
        self.x = x
        self.mean_x = x
        self.y1 = y1
        self.y2 = y2
        self.m = m
        self.tolerance = tolerance

    def pixels_in(self, nonzero, x=None):
        """
        Returns indices of the pixels in `nonzero` that are located within this window.
       Parameters
        ----------
        nonzero : Coordinates of the non-zero pixels in the image.
        Returns
        -------
        Array of indices of the pixels within this window.
        """
        if x is not None:
            self.x = x
        win_indices = (
                (nonzero[0] >= self.y1) & (nonzero[0] < self.y2) &
                (nonzero[1] >= self.x - self.m) & (nonzero[1] < self.x + self.m)
                ).nonzero()[0]
        if len(win_indices) > self.tolerance:
            self.mean_x = np.int(np.mean(nonzero[1][win_indices]))
        else:
            self.mean_x = self.x

        return win_indices

    def coordinates(self):
        """
        Returns coordinates of the bounding rect.
        Returns
        -------
        Tuple of ((x1, y1), (x2, y2))
        """
        return ((self.x - self.m, self.y1), (self.x + self.m, self.y2))

class Line(object):
    """
    Represents a single lane edge line.
    """

    def __init__(self, x, y, h, w):
        """
        Initialises a line object by fitting a 2nd degree polynomial to provided line points.
        Parameters
        ----------
        x   : Array of x coordinates for pixels representing a line.
        y   : Array of y coordinates for pixels representing a line.
        h   : Image height in pixels.
        w   : Image width in pixels.
        """
        # polynomial coefficients for the most recent fit
        self.h = h
        self.w = w
        self.frame_impact = 0
        self.coefficients = deque(maxlen=5)
        self.process_points(x, y)

    def averaged_fit(self):
        """
        Returns coefficients for a line averaged across last 5 points' updates.
        Returns
        -------
        Array of polynomial coefficients.
        """
        return np.array(self.coefficients).mean(axis=0)

    def fit(self, x, y):
        """
        Fits a 2nd degree polynomial to provided points and returns its coefficients.
        Parameters
        ----------
        x   : Array of x coordinates for pixels representing a line.
        y   : Array of y coordinates for pixels representing a line.
        """
        self.coefficients.append(np.polyfit(y, x, 1))
        self.process_points(x, y)

    def process_points(self, x, y):
        """
        Fits a polynomial if there is enough points to try and approximate a line and updates a queue of coefficients.
        Parameters
        ----------
        x   : Array of x coordinates for pixels representing a line.
        y   : Array of y coordinates for pixels representing a line.
        """
        enough_points = len(y) > 0 and np.max(y) - np.min(y) > self.h * .625
        if enough_points or len(self.coefficients) == 0:
            self.fit(x, y)

#Initialize the camera and the image (which will hold a numpy array)
camera = picamera.PiCamera()

# Set the camera resolution 
camera.resolution = (1920, 1080)

# Get camera calibration parameters
# TODO: Make calibraiton program and get horizontal line

exitProgram = False
while (not exitProgram):
    # Initialize stream 
    stream = picamera.array.PiRGBArray(camera) # Does not store data yet
    
    # Take photo and make it a numpy array
    camera.capture(stream, format = 'bgr')
    img = stream.array

    #TODO:  Create a while loop which exits when told to by i2c


    # Get color thresholding stuff
    hls = cv2.cvtColor(np.copy(img), cv2.COLOR_RGB2HLS).astype(np.float)
    sChannel = hls[:, :, 2]


    # Take sobels (used later)
    sobelX = cv2.Sobel(sChannel, cv2.CV_64F, 1, 0, ksize = 3)
    sobelY = cv2.Sobel(sChannel, cv2.CV_64F, 0, 1, ksize = 3)
    magnitude = np.sqrt(sobelX ** 2 + sobelY ** 2)
    direction = np.arctan2(np.absolute(sobelY), np.absolute(sobelX))
    sobelAbsX = (np.absolute(sobelX) * 255 / np.max(np.absolute(sobelX))).astype(np.uint8)
    sobelAbsY = (np.absolute(sobelY) * 255 / np.max(np.absolute(sobelY))).astype(np.uint8)

    # Convert magnitude to unsigned int8 vals
    magnitude = (magnitude * 255 / np.max(magnitude)).astype(np.uint8)

    # Initialize  masks for direction and magnitude, and abs sobels
    magnitudeMask = np.zeros_like(sChannel)
    directionMask = np.zeros_like(sChannel)
    sobelAbsXMask = np.zeros_like(sChannel)
    sobelAbsYMask = np.zeros_like(sChannel)

    # Initialize a gradient mask
    gradientMask = np.zeros_like(sChannel)

    # Fill direction, sobelAbs, and magnitude masks
    sobelAbsXMask[(sobelAbsX >= 20) & (sobelAbsX <= 100)] = 1
    sobelAbsYMask[(sobelAbsY >= 20) & (sobelAbsY <= 100)] = 1
    magnitudeMask[(magnitude >= 20) & (magnitude <= 100)] = 1
    directionMask[(direction >= 0.7) & (direction <= 1.3)] = 1 

    # Fill gradient mask
    gradientMask[((sobelAbsXMask == 1) & (sobelAbsYMask == 1)) | ((magnitudeMask == 1) & (directionMask == 1))] = 1

    # Color threshold based off of sChannel
    colorMask = np.zeros_like(sChannel)
    colorMask[(sChannel >= 170) & (sChannel <= 255)]
    
    # Combine gradient and color masks
    edges = np.zeros_like(gradientMask)
    edges[(gradientMask == 1) | (colorMask == 1)] = 1
    

    # Warp the image to get a "birds-eye" 
    (h, w) = (img.shape[0], img.shape[1])
    source = np.float32([[w // 2 - 150, h * .625], [w // 2 + 150, h * .625], [-100, h], [w + 100, h]])
    destination = np.float32([[100, 0], [w - 100, 0], [100, h], [w - 100, h]])
    transform_matrix = cv2.getPerspectiveTransform(source, destination)
    image = cv2.warpPerspective(img, transform_matrix, (w, h))
    edges = cv2.warpPerspective(edges, transform_matrix, (w, h))

    """ Find lines """
    histogram = np.sum(edges[int(h / 2):, :], axis=0)
    nonzero = edges.nonzero() # finds the indices where the (now warped) mask is true

    # Create empty lists to receive left and right lane pixel indices
    l_indices = np.empty([0], dtype=np.int)
    r_indices = np.empty([0], dtype=np.int)
    window_height = int(h / numWindows)

    # Determine where the windows need to be placed (windows are used for determining line coords based off of a cluster of nonzero pixels)
    for i in range(9):
        l_window = Window(
            y1=h - (i + 1) * window_height,
            y2=h - i * window_height,
            x=l_windows[-1].x if len(l_windows) > 0 else np.argmax(histogram[w // 2])
        )
        r_window = Window(
            y1=h - (i + 1) * window_height,
            y2=h - i * window_height,
            x=r_windows[-1].x if len(r_windows) > 0 else np.argmax(histogram[w // 2:]) + w // 2
        )

        # Append nonzero indices in the window boundary to the lists
        l_indices = np.append(l_indices, l_window.pixels_in(nonzero), axis=0)
        r_indices = np.append(r_indices, r_window.pixels_in(nonzero), axis=0)

        # Append respective windows
        l_windows.append(l_window)
        r_windows.append(r_window)

    # If either windows are empty, there is an error but we need to keep the cart going. Restart the image processing (hopefully the cart started already). 
    if ((len(l_windows) == 0) or (len(r_windows) == 0)):
        continue

    leftLine = Line(x=nonzero[1][l_indices], y=nonzero[0][l_indices], h = h, w = w)
    rightLine = Line(x=nonzero[1][r_indices], y=nonzero[0][r_indices], h=h, w = w)

    # Take the slopes and then average it
    slopeL = (leftLine.averaged_fit())
    slopeR = (rightLine.averaged_fit())
    slope = ((slopeL + slopeR) / 2)[0]
    
    # TODO: Train a RNN to determine the best slope and throttle
    # Send steering based on the slope
    if (slope > 0):
        if (slope > 0.25): 
            if (slope > 0.5): 
                writeData(255)
            else:
                writeData(191)
        else: 
            writeData(127)
    elif (slope <= 0):
        if (slope < -0.25):
            if slope( < -0.5):
                writeData(0)
            else: 
                writeData(63)
        else:
            writeData(35)
    # Send throttle (TODO: make better throttle parameters)
    writeData(127) # Is this a good parameter @Tyler? 
