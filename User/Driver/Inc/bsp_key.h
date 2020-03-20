#ifndef __BSP_KEY_H
#define __BSP_KEY_H
 
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "calibration.h"
#include "stdio.h"

#define  KEY_ON      0  //低电平按下按键
#define  KEY_OFF     1
 
#define KEY1_GPIO_PIN              GPIO_Pin_13
#define KEY1_GPIO_PORT             GPIOC
 
void KEY1_GPIO_Config(void);
void Key1_Scan(void);
void Key1_Process(void);
 
#endif /* __BSP_KEY_H */
