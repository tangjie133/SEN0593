/**
  ******************************************************************************
  * @file    main.c
	* @author  MCU Software Team
	* @Version V1.0.0
  * @Date    21-Oct-2019
  * @brief   main function
  ******************************************************************************
  * This example is used to test uart0,when interrupt received data reaches 10, 
  * it is sent out from serial port.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"	
#include "DFRobot_RTU_Timer.h"
#include "DFRobot_RTU_Parse.h"

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t ucReceiveData[255] = {0};
uint16_t readCount = 0,sendCount = 0;
uint8_t mode  = 0;

//static uint8_t ucRxCompleteFlag = 0;
volatile uint8_t receData;
volatile uint8_t RecIndexLen = 0;

/* Private macros-------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
UART_HandleTypeDef sUartxHandle = {0};
TIM_HandleTypeDef sTim2Handle = {0};
FLASH_EraseInitTypeDef 	sFlashEraseInit = {0};	
extern struct HoldingRegList sHoldingReg;
float x1 = 0.882,y1 = 0.817,ir1 = 0.75,z1 = 0.73,z2 = 0.665;

void timer2_init(){
  __HAL_RCC_TIM2_CLK_ENABLE();
  sTim2Handle.Instance = TIM2;
  /*ϵͳʱ��Ϊ24000000*/

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (APBClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  sTim2Handle.Init.Period            = (HAL_RCC_GetPCLKFreq()/10000 -1);//��TIME2��ʱ������Ϊ10KHZ(100us)
  sTim2Handle.Init.Prescaler         = 10;//(10*100us = 1MS)
  sTim2Handle.Init.ClockDivision     = 0;
  sTim2Handle.Init.CounterMode       = TIM_COUNTERMODE_UP; // TIM_COUNTERMODE_DOWN
  sTim2Handle.Init.RepetitionCounter = 0;
  sTim2Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&sTim2Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

	HAL_NVIC_EnableIRQ(TIM2_IRQn);

  if (HAL_TIM_Base_Start_IT(&sTim2Handle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();		
	
  /* Configure the system clock to HIRC 24MHz*/
  SystemClock_Config();
			
  /* Peripheral clock enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_I2C_CLK_ENABLE();
	__HAL_RCC_UART0_CLK_ENABLE();
    __HAL_RCC_UART1_CLK_ENABLE();
    __HAL_RCC_I2C_CLK_ENABLE();

	UartInit();
    init_I2C();
	sUartxHandle.Instance = UART0;
	sUartxHandle.Init.BaudRate = 9600;
	sUartxHandle.Init.BaudDouble = UART_BAUDDOUBLE_DISABLE;	// 
	sUartxHandle.Init.WordLength = UART_WORDLENGTH_8B;
	sUartxHandle.Init.Parity = UART_PARITY_NONE;
	sUartxHandle.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&sUartxHandle);
	
	  HAL_UART_Receive_IT(&sUartxHandle, (uint8_t *)&receData, 1);	
	  HAL_NVIC_EnableIRQ(UART0_IRQn); // 
    DFRobotTimerInit();
    DFRobot_Init_Reg_Attribute();//初始化寄存器
    init_Tcs3430();
    x1 = sHoldingReg.REG_CONFIG_X / 1000.0;
    y1 = sHoldingReg.REG_CONFIG_Y / 1000.0;
    ir1 = sHoldingReg.REG_CONFIG_IR1 / 1000.0;
    z1 = sHoldingReg.REG_CONFIG_Z / 1000.0;
    z2 = sHoldingReg.REG_CONFIG_Z2 / 1000.0;
    printf("X:%d\n",x1);
    printf("Y:%d\n",y1);
    printf("Z:%d\n",z1);
    printf("IR:%d\n",ir1);
    printf("Z2:%d\n",z2);
  while (1){
    if(DFRobot_RTU_Timer_Cnt > 5){// 5ms
        if(RecIndexLen > 0 ){
            DFRobot_RTU_Parse(ucReceiveData, RecIndexLen);
            RecIndexLen = 0;
        }else{
            DFRobot_RTU_Timer_Cnt = 0;
        }
    }

    if(sendCount == 500){
        if(mode == 1){
            DFRobot_Read_Reg(CMD_READ_INPUT, 0x06, 7);
        }
        sendCount = 0;
    }	
    
    if(readCount == 500){
         readCount = 0;
        get_Data();
     }
    readCount++;
    sendCount++;
    HAL_Delay(1);
	}
}


/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    ucReceiveData[RecIndexLen] = receData;
	DFRobot_RTU_Timer_Cnt = 0;
    RecIndexLen++;
	HAL_UART_Receive_IT(&sUartxHandle, (uint8_t *)&receData, 1);
}





/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};	
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HIRC;
  RCC_OscInitStruct.HIRCState = RCC_HIRC_ON;
  RCC_OscInitStruct.HIRCCalibrationValue = RCC_HIRCCALIBRATION_24M;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
	
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HIRC;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APBCLKDivider = RCC_PCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }	
}
 

/**
  * @brief  UART MSP Init.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};

    /**if UARTx is UART0 
		GPIO Configuration:    
    PA1     ------> UART0_RXD
    PA2     ------> UART0_TXD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF;
	GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;	
	GPIO_InitStruct.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	GPIO_InitStruct.SlewRate = GPIO_SLEW_RATE_HIGH;
	GPIO_InitStruct.DrvStrength = GPIO_DRV_STRENGTH_LOW;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = GPIO_AF5_UART0_TXD;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF;
	GPIO_InitStruct.Alternate = GPIO_AF5_UART0_RXD;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

     
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1);
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


/* Private function -------------------------------------------------------*/



