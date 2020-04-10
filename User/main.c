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
extern unsigned char key_value;

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

	TIM_Cmd(TIM1, ENABLE);
	Read_Offset_to_Flash();
	
	{
		unsigned char i = 0;
		for(i=0; i<100; i++)
		{
			Acceleration_Filter();
			Magn_Filter();
		}
	}
			
  	while(1)
  	{
		if(key_value != 0)
		{
			Key1_Process();
		}
		else
		{
			if(cnt_50ms > 3)//
			{
		 		//QMA7981_read_xyz_g();
		 		//qmc6308_read_mag_xyz();
				Get_Angle();
				cnt_50ms = 0;
			}
		}
	}
}




