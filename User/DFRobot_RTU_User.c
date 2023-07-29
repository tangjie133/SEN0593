/**
 * @file DFRobot_RTU_User.c
 * @author your name (you@domain.com)
 * @brief 通常只需要更改该文件和对应的.c文件的适配函数即变量，即可完成整套RTU适配不同硬件平台的功能
 * @version 0.1
 * @date 2022-06-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "DFRobot_REG_Operation.h"
#include "DFRobot_RTU_User.h"
#include "DFRobot_RTU_Timer.h"
#include "DFRobot_RTU_Parse.h"
#include "stdlib.h"
#if HOLDING_REGISTER
struct HoldingRegList sHoldingReg;
#endif
#if INPUT_REGISTER
struct InputRegList sInputReg;
#endif
extern UART_HandleTypeDef sUartxHandle;
extern FLASH_EraseInitTypeDef 	sFlashEraseInit;
uint32_t uiErrorPage;
uint32_t holdingregAddr = 0;
extern void Error_Handler(void);

extern uint8_t mode;
void DFRobot_Init_Reg_Attribute(void){
    //输入寄存器和保持寄存器前6个寄存器互斥，共同维护默认的6个寄存器，具体详解可看协议规范定义
#if INPUT_REGISTER
    attachInputRegister(0x0000, 0x0000, 0xFFFF, RO);        //PID                    设备PID
    attachInputRegister(0x0001, 0x3343, 0x3343, RO);        //VID                    设备的VID,固定为0x3343
    attachInputRegister(0x0002, 0x0001, 0x00F7, RO);        //ADDR                   模块的设备地址
    attachInputRegister(0x0003, 0x0000, 0xFFFF, RO);        //BAUD                   串口波特率
    attachInputRegister(0x0004, 0x0000, 0xFFFF, RO);        //STOPBIT                串口校验位和停止位
    attachInputRegister(0x0005, 0x0000, 0xFFFF, RO);        //VERSION                固件版本信息
    attachInputRegister(0x0006, 0x0000, 0xFFFF, RO);        //LUX                    光线强度
    attachInputRegister(0x0007, 0x0000, 0xFFFF, RO);        //CCT                    色温
    attachInputRegister(0x0008, 0x0000, 0xFFFF, RO);        //IR1                    红外值1
    attachInputRegister(0x0009, 0x0000, 0xFFFF, RO);        //IR2                    红外值2
    attachInputRegister(0x000A, 0x0000, 0xFFFF, RO);        //LOCATION               位置
    //attachInputRegister(......, ......, ......, RO);      //CUSTOMIZE              自定义输入寄存器

    //输入寄存器赋初值
    sInputReg.REG_PID = DEVICE_PID;
    sInputReg.REG_VID = DEVICE_VID;
    sInputReg.REG_ADDR = DEVICE_ADDRESS;
    sInputReg.REG_BAUDRATE = BAUDRATE;
    sInputReg.REG_VERIFY_AND_STOP = VERIFY_AND_STOP;
    sInputReg.REG_VERSION = DEVICE_VERSION;
    //sInputReg.REG_... = ...;
#endif
#if HOLDING_REGISTER
    attachHoldingRegister(0x0000, 0x0000, 0xFFFF, RW);      //CONFIG_X                     保留寄存器
    attachHoldingRegister(0x0001, 0x0000, 0xFFFF, RW);      //CONFIG_Y                     保留寄存器
    attachHoldingRegister(0x0002, 0x0000, 0xFFFF, RW);      //CONFIG_Z                     保留寄存器
    attachHoldingRegister(0x0003, 0x0000, 0xFFFF, RW);      //CONFIG_IR1                    保留寄存器
    attachHoldingRegister(0x0004, 0x0000, 0xFFFF, RW);      //CONFIG_2                     保留寄存器
    attachHoldingRegister(0x0005, 0x0000, 0xFFFF, RW);      //RESERVED                     保留寄存器
    attachHoldingRegister(0x0006, 0x0000, 0xFFFF, RW);      //MODEL                        配置传感器上报数据模式
    // attachHoldingRegister(0x0007, 0x0000, 0xFFFF, RW);      //CONFIG_X                     X系数
    // attachHoldingRegister(0x0008, 0x0000, 0xFFFF, RW);      //CONFIG_Y                     Y系数
    // attachHoldingRegister(0x0009, 0x0000, 0xFFFF, RW);      //CONFIG_Z                     Z系数
    // attachHoldingRegister(0x000A, 0x0000, 0xFFFF, RW);      //CONFIG_IR1                   IR1系数
    //attachHoldingRegister(......, ......, ......, RW);      //CUSTOMIZE               自定义保持寄存器
    //保持寄存器赋初值
    holdingregAddr = FLASH_PROGRAM_ADDRESS_START_PID;//flash起始地址
    if(*(uint8_t*)(holdingregAddr) != 0x1f){//是否被写入默认值
        sFlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;	// 页擦除 
        sFlashEraseInit.PageAddress = FLASH_PROGRAM_ADDRESS_START; // 开始擦除地址 必须以512Byte为单位
        sFlashEraseInit.NbPages	= (FLASH_PROGRAM_ADDRESS_END - FLASH_PROGRAM_ADDRESS_START)/FLASH_PAGE_SIZE + 1;
        if(HAL_FLASH_Erase(&sFlashEraseInit, &uiErrorPage) != HAL_OK){
            Error_Handler();
        }
        holdingregAddr = FLASH_PROGRAM_ADDRESS_START;
        while (holdingregAddr < FLASH_PROGRAM_ADDRESS_END){
            if (*(uint8_t*)(holdingregAddr) != 0xFF){	// 判断地址数据是否擦除成功		
                holdingregAddr = holdingregAddr + 1;			
            }else{
                holdingregAddr = holdingregAddr + 1;
            }
        }
        sFlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;	// 页擦除 
        sFlashEraseInit.PageAddress = FLASH_PROGRAM_ADDRESS_START_PID; // 开始擦除地址 必须以512Byte为单位
        sFlashEraseInit.NbPages	= (FLASH_PROGRAM_ADDRESS_END_PID - FLASH_PROGRAM_ADDRESS_START_PID)/FLASH_PAGE_SIZE + 1;
        if(HAL_FLASH_Erase(&sFlashEraseInit, &uiErrorPage) != HAL_OK){
            Error_Handler();
        }
        holdingregAddr = FLASH_PROGRAM_ADDRESS_START_PID;
        while (holdingregAddr < FLASH_PROGRAM_ADDRESS_END_PID){
            if (*(uint8_t*)(holdingregAddr) != 0xFF){	// 判断地址数据是否擦除成功		
                holdingregAddr = holdingregAddr + 1;			
            }else{
                holdingregAddr = holdingregAddr + 1;
            }
        }
        
        uint8_t flage = 0x1f;
        holdingregAddr = FLASH_PROGRAM_ADDRESS_START_PID;
				if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, holdingregAddr, flage) == HAL_OK){	// 写入操作		
                holdingregAddr = holdingregAddr + 1;
            }			
        sHoldingReg.REG_RESERVED5 = 0;
        sHoldingReg.REG_CONFIG = 0;
        sHoldingReg.REG_CONFIG_X = 1000;
        sHoldingReg.REG_CONFIG_Y = 1000;
        sHoldingReg.REG_CONFIG_Z = 1000;
        sHoldingReg.REG_CONFIG_IR1 = 1000;
        sHoldingReg.REG_CONFIG_Z2 = 1000;
        
        //sHoldingReg.REG_... = ...;
        uint8_t flashData = 0;
        holdingregAddr = FLASH_PROGRAM_ADDRESS_START;
        for(uint8_t i = 0; i < sizeof(sHoldingReg); i++){//将默认值写入flash
            memcpy(&flashData,(uint8_t*)&sHoldingReg+i,sizeof(uint8_t));
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, holdingregAddr, flashData) == HAL_OK){	// 写入操作		
                holdingregAddr = holdingregAddr + 1;
            }
        }
    }else{
        holdingregAddr = FLASH_PROGRAM_ADDRESS_START;
        memcpy(&sHoldingReg,&*(uint8_t*)(holdingregAddr),sizeof(sHoldingReg));
    }
    
#endif
#if COIL_REGISTER
    //待定
#endif
#if DISCRETEINPUT_REGISTER
    //待定
#endif
}
uint8_t *RTU_UART_Data = NULL;
void DFRobot_RTU_Response(uint8_t *data, uint8_t len){
    RTU_UART_Data = malloc(len+1);
    memcpy(RTU_UART_Data, data, len);
    HAL_UART_Transmit_IT(&sUartxHandle, RTU_UART_Data, len);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(RTU_UART_Data != NULL){
        free(RTU_UART_Data);
        RTU_UART_Data = NULL;
    }
}
uint8_t state = 0;

bool DFRobot_RTU_Write_HoldingReg_CB(uint16_t StartRegAddr, uint8_t *data, uint8_t len){
    uint16_t _data = 0;
    if(StartRegAddr == 0x06){
        if(data[0] == 0){
            state = 0;
            mode = data[0];
        }else if(data[0] == 3){
            if(state == 1)
                mode = 2;
        }else if(data[0] == 1){
            mode = 1;
            state = 1;
        }else if(data[0] == 2){
            if(state == 1)
                mode = 1;
            
        }
    }
    else if(StartRegAddr == 0x00){
        _data = data[1] << 8 | data[0];
        sHoldingReg.REG_CONFIG_X = _data;
        _data = data[3] << 8 | data[2];
        sHoldingReg.REG_CONFIG_Y = _data;
        _data = data[5] << 8 | data[4];
        sHoldingReg.REG_CONFIG_Z = _data;
        _data = data[7] << 8 | data[6];
        sHoldingReg.REG_CONFIG_IR1 = _data;
        _data = data[9] << 8 | data[8];
        sHoldingReg.REG_CONFIG_Z2 = _data;
        sFlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;	// 页擦除 
        sFlashEraseInit.PageAddress = FLASH_PROGRAM_ADDRESS_START; // 开始擦除地址 必须以512Byte为单位
        sFlashEraseInit.NbPages	= (FLASH_PROGRAM_ADDRESS_END - FLASH_PROGRAM_ADDRESS_START)/FLASH_PAGE_SIZE + 1;
        if(HAL_FLASH_Erase(&sFlashEraseInit, &uiErrorPage) != HAL_OK){
            Error_Handler();
        }
        holdingregAddr = FLASH_PROGRAM_ADDRESS_START;
        while (holdingregAddr < FLASH_PROGRAM_ADDRESS_END){
            if (*(uint8_t*)(holdingregAddr) != 0xFF){	// 判断地址数据是否擦除成功		
                holdingregAddr = holdingregAddr + 1;			
            }else{
                holdingregAddr = holdingregAddr + 1;
            }
        }
        holdingregAddr = FLASH_PROGRAM_ADDRESS_START;
        uint8_t flashData = 0;
        for(uint8_t i = 0; i < sizeof(sHoldingReg); i++){//将修改后得数据写入flash 
            memcpy(&flashData,(uint8_t*)&sHoldingReg+i,sizeof(uint8_t));
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, holdingregAddr, flashData) == HAL_OK){	// 写入操作		
                holdingregAddr = holdingregAddr + 1;
            }
        }
    }
    
    return true;
}

void DFRobot_RTU_Read_HoldingReg_CB(uint16_t StartRegAddr, uint8_t RegNum){
    
}

void DFRobot_RTU_Read_InputReg_CB(uint16_t StartRegAddr, uint8_t RegNum){
    
}

/*
void Example_Main(void){
    DFRobot_Init_Reg_Attribute();
    while(1){
        if(DFRobot_RTU_Timer_Cnt > 5){// 连续5ms没有收到数据，则认为接收完一帧数据了
            if(RecIndexLen > 0 ){
                DFRobot_RTU_Parse(ucReceiveData, RecIndexLen);
                RecIndexLen = 0;
            }else{
                DFRobot_RTU_Timer_Cnt = 0;
            }
        }
    }
}
*/


