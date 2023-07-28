#ifndef __DFROBOT_RTU_TIMER_H
#define __DFROBOT_RTU_TIMER_H
#include "stdint.h"
#include "stdio.h"
//给RTU的timer中断计数的标志位
extern volatile uint8_t DFRobot_RTU_Timer_Cnt;

/**
 * @fn DFRobotTimerInit
 * @brief 初始化一个给RTU串口接收用的timer
 * @details 初始化一个用来判断串口是否完成一帧数据的接收的timer。1ms产生一次中断
 * @return None
 * @note RTU串口处理是基于一帧数据来处理的，所以RTU主机每次发送的命令必须是以连续的方式发送的一串数据
 * @note 所以，RTU从机接收也必须判断什么时候这一帧数据接收完成，也就是串口数据的一帧完成
 */
extern void DFRobotTimerInit(void);
/**
 * @fn DFRobot_RTU_Timer_Handle
 * @brief RTU timer 中断处理函数
 * @return None
 */
extern void DFRobot_RTU_Timer_Handle(void);
#endif
