#include "i2c.h" 
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
	// open the i2c bus and check whether it opened properly
	int bus;
	if ((bus = i2c_open("/dev/i2c-0")) == -1)
	{
		//std::cout << "Could not open the i2c bus" << std::endl; 
		return -1; 
	}

	// initialize the slave device
	I2CDevice slave;
	i2c_init_device(&slave); 
	slave.addr = 0x04; 

	// write message to the slave
	char* buffer = (char*) malloc(sizeof(char) * 5); 
	buffer[0] = '*'; 
	buffer[1] = 0x32; 
	buffer[2] = 0x69; 
	buffer[3] = '!'; 
	buffer[4] = '!'; 

	i2c_write(&slave, 0x0, "*AB!!", 5);  

	i2c_close(bus); 
}
