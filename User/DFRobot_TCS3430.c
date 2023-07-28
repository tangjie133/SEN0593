#include "DFRobot_TCS3430.h"


I2C_HandleTypeDef i2c_test = {0};

eTCS3430RegisterAddress_t _TCS3430Register;
sEnableReg_t _enableReg;
sCFG1Reg_t _cfg1Reg;
sCFG3Reg_t _cfg3Reg;
sAZCFGReg_t _AZCfgReg;
sINTENABReg_t _intEnabReg;

extern struct InputRegList sInputReg;

uint8_t aTime = 0x48, wTime = 0; //0X24 积分时间

float data[3][4]={{-0.00262,0.643836,-0.20156,0.004045},
                  {0.022363,0.535012,0.040962,-0.00789},
                  {0.137322,-0.13597,1.281127,-0.052}};


void init_I2C(void){
    GPIO_InitTypeDef  gpioi2c={0};
    gpioi2c.Pin    = GPIO_PIN_6;
	gpioi2c.Mode = GPIO_MODE_AF; // GPIO端口复用功能 
	gpioi2c.Alternate = GPIO_AF4_I2C_SDA;
	gpioi2c.OpenDrain = GPIO_OPENDRAIN; // 开漏输出
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE; // 禁止输入去抖动
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH; // 电压转换速率
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH; // 驱动强度
	gpioi2c.Pull = GPIO_NOPULL;			// 上拉
	HAL_GPIO_Init(GPIOD, &gpioi2c);
	
	
	gpioi2c.Pin = GPIO_PIN_5;
	gpioi2c.Mode = GPIO_MODE_AF;
	gpioi2c.Alternate = GPIO_AF4_I2C_SCL;
	gpioi2c.OpenDrain = GPIO_OPENDRAIN; 
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH;
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	gpioi2c.Pull = GPIO_NOPULL;	
	HAL_GPIO_Init(GPIOD, &gpioi2c);
	/*set init handle*/

	i2c_test.Instance = I2C;
	i2c_test.Init.master = I2C_MASTER_MODE_ENABLE; // 主机模式使能
	i2c_test.Init.slave = I2C_SLAVE_MODE_DISABLE; // 从机模式禁止
	i2c_test.Mode = HAL_I2C_MODE_MASTER; // 主机模式
	printf("Enter Test I2C Master\r\n");
	
	
	i2c_test.Init.broadack = I2C_BROAD_ACK_DISABLE; // 广播地址应答禁止
	i2c_test.Init.speedclock = I2C_SPEED_RATE; // I2C传输速率  
	i2c_test.State = HAL_I2C_STATE_RESET; //
	
	HAL_I2C_Init(&i2c_test);

}

void init_Tcs3430(void){
    uint8_t wlong = DFRobot_TCS3430_CONFIG_NO_WLONG, hgain = DFRobot_TCS3430_HGAIN_DISABLE;
    uint8_t buf[2];
    _enableReg.wen = 0;
    _cfg1Reg.again=1;//增益 4
    _cfg3Reg.intReadClear = 0;
    _cfg3Reg.sai = 0;
    _AZCfgReg.azMode = 0;
    _AZCfgReg.azNTHIteration = 0x7F;
    _intEnabReg.aien = 0;
    _intEnabReg.asien = 0;
    _cfg3Reg.intReadClear = 1;
    _enableReg.pon = 1;
    _enableReg.aen = 1;
    I2C_Write_Data(eRegENABLEAddr,((uint8_t*)(&_enableReg)),1);
    I2C_Write_Data(eRegATIMEAddr,&aTime,1);//积分时间
    I2C_Write_Data(eRegWTIMEAddr,&wTime,1);
    I2C_Write_Data(eRegCFG0Addr,&wlong,1);//_wlong =0;
    I2C_Write_Data(eRegCFG1Addr,((uint8_t*)(&_cfg1Reg)),1);
    I2C_Write_Data(eRegCFG2Addr,&hgain,1);
    I2C_Write_Data(eRegCFG3Addr,((uint8_t*)(&_cfg3Reg)),1);
    I2C_Write_Data(eRegCFG3Addr,((uint8_t*)(&_cfg3Reg)),1);
    I2C_Write_Data(eRegAZCONFIGAddr,((uint8_t*)(&_AZCfgReg)),1);
    I2C_Write_Data(eRegAZCONFIGAddr,((uint8_t*)(&_AZCfgReg)),1);
    I2C_Write_Data(eRegINTENABAddr,((uint8_t*)(&_intEnabReg)),1);
    I2C_Write_Data(eRegCFG3Addr,((uint8_t*)(&_cfg3Reg)),1);
    I2C_Write_Data(eRegINTENABAddr,((uint8_t*)(&_intEnabReg)),1);
    
    I2C_Write_Data(eRegENABLEAddr,((uint8_t*)(&_enableReg)),1);
    I2C_Read_Data(eRegREVIDAddr,buf,2);
    printf("RevisionID:%x ,DeviceID:%x \n",buf[0],buf[1]);
    if((buf[0] != 0x41) || buf[1] != 0xDC){
        printf("TCS3430 init error!!! \n");
        _enableReg.aen = 0;
        I2C_Write_Data(eRegENABLEAddr,((uint8_t*)(&_enableReg)),1);
        _enableReg.pon = 0;
        I2C_Write_Data(eRegENABLEAddr,((uint8_t*)(&_enableReg)),1);
    }
    printf("TCS3430 init ok!!!!! \n");
}

void get_Data(void){
    uint16_t X = 0,Y = 0, Z = 0, IR1 = 0, IR2 = 0,lux = 0,CCT = 0,change = 0;
    uint8_t buf[8];
    float hight_X=0,hight_Y=0,hight_Z=0,low_X=0,low_Y=0,low_Z=0,temp_x=0,temp_y=0,temp=0,x1 = 0.882,y1 = 0.817,ir1 = 0.75,z1 = 0.73,z2 = 0.95;//0.95
    I2C_Read_Data(eRegCH0DATALAddr,buf,8);
    Z = buf[0] | buf[1] << 8;
    Y =  buf[2] | buf[3] << 8;
    IR1 =  buf[4] | buf[5] << 8;
    X =  buf[6] | buf[7] << 8;
    printf("X: %d,Y: %d, Z: %d, IR1: %d\n",X,Y,Z,IR1);
    lux = (data[1][0]* X + data[1][1]* Y + data[1][2]* Z + data[1][3]* IR1 ) * 8;
    change = (IR1 * 1000)/Y;
    if(change >253){//高红外
        Z = Z * z1;
    }else{
        Z = Z * z2;
    }
    
    X = X *x1;
    Y = Y * y1; 
    IR1 = IR1 *ir1;
    printf("new X: %d,Y: %d, Z: %d, IR1: %d\n",X,Y,Z,IR1);

    IR2 = get_IR2();
    
    hight_X = data[0][0]* X + data[0][1]* Y + data[0][2]* Z + data[0][3]* IR1;
    hight_Y = data[1][0]* X + data[1][1]* Y + data[1][2]* Z + data[1][3]* IR1;
    hight_Z = data[2][0]* X + data[2][1]* Y + data[2][2]* Z + data[2][3]* IR1;
    temp_x = hight_X/(hight_X+hight_Y+hight_Z);
    temp_y = hight_Y/(hight_X+hight_Y+hight_Z);
    temp = (temp_x-0.332)/(0.1858-temp_y);
    CCT = (uint16_t)(437.0*temp* temp*temp +3601.0*temp * temp+6861.0*temp+5517.0);
    
    if((CCT > 3500 && IR1 > 1000) || (CCT > 3500 && ((lux / IR1) < 0.5))){
        CCT = CCT * 0.9676 + 1083.6;
    }else if((CCT > 4000 && IR1 < 180) || (CCT > 4000 && IR1 < 300 && ((lux / IR1) > 2.5))){
      CCT = 1.0611 * CCT + 433.07;
      }else{
      //cct = cct1;
      }
    sInputReg.REG_IR1 = IR1;
    sInputReg.REG_IR2 = IR2;
    sInputReg.REG_LUX = lux;
    sInputReg.REG_CCT = CCT;
    sInputReg.REG_x = temp_x * 10000;
    sInputReg.RGB_y = temp_y * 10000;

  
    #if 1
        printf("lux: %d,CCT: %d",lux,CCT);
    #endif

}
uint16_t get_IR2(void)
{
    uint16_t IR2 = 0;
    uint16_t delayTime = 0;
    uint8_t buf[2];
    _enableReg.aen = 0;
    I2C_Write_Data(eRegENABLEAddr, ((uint8_t*)(&_enableReg)),1);
    _cfg1Reg.amux=1;
    I2C_Write_Data(eRegCFG1Addr,((uint8_t*)(&_cfg1Reg)),1);
    _enableReg.aen = 1;
    I2C_Write_Data(eRegENABLEAddr,((uint8_t*)(&_enableReg)),1);
    delayTime = (aTime+1)*2.78 + (wTime+1)*2.78; 
    HAL_Delay(delayTime);
    I2C_Read_Data(eRegCH3DATALAddr,buf,2);
    IR2 = buf[0] | buf[1] << 8;
    _enableReg.aen = 0;
    I2C_Write_Data(eRegENABLEAddr, ((uint8_t*)(&_enableReg)),1);
    _cfg1Reg.amux=0;
    I2C_Write_Data(eRegCFG1Addr,((uint8_t*)(&_cfg1Reg)),1);
    _enableReg.aen = 1;
    I2C_Write_Data(eRegENABLEAddr, ((uint8_t*)(&_enableReg)),1);
    HAL_Delay(delayTime);
    return IR2;
}

HAL_StatusTypeDef I2C_Write_Data(uint8_t reg_address,uint8_t *wdata,uint8_t size)
{
	uint8_t data[17] = {0};
	uint8_t i = 0;
	HAL_StatusTypeDef  ret = HAL_ERROR;
	
	data[0] = reg_address;  
	for(i=0;i<size;i++)
	{
		data[i+1]= wdata[i];
	}
	ret = HAL_I2C_Master_Transmit(&i2c_test, DFRobot_TCS3430_ICC_ADDR, data, size+1);

	return ret;
}

HAL_StatusTypeDef I2C_Read_Data(uint8_t reg_address,uint8_t *rdata,uint8_t size)
{
	HAL_StatusTypeDef  ret = HAL_ERROR;
	
	ret = HAL_I2C_Master_Transmit(&i2c_test, DFRobot_TCS3430_ICC_ADDR, &reg_address, 1);
	ret = HAL_I2C_Master_Receive(&i2c_test, DFRobot_TCS3430_ICC_ADDR, rdata, size);
	return ret;
}
