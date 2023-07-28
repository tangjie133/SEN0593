#ifndef __DFROBOT_RTU_PARSE_H
#define __DFROBOT_RTU_PARSE_H
#include "cs32l010_hal.h"
//#include "macro.h"

#define  BROADCAST_ADDRESS            0x00 
#define  READREGLEN                   0x18

#define  RTU_ADDR_OFFSET              0x00
#define  RTU_CMD_CODE_OFFSET          0x01
#define  RTU_ERROR_CODE_OFFSET        0x01
#define  RTU_EXCEPTION_CODE_OFFSET    0x02
#define  RTU_ERROR_CRC_H_OFFSET       0x03
#define  RTU_ERROR_CRC_L_OFFSET       0x04

#define  RTU_ILLEGAL_ERROR            0x01
#define  RTU_ILLEGAL_DATA_ADDR        0x02
#define  RTU_ILLEGAL_DATA             0x03
#define  RTU_SLAVE_DEV_FAULT          0x04    
#define  RTU_CRC_ERROR                0x08

#define  CMD_READ_COILS 					    0x01
#define  CMD_READ_DISCRETE  					0x02
#define  CMD_READ_HOLDING   					0x03
#define  CMD_READ_INPUT   						0x04
#define  CMD_WRITE_COILS  						0x05
#define  CMD_WRITE_HOLDING   					0x06
#define  CMD_WRITE_MULTI_COILS  			0x0F
#define  CMD_WRITE_MULTI_HOLDING   		0x10

extern void UartSend(uint8_t *data, uint8_t len);
extern void SoftUartSend(uint8_t *data, uint8_t len);

extern uint16_t Calculate_CRC(uint8_t *data, uint8_t len);
extern void DFRobot_RTU_Parse(uint8_t *data, uint8_t len);



#endif
