#ifndef _DFROBOT_TCS3430_H_
#define _DFROBOT_TCS3430_H_
#include "cs32l010_hal.h"
#include "DFRobot_RTU_Config.h"
#include "DFRobot_RTU_User.h"

#define DFRobot_TCS3430_ICC_ADDR            (0x39 << 1)// 从机设备地址
#define DFRobot_TCS3430_CONFIG_NO_WLONG             0x80
#define DFRobot_TCS3430_HGAIN_DISABLE                 0x04

typedef enum {
    eRegENABLEAddr = 0x80,
    eRegATIMEAddr,
    eRegWTIMEAddr = 0x83,
    eRegAILTLAddr,
    eRegAILTHAddr,
    eRegAIHTLAddr,
    eRegAIHTHAddr,
    eRegPERSAddr = 0x8C,
    eRegCFG0Addr,
    eRegCFG1Addr = 0x90,
    eRegREVIDAddr,
    eRegIDAddr,
    eRegSTATUSAddr,
    eRegCH0DATALAddr,
    eRegCH0DATAHAddr,
    eRegCH1DATALAddr,
    eRegCH1DATAHAddr,
    eRegCH2DATALAddr,
    eRegCH2DATAHAddr,
    eRegCH3DATALAddr,
    eRegCH3DATAHAddr,
    eRegCFG2Addr = 0x9F,
    eRegCFG3Addr = 0xAB,
    eRegAZCONFIGAddr = 0xD6,
    eRegINTENABAddr = 0xDD,
  } eTCS3430RegisterAddress_t;

/*
   * Enable Register (ENABLE 0x80)
   * ------------------------------------------------------------------------------------------
   * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
   * ------------------------------------------------------------------------------------------
   * |                  reversed                 |   WEN    | reserved |    AEN    |   PON    |
   * ------------------------------------------------------------------------------------------
   *
   *
*/
typedef struct {
  uint8_t   pon: 1; /* Power ON. This field activates the internal oscillator to permit the timers and ADC channels to operate. 
                       Writing a one activates the oscillator. 
                       Writing a zero disables the oscillator. */
  uint8_t   aen: 1; /* ALS Enable. This bit actives the ALS function.
                      Set aen=1 and pon=1 in the same command to ensure autozero function is run prior to the first measurement.*/
  uint8_t   reservedBit2: 1; /* Reserved. */
  uint8_t   wen: 1; /* Wait Enable. This bit activates the wait featrue. 
                       Writing a 1 activates the wait timer.
                       Writing a 0 disables the wait timer. */
  uint8_t   reservedBit4_7: 4; /* Reserved. */
}sEnableReg_t;

/* 
   * CFG1 Register (0x90) 
   * ---------------------------------------------------------------------------------------------
   * |    b7    |       b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
   * ---------------------------------------------------------------------------------------------
   * |                  reversed                    |   AMUX   | reversed |         AGAIN        |
   * ---------------------------------------------------------------------------------------------
   * Reserved: Write as 0.
   * AMUX:ALS Multiplexer. Sets the CH3 input. Default = 0 (X Channel). Set to 1 to read IR2.
   * AGAIN: ALS Gain Control. Sets the gain of the ALS DAC.
   * ----------------------------------------------------------
   * | Field Value |            ALS GAIN VALUE                |
   * ----------------------------------------------------------
   * |     00      |               1X Gain                    |
   * ----------------------------------------------------------
   * |     01      |               4X Gain                    |
   * ----------------------------------------------------------
   * |     10      |               16X Gain                   |
   * ----------------------------------------------------------
   * |     11      |               64X Gain                   |
   * ----------------------------------------------------------
*/
typedef struct { 
  uint8_t   again: 2; 
  uint8_t   reservedBit2: 1; 
  uint8_t   amux: 1;
  uint8_t   reservedBit4_7: 4; 
}sCFG1Reg_t;

/*
   * CFG3 Register (0xAB)
   * ----------------------------------------------------------------------------------------------------
   * |        b7          |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
   * ----------------------------------------------------------------------------------------------------
   * |   INT_READ_CLEAR   |     Reserved(0)     |    SAI   |           Reserved(any value)              |
   * ----------------------------------------------------------------------------------------------------
   * INT_READ_CLEAR:If this bit is set, all flag bits in the STATUS register will be reset whenever the STATUS register is read over I2C.
   * SAI:Sleep After Interrupt. Power down the device at the end of the ALS cycle if an interrupt has been generated.
*/
typedef struct { 
  uint8_t   reservedBit0_3: 4; 
  uint8_t   sai: 1; 
  uint8_t   reservedBit5_6: 2;
  uint8_t   intReadClear: 1; 
}sCFG3Reg_t;

/* 
   * AZ_CONFIG Register (0xD6)
   * ---------------------------------------------------------------------------------------------
   * |     b7    |       b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0   |
   * ---------------------------------------------------------------------------------------------
   * |  AZ_MODE  |                                 AZ_NTH_ITERATION                              |
   * ---------------------------------------------------------------------------------------------
   * AZ_MODE: 0, Always start at zero when searching the best offset value
              1, Always start at the previous (offset_c) with the auto-zero mechanism
   * AZ_NTH_ITERATION : Run autozero automatically every nth ALS iteration(0=never, 7Fh=only at first ALS cycle, n=every nth time)
*/
typedef struct { 
  uint8_t   azNTHIteration: 7;
  uint8_t   azMode: 1; 
} sAZCFGReg_t;


/*
   * INTENAB Register (0xDD)
   * ------------------------------------------------------------------------------------------
   * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
   * ------------------------------------------------------------------------------------------
   * |  ASIEN   |     Reserved(0)     |   AIEN   |                 Reserved(0)                |
   * ------------------------------------------------------------------------------------------
   * ASIEN: Writing '1' to this bit enables ASAT interrupt.
   * AIEN: Writing '1' to this bit enables ALS interrupt.
*/
typedef struct { 
  uint8_t   reservedBit0_3: 4; 
  uint8_t   aien: 1; 
  uint8_t   reservedBit5_6: 2;
  uint8_t   asien: 1; 
} sINTENABReg_t;




/**
 * @brief 初始化I2C
 * 
 */
void init_I2C(void);

/**
 * @brief 初始化TCS3430
 * 
 */
void init_Tcs3430(void);
/**
 * @brief 获取TCS3430的数据
 * 
 */
void get_Data(void);

/**
 * @brief 获取IR2数据
 * 
 * @return uint16_t 返回IR2数据
 */
uint16_t get_IR2(void);

/**
  * @brief I2C_Write_Data
	* @param  dev_address 设备地址
	*					reg_address 寄存器地址
	*					wdata 发送的数据
	*					size write size <=16
  * @retval ret result HAL_OK  or HAL_ERROR 
  */
HAL_StatusTypeDef I2C_Write_Data(uint8_t reg_address,uint8_t *wdata,uint8_t size);


/**
  * @brief I2C_Read_Data
	* @param  dev_address i2c device address
	*					reg_address register address
	*					rdata handle to read data
	*					size read size <=16
  * @retval ret result HAL_OK  or HAL_ERROR
  */
HAL_StatusTypeDef I2C_Read_Data(uint8_t reg_address,uint8_t *rdata,uint8_t size);
#endif
