'''
Gather and store gps data from the arduino
save file into gpstestdata.txt
'''

#Library
from smbus2 import SMBusWrapper, SMBus #I2c #has to be installed
import time

#Global definitions
address = 0x04 # Slave Address, set on Arduino
channel = 1

#function to read size bytes
def readBlock(size): #Reads Arrays of size
	with SMBusWrapper(channel) as bus:
		value = bus.read_i2c_block_data(address,0,size)
	return value


def checkbus():  #uchecks to see if device is on other side of bue
	bus = SMBus(1)
	for device in range(128):
		try:
			bus.write_byte(device, 0, 0xFF)
			#print (hex(device))
			bus.close()
			return True; #if device found arduino ==  ready
		except: #if fail, don't kill program
			pass
	bus.close()
	return False;
	
def fixbytes(first_byte, second_byte, third_byte, fourth_byte):
	final_num = first_byte << 24 | second_byte << 16 | third_byte << 8 | fourth_byte
	return final_num;

def getgps():
	#open file
	fp = open("gpstestdata.txt", "a")
	
	busready = False
	#bus checking
	while busready != True:
		busready = checkbus()
		time.sleep(.25)
	
	# request value every second
	if (fp != None): #file error handling
		while True:
				busready = checkbus()
				if busready:
					#print("loop") #test
					time.sleep(1)
					direction = readBlock(24) #read values into direction array
					for i in range(0,21,4): # 6 ints * 4 bytes == 24
						number = fixbytes(direction[i],direction[i+1],direction[i+2],direction[i+3])
						fp.write(str(number) + ",")
						i += 4
					fp.write("\n")
getgps()
