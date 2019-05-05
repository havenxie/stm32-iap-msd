#ifndef __TSENSOR_H
#define __TSENSOR_H	
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//ADC 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/7 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

#define ADC_CH1  		ADC_Channel_1  //通道1		 
#define ADC_CH_TEMP  	ADC_Channel_16 //温度传感器通道
   
u16  T_Get_Temp(void);  //取得温度值
void T_Adc_Init(void); //ADC通道初始化
u16  T_Get_Adc(u8 ch); //获得某个通道值  
u16  T_Get_Adc_Average(u8 ch,u8 times);//得到某个通道10次采样的平均值 	  
#endif 
