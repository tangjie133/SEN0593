#include "DFRobot_RTU_Timer.h"


extern void timer2_init();
volatile uint8_t DFRobot_RTU_Timer_Cnt = 0;
void DFRobotTimerInit(void){
  //根据实际情况，更改适合的timer初始化
  timer2_init();
  DFRobot_RTU_Timer_Cnt = 0;
}

void DFRobot_RTU_Timer_Handle(void){
  DFRobot_RTU_Timer_Cnt++;
}



