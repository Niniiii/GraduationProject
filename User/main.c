#include "angle_calculation.h"
#include "bsp_key.h"
#include "QMAX981.h"
#include "QMC6308.h"
#include "qst_sw_i2c.h"
#include "SYSTEM.h"
#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include <stdio.h>

extern uint16_t cnt_50ms;

int main()
{
	//stm32 harewave Init
	HardWave_Init();
	
	//sensor Init
	qmaX981_init();
	qmaX981_init();
	qmaX981_init();
	qmaX981_init();
	qmaX981_init();
	qmaX981_init();
	qmaX981_init();
	qmc6308_init();
	Get_Acce_Offset();
	
  	while(1)
  	{
		Key1_Process();
		if(cnt_50ms > 9)//
		{
		 	//QMA7981_read_xyz_g();
		 	//qmc6308_read_mag_xyz();
			Get_Angle();
			cnt_50ms = 0;
		}
	}
}




