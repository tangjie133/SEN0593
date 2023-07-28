#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cs32l010_hal.h"
#include "stdio.h"
#include "DFRobot_RTU_Config.h"
#include "DFRobot_RTU_Parse.h"
#include "DFRobot_RTU_Timer.h"
#include "DFRobot_RTU_User.h"
#include "DFRobot_REG_Operation.h"
#include <stdbool.h>
#include "log.h"
#include "DFRobot_TCS3430.h"
/* USER CODE END Includes */



	/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void SystemClock_Config(void);
	

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
