#ifndef __DFROBOT_RTU_CONFIG_H
#define __DFROBOT_RTU_CONFIG_H

#define    RO                    0x01
#define    RW                    0x02

#define BIG_ENDIAN               1
#define HOLDING_REGISTER         1
#define COIL_REGISTER            0
#define INPUT_REGISTER           1
#define DISCRETEINPUT_REGISTER   0

#define DEVICE_PID        0x0251
#define DEVICE_VID        0x3334
#define DEVICE_ADDRESS    0x0023
#define DEVICE_VERSION    0x1000
#define BAUDRATE          0x0003
#define VERIFY_AND_STOP   0x0001

#define I2C_SPEED_RATE                  100 //i2c速率 100k
#define MODE_ONCE                       0
#define MODE_CONTINUOUS                 1


#define FLASH_PROGRAM_ADDRESS_START				0xFE00U		  // 64K
#define FLASH_PROGRAM_ADDRESS_END				0x10000U  	
#define FLASH_PROGRAM_ADDRESS_START_PID			0xFA00U		  // 64K
#define FLASH_PROGRAM_ADDRESS_END_PID			0xFC00U		  
	

#endif
