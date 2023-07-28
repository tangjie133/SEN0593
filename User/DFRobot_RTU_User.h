/**
 * @file DFRobot_RTU_User.h
 * @author your name (you@domain.com)
 * @brief 通常只需要更改该文件和对应的.c文件的适配函数即变量，即可完成整套RTU适配不同硬件平台的功能
 * @version 0.1
 * @date 2022-06-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __DFROBOT_RTU_USER_H
#define __DFROBOT_RTU_USER_H
#include "stdint.h"
#include "stdbool.h"
#include "DFRobot_RTU_Config.h"





#if HOLDING_REGISTER
/**
 * @brief 根据模块实际情况自定义保持寄存器
 * 
 */
struct HoldingRegList{
    uint16_t REG_CONFIG;
    //... REG_CUSTOMIZE;
};
extern struct HoldingRegList sHoldingReg;
#endif

#if INPUT_REGISTER
/**
 * @brief 根据模块实际情况自定义输入寄存器
 * 
 */
struct InputRegList{
    uint16_t REG_PID;
    uint16_t REG_VID;
    uint16_t REG_ADDR;
    uint16_t REG_BAUDRATE;
    uint16_t REG_VERIFY_AND_STOP;
    uint16_t REG_VERSION;
    uint16_t REG_LUX;//光线强度
    uint16_t REG_CCT;//色温
    uint16_t REG_IR1;//红外值1
    uint16_t REG_IR2;//红外值2
    uint16_t REG_LOCATION;//传感器位置
    uint16_t REG_x;//小X
    uint16_t RGB_y;//小y
    //... REG_CUSTOMIZE;
};
extern struct InputRegList sInputReg;
#endif

#if COIL_REGISTER
    //待定
#endif
#if DISCRETEINPUT_REGISTER
    //待定
#endif

extern volatile uint8_t RecIndexLen;
extern uint8_t ucReceiveData[255];
/**
 * @fn DFRobot_RTU_Response
 * @brief 响应RTU协议通信的函数，由自己实现其具体通信方式，例如是串口通信，则只需实现串口发送函数即可
 * 
 * @param data 具体RTU响应包数据
 * @param len 响应包数据长度
 */
extern void DFRobot_RTU_Response(uint8_t *data, uint8_t len);

/**
 * @fn DFRobot_RTU_Write_HoldingReg_CB
 * @brief 写保持寄存器的回调函数，可根据实际情况处理数据，配置传感器或存flash或eeprom等。
 * 
 * @param StartRegAddr 准备写入寄存器的起始地址
 * @param data 需要写入的数据
 * @param len 需要写入多少个寄存器
 * @return bool 返回是否成功
 */
extern bool DFRobot_RTU_Write_HoldingReg_CB(uint16_t StartRegAddr, uint8_t *data, uint8_t len);

/**
 * @fn DFRobot_RTU_Read_HoldingReg_CB
 * @brief 读保持寄存器的回调函数，可根据实际情况处理该函数
 * 
 * @param StartRegAddr 准备读取的保持寄存器起始地址
 * @param RegNum 准备读取的保持寄存器个数
 */
extern void DFRobot_RTU_Read_HoldingReg_CB(uint16_t StartRegAddr, uint8_t RegNum);

/**
 * @fn DFRobot_RTU_Read_InputReg_CB
 * @brief 读取输入寄存器的回调函数，可根据实际情况处理该函数
 * 
 * @param StartRegAddr 准备读取的输入寄存器起始地址
 * @param RegNum 准备读取的输入寄存器个数
 */
extern void DFRobot_RTU_Read_InputReg_CB(uint16_t StartRegAddr, uint8_t RegNum);
#endif
