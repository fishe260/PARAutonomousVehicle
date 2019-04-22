'''
I2C
RPi to Aruduino I2C Functions

Wiring
RPi - Arduino Uno
Pin 2 SDA - Pin A4
Pin 3 SDL - Pin A5


'''

from smbus2 import SMBusWrapper #

address = 0x04 #Address of the connected Arduino
channel = 1 #channel 1 for Rpi3

def writeData(value): #Writes Bytes/ints
	with SMBusWrapper(channel) as bus:
		bus.write_byte_data(address,0,value) #Write value to address with 0 offset
	return True
	
def writeBlock(value): #Writes Arrays
	with SMBusWrapper(channel) as bus:
		bus.write_i2c_block_data(address,0,value) #Write array value to address with 0 offset
	return True
	
def readData(): #Reads Bytes/ints
	with SMBusWrapper(channel) as bus:
		value = bus.read_byte_data(address,0)
	return value
	
def readBlock(size): #Reads Arrays of size
	with SMBusWrapper(channel) as bus:
		value = bus.read_i2c_block_data(address,0,size)
	return value
	
