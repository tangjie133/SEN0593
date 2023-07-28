#ifndef _DFROBOTREGOPERATION_H
#define _DFROBOTREGOPERATION_H
#include "DFRobot_RTU_Config.h"
#include "stdint.h"
//#include "macro.h"

#define    EEPROM_BASE_ADDR            0x4000


#define    DEVICE_REG_NUM_TOTAL        0x17



#define    RTU_REG_ADDR_H_OFFSET       0x02
#define    RTU_REG_ADDR_L_OFFSET       0x03
#define    RTU_REG_NUM_H_OFFSET        0x04
#define    RTU_REG_NUM_L_OFFSET        0x05
#define    RTU_REG_VALUE_H_OFFSET      0x04
#define    RTU_REG_VALUE_L_OFFSET      0x05
#define    RTU_REG_BYTE_NUM_OFFSET     0x06

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

#define  RTU_READ_REG_CMD             0x03
#define  RTU_WRITE_REG_CMD            0x06
#define  RTU_WRITE_MULTIPLE_REG_CMD   0x10
#define  RTU_USER_DEFINED_CMD         0x41

struct sCustomRegister{
  struct sCustomRegister *next;
  uint16_t addr;
  uint16_t min;
  uint16_t max;
  uint8_t auth;
};

/**
 * @brief 保持寄存器的链表,具有可读可写属性
 * 
 */
struct sHoldingRegister{
  struct sHoldingRegister *next;
  uint16_t addr;
  uint16_t min;
  uint16_t max;
  uint8_t auth;
};  

/**
 * @brief 线圈寄存器的链表,具有可读可写属性
 * 
 */
struct sCoilRegister{
  struct sCoilRegister *next;
  uint16_t addr;
  uint16_t min;
  uint16_t max;
  uint8_t auth;
};  

/**
 * @brief 输入寄存器的链表,具有只读属性
 * 
 */
struct sInputRegister{
  struct sInputRegister *next;
  uint16_t addr;
  uint16_t min;
  uint16_t max;
  uint8_t auth;
};  

/**
 * @brief 离散输入寄存器的链表,具有只读属性
 * 
 */
struct sDiscreteInputRegister{
  struct sDiscreteInputRegister *next;
  uint16_t addr;
  uint16_t min;
  uint16_t max;
  uint8_t auth;
};  



extern struct sCustomRegister *cCustomRegisterHead;
extern struct sCustomRegister *cCustomRegisterTail;
extern struct sHoldingRegister *cHoldingRegisterHead;
extern struct sHoldingRegister *cHoldingRegisterTail;
extern struct sCoilRegister *cCoilRegisterHead;
extern struct sCoilRegister *cCoilRegisterTail;
extern struct sInputRegister *cInputRegisterHead;
extern struct sInputRegister *cInputRegisterTail;
extern struct sDiscreteInputRegister *cDiscreteInputRegisterHead;
extern struct sDiscreteInputRegister *cDiscreteInputRegisterTail;
/**
 * @brief 根据模块寄存器表，初始化寄存器参数属性
 * 
 */
extern void DFRobot_Init_Reg_Attribute(void);

void writeDataSensorReg(uint16_t StartRegAddr,uint8_t *data, uint8_t len);

#if HOLDING_REGISTER
/**
 * @fn attachHoldingRegister
 * @brief 添加保持寄存器的属性
 * 
 * @param addr 保持寄存器地址
 * @param min 该保持寄存器的最小值
 * @param max 该保持寄存器的最大值
 * @param auth 权限(保持寄存器是可读可写)
 */
void attachHoldingRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth);
#endif
#if COIL_REGISTER
/**
 * @fn attachCoilRegister
 * @brief 添加线圈寄存器的属性
 * 
 * @param addr 线圈寄存器地址
 * @param min 该线圈寄存器的最小值
 * @param max 该线圈寄存器的最大值
 * @param auth 权限(线圈寄存器是可读可写)
 */
void attachCoilRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth);
#endif

#if INPUT_REGISTER
/**
 * @fn attachInputRegister
 * @brief 添加输入寄存器的属性
 * 
 * @param addr 输入寄存器地址
 * @param min 该输入寄存器的最小值
 * @param max 该输入寄存器的最大值
 * @param auth 权限(输入寄存器是只读)
 */
void attachInputRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth);
#endif

#if DISCRETEINPUT_REGISTER
/**
 * @fn attachDiscreteInputRegister
 * @brief 添加离散输入寄存器的属性
 * 
 * @param addr 离散输入寄存器地址
 * @param min 该离散输入寄存器的最小值
 * @param max 该离散输入寄存器的最大值
 * @param auth 权限(离散输入寄存器是只读)
 */
void attachDiscreteInputRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth);
#endif
extern uint8_t DFRobot_Read_Reg(uint16_t rtuCmdCodeOffset, uint16_t StartRegAddr, uint16_t ReadRegNum);
extern uint8_t DFRobot_Write_One_Reg(uint16_t rtuCmdCodeOffset, uint16_t StartRegAddr, uint16_t RegValue);
extern uint8_t DFRobot_Write_Mult_Reg(uint16_t rtuCmdCodeOffset, uint16_t StartRegAddr, uint16_t ReadRegNum ,uint8_t *data, uint8_t len);
#endif
