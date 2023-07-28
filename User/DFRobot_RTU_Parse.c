#include "DFRobot_RTU_Parse.h"
#include "DFRobot_REG_Operation.h"
#include "DFRobot_RTU_User.h"
#include "DFRobot_RTU_Config.h"
#include "stdio.h"
extern uint8_t mode;

uint16_t Calculate_CRC(uint8_t *data, uint8_t len){
  uint16_t crc = 0xFFFF;
  for( uint8_t pos = 0; pos < len; pos++){
    crc ^= (uint16_t)data[ pos ];
    for(uint8_t i = 8; i != 0; i--){
      if((crc & 0x0001) != 0){
        crc >>= 1;
        crc ^= 0xA001;
      }else{
         crc >>= 1;
      }
    }
  }
  crc = ((crc & 0x00FF) << 8) | ((crc & 0xFF00) >> 8);
  return crc;
}

//解析串口接收到的一帧数据包
/* *
 *  =================================================================
 *  |地址|功能码|寄存器地址高位|寄存器地址低位|......|CRC低位|CRC高位|
 *  =================================================================
 */
void DFRobot_RTU_Parse(uint8_t *data, uint8_t len){
  uint8_t RtuErrCode = 0;
  uint16_t CalCRC = 0, RecCRC = 0;
  if(data[RTU_ADDR_OFFSET] == DEVICE_ADDRESS || (data[RTU_ADDR_OFFSET] == BROADCAST_ADDRESS)){//当接收到地址为本机地址或者为广播地址时
    CalCRC = Calculate_CRC( data, len - 2);                          //计算得到的crc
    RecCRC = (uint16_t)((data[len - 2] << 8) | data[len - 1]);       //接收的数据中的CRC
    if(CalCRC != RecCRC ){
      RtuErrCode = RTU_CRC_ERROR;
    }else{
      uint16_t StartRegAddr = (uint16_t)(data[RTU_REG_ADDR_H_OFFSET]<<8 | data[RTU_REG_ADDR_L_OFFSET]);
      uint16_t RegNumOrValue = (uint16_t)(data[RTU_REG_NUM_H_OFFSET]<<8 | data[RTU_REG_NUM_L_OFFSET]);
			//printf("%d", data[RTU_CMD_CODE_OFFSET]);
				switch(data[RTU_CMD_CODE_OFFSET]){//功能码 
					case CMD_READ_COILS://读单个/多个线圈寄存器命令
						RtuErrCode = ((RegNumOrValue + StartRegAddr) > READREGLEN) ? RTU_ILLEGAL_DATA_ADDR : RTU_ILLEGAL_ERROR; 
						break;
					case CMD_READ_DISCRETE://读单个/多个离散输入寄存器命令
						RtuErrCode = ((RegNumOrValue + StartRegAddr) > READREGLEN) ? RTU_ILLEGAL_DATA_ADDR : RTU_ILLEGAL_ERROR;
						break;
					case CMD_READ_HOLDING://读单个/多个保持寄存器命令
						RtuErrCode = ((RegNumOrValue + StartRegAddr) > sizeof(sHoldingReg)/2) ? RTU_ILLEGAL_DATA_ADDR : DFRobot_Read_Reg(CMD_READ_HOLDING, StartRegAddr, RegNumOrValue);
						break;
					case CMD_READ_INPUT://读单个/多个输入寄存器命令
            if(mode == 0)
						  RtuErrCode =  ((RegNumOrValue + StartRegAddr) > sizeof(sInputReg)/2) ? RTU_ILLEGAL_DATA_ADDR : DFRobot_Read_Reg(CMD_READ_INPUT, StartRegAddr, RegNumOrValue);
						break;
					case CMD_WRITE_COILS://写单个线圈寄存器命令
						RtuErrCode = ((RegNumOrValue + StartRegAddr) > READREGLEN) ? RTU_ILLEGAL_DATA_ADDR : RTU_ILLEGAL_ERROR;
						break;
					case CMD_WRITE_HOLDING://写单个保持寄存器命令
						RtuErrCode =  DFRobot_Write_One_Reg(CMD_WRITE_HOLDING, StartRegAddr, RegNumOrValue);
						break;
					case CMD_WRITE_MULTI_COILS://写多个线圈寄存器命令
						RtuErrCode =  RTU_ILLEGAL_ERROR;
						break;
					case CMD_WRITE_MULTI_HOLDING://写多个保持寄存器命令
						RtuErrCode = DFRobot_Write_Mult_Reg(CMD_WRITE_MULTI_HOLDING, StartRegAddr, RegNumOrValue, data, len);
						break;
					default://其他错误命令码
						RtuErrCode = RTU_ILLEGAL_ERROR;
						break;
				}
    }
    if(RtuErrCode > 0){//说明应该返回错误码
      static uint8_t SendData[5] = {0};
      SendData[RTU_ADDR_OFFSET] = DEVICE_ADDRESS;
      SendData[RTU_ERROR_CODE_OFFSET] = 0x80 | data[RTU_CMD_CODE_OFFSET];
      SendData[RTU_EXCEPTION_CODE_OFFSET] = RtuErrCode;
      CalCRC = Calculate_CRC(SendData, 3);
      SendData[RTU_ERROR_CRC_H_OFFSET] = (uint8_t)(CalCRC >> 8);
      SendData[RTU_ERROR_CRC_L_OFFSET] = (uint8_t)(CalCRC & 0xFF);
      DFRobot_RTU_Response(SendData,5);
    }
  }
}
