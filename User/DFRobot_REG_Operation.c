#include "DFRobot_REG_Operation.h"
#include "DFRobot_RTU_Parse.h"
#include "DFRobot_RTU_Config.h"
#include "DFRobot_RTU_User.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "main.h"

struct sCustomRegister *cCustomRegisterHead = NULL;
struct sCustomRegister *cCustomRegisterTail = NULL;

#if HOLDING_REGISTER
struct sHoldingRegister *cHoldingRegisterHead = NULL;
struct sHoldingRegister *cHoldingRegisterTail = NULL;
#endif

#if COIL_REGISTER
struct sCoilRegister *cCoilRegisterHead = NULL;
struct sCoilRegister *cCoilRegisterTail = NULL;
#endif

#if INPUT_REGISTER
struct sInputRegister *cInputRegisterHead = NULL;
struct sInputRegister *cInputRegisterTail = NULL;
#endif

#if DISCRETEINPUT_REGISTER
struct sDiscreteInputRegister *cDiscreteInputRegisterHead = NULL;
struct sDiscreteInputRegister *cDiscreteInputRegisterTail = NULL;
#endif


#if HOLDING_REGISTER
void attachHoldingRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth){
  struct sHoldingRegister *p;
  p = (struct sHoldingRegister*)malloc(sizeof(struct sHoldingRegister)+1);
  if(p == NULL){
    return;
  }
  p->next = NULL;
  if(cHoldingRegisterHead == NULL){
    cHoldingRegisterHead = p;
    cHoldingRegisterTail=p;
  }else{
    cHoldingRegisterTail->next = p;
    cHoldingRegisterTail = p;
  }
  p->addr = addr;
  p->min = min;
  p->max = max;
  p->auth = auth;
}

/**
 * @fn FindHoldingP
 * @brief 查询是否有输入地址的保持寄存器
 * 
 * @param offset 想要查询的地址
 * @return struct sHoldingRegister* (有则返回该节点，无则返回NULL)
 */
static struct sHoldingRegister* FindHoldingP(uint16_t offset){
  struct sHoldingRegister *p;
  p = cHoldingRegisterHead;
  while(offset){
    if(p == NULL){
      break;
    }
    p = p->next;
    offset--;
  }
  return p;
}
#endif

#if COIL_REGISTER
void attachCoilRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth){
  struct sCoilRegister *p;
  p = (struct sCoilRegister*)malloc(sizeof(struct sCoilRegister)+1);
  if(p == NULL){
    return;
  }
  p->next = NULL;
  if(cCoilRegisterHead == NULL){
    cCoilRegisterHead = p;
    cCoilRegisterTail=p;
  }else{
    cCoilRegisterTail->next = p;
    cCoilRegisterTail = p;
  }
  p->addr = addr;
  p->min = min;
  p->max = max;
  p->auth = auth;
}

/**
 * @fn FindCoilP
 * @brief 查询是否有输入地址的线圈寄存器
 * 
 * @param offset 想要查询的地址
 * @return struct sCoilRegister* (有则返回该节点，无则返回NULL)
 */
static struct sCoilRegister* FindCoilP(uint16_t offset){
  struct sCoilRegister *p;
  p = cCoilRegisterHead;
  while(offset){
    if(p == NULL){
      break;
    }
    p = p->next;
    offset--;
  }
  return p;
}
#endif

#if INPUT_REGISTER
void attachInputRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth){
  struct sInputRegister *p;
  p = (struct sInputRegister*)malloc(sizeof(struct sInputRegister)+1);
  if(p == NULL){
    return;
  }
  p->next = NULL;
  if(cInputRegisterHead == NULL){
    cInputRegisterHead = p;
    cInputRegisterTail=p;
  }else{
    cInputRegisterTail->next = p;
    cInputRegisterTail = p;
  }
  p->addr = addr;
  p->min = min;
  p->max = max;
  p->auth = auth;
}

/**
 * @fn FindInputP
 * @brief 查询是否有输入地址的输入寄存器
 * 
 * @param offset 想要查询的地址
 * @return struct sInputRegister* (有则返回该节点，无则返回NULL)
 */
static struct sInputRegister* FindInputP(uint16_t offset){
  struct sInputRegister *p;
  p = cInputRegisterHead;
  while(offset){
    if(p == NULL){
      break;
    }
    p = p->next;
    offset--;
  }
  return p;
}
#endif

#if DISCRETEINPUT_REGISTER
void attachDiscreteInputRegister(uint16_t addr,uint16_t min, uint16_t max, uint8_t auth){
  struct sDiscreteInputRegister *p;
  p = (struct sDiscreteInputRegister*)malloc(sizeof(struct sDiscreteInputRegister)+1);
  if(p == NULL){
    return;
  }
  p->next = NULL;
  if(cDiscreteInputRegisterHead == NULL){
    cDiscreteInputRegisterHead = p;
    cDiscreteInputRegisterTail=p;
  }else{
    cDiscreteInputRegisterTail->next = p;
    cDiscreteInputRegisterTail = p;
  }
  p->addr = addr;
  p->min = min;
  p->max = max;
  p->auth = auth;
}

/**
 * @fn FindDiscreteInputP
 * @brief 查询是否有输入地址的离散输入寄存器
 * 
 * @param offset 想要查询的地址
 * @return struct sDiscreteInputRegister* (有则返回该节点，无则返回NULL)
 */
static struct sDiscreteInputRegister* FindDiscreteInputP(uint16_t offset){
  struct sDiscreteInputRegister *p;
  p = cDiscreteInputRegisterHead;
  while(offset){
    if(p == NULL){
      break;
    }
    p = p->next;
    offset--;
  }
  return p;
}
#endif

static void Calculate_CRC_AND_Response(uint8_t *data, uint8_t len){
  uint16_t tempCrc = 0;
  tempCrc = Calculate_CRC(data, len-2);
  data[len-2] = tempCrc>>8 & 0xFF;
  data[len-1] = tempCrc & 0xFF;
  DFRobot_RTU_Response(data,len);
}

static void DFRobot_Response_OK(uint8_t cmd, uint16_t StartRegAddr, uint16_t NumOrValue){
  uint8_t tempData[8];
  tempData[0] =DEVICE_ADDRESS;
  tempData[1] = cmd;
	tempData[2] = StartRegAddr >> 8;
	tempData[3] = StartRegAddr & 0xFF;
	tempData[4] = NumOrValue >> 8;
	tempData[5] = NumOrValue & 0xFF;
  Calculate_CRC_AND_Response(tempData, 8);
}

//读取寄存器的值
uint8_t DFRobot_Read_Reg(uint16_t rtuCmdCodeOffset, uint16_t StartRegAddr, uint16_t ReadRegNum){
	uint8_t ret = 0;
  uint8_t *tempData = NULL;
  tempData = (uint8_t *)malloc(ReadRegNum*2+5);
  if(tempData == NULL){
    //printf("READ Allocate memory.");//从设备故障
    return RTU_SLAVE_DEV_FAULT;
  }
  tempData[0] = DEVICE_ADDRESS;
  tempData[1] = rtuCmdCodeOffset;
  tempData[2] = ReadRegNum * 2;
  switch(rtuCmdCodeOffset){
    case CMD_READ_HOLDING:
      DFRobot_RTU_Read_HoldingReg_CB(StartRegAddr, ReadRegNum);
      memcpy(&tempData[3],(uint8_t *)&sHoldingReg + StartRegAddr*2,ReadRegNum*2);
      #if BIG_ENDIAN
        for(uint8_t i = 0; i< ReadRegNum*2; i+=2){
          uint8_t temp = tempData[3 + i];
          tempData[3 + i] = tempData[3 + i + 1];
          tempData[3 + i + 1] = temp;
        }
      #endif
      break;
    case CMD_READ_INPUT:
      DFRobot_RTU_Read_InputReg_CB(StartRegAddr, ReadRegNum);
      memcpy(&tempData[3],(uint8_t *)&sInputReg + StartRegAddr*2,ReadRegNum*2);
      #if BIG_ENDIAN
        for(uint8_t i = 0; i< ReadRegNum*2; i+=2){
          uint8_t temp = tempData[3 + i];
          tempData[3 + i] = tempData[3 + i + 1];
          tempData[3 + i + 1] = temp;
        }
      #endif
      break;
    default:
      break;
  }
  Calculate_CRC_AND_Response(tempData, ReadRegNum*2 + 5);
  free(tempData);
  return ret;
}


//写单个寄存器的值
uint8_t DFRobot_Write_One_Reg(uint16_t rtuCmdCodeOffset, uint16_t StartRegAddr, uint16_t RegValue){
  uint8_t ret = 0;
  uint16_t data = RegValue;
  if(rtuCmdCodeOffset == CMD_WRITE_HOLDING){
    struct sHoldingRegister *p;
    p = FindHoldingP(StartRegAddr);
    if(p == NULL){
      ret = RTU_ILLEGAL_DATA_ADDR;//寄存器不存在
      return ret;
    }
    if(RegValue > p->max || RegValue < p->min){//所写值超过寄存器的属性范围
      ret = RTU_ILLEGAL_DATA;
    }else{
      if(!DFRobot_RTU_Write_HoldingReg_CB(StartRegAddr,(uint8_t *)&data,1)){
        ret = RTU_ILLEGAL_ERROR;
      }else{
        memcpy((uint8_t *)&sHoldingReg + StartRegAddr*2, &data, sizeof(data));
        DFRobot_Response_OK(rtuCmdCodeOffset, StartRegAddr, RegValue);
      }
    }
  }else{
    ret = RTU_ILLEGAL_ERROR;
  }
  return ret;
}


//写多个寄存器的值
uint8_t DFRobot_Write_Mult_Reg(uint16_t rtuCmdCodeOffset, uint16_t StartRegAddr, uint16_t RegNum ,uint8_t *data, uint8_t len){
  uint8_t ret = 0;
  uint8_t temp = RegNum, i = 0;
  uint16_t tempValue = 0;
  if(rtuCmdCodeOffset == CMD_WRITE_MULTI_HOLDING){
    struct sHoldingRegister *p;
    p = FindHoldingP(StartRegAddr);
    while(temp){
      tempValue = (uint16_t)(data[7 + i]<<8 | data[8 + i]);
      if(p == NULL){
        return RTU_ILLEGAL_DATA_ADDR;//寄存器不存在
      }
      if(tempValue > p->max || tempValue < p->min){//检测数据是否合法
        return RTU_ILLEGAL_DATA;
      }
      i += 2;
      p = p->next;
      temp--;
    }
    #if BIG_ENDIAN
      for(uint8_t i = 0; i < RegNum*2; i+=2){
          uint8_t temp = data[7 + i];
          data[7 + i] = data[7 + i + 1];
          data[7 + i + 1] = temp;
      }
    #endif
    if(!DFRobot_RTU_Write_HoldingReg_CB(StartRegAddr,(uint8_t *)&data[7],RegNum)){
      ret = RTU_ILLEGAL_ERROR;
    }else{
      //memcpy((uint8_t *)&sHoldingReg + StartRegAddr*2, &data[7], RegNum*2);
      DFRobot_Response_OK(rtuCmdCodeOffset, StartRegAddr, RegNum);
    }
  }else{
    ret = RTU_ILLEGAL_ERROR;
  }
  return ret;
}
