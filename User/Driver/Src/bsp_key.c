#include "bsp_key.h"

void KEY1_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = KEY1_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStruct);	
	
}

unsigned char Acce_Offset_Flag = 0;
unsigned char Mag_XY_Offset_Flag = 0;
unsigned char Mag_Z_Offset_Flag = 0;
unsigned char key_vaule = 0;

void Key1_Scan()
{
	if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
	{
		key_vaule++;
		if(key_vaule > 7)
			key_vaule = 0;
		switch(key_vaule)
		{
			case 1: Acce_Offset_Flag = 1;
				break;
			case 3: Acce_Offset_Flag = 0;
				//printf("接下来进行磁力计X和Y轴校准,请水平旋转罗盘一周……\n"); 
				Mag_XY_Offset_Flag = 1;
				//printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
				break; 
			case 5: Mag_XY_Offset_Flag = 0;
				//printf("接下来进行磁力计Z轴校准,请转动罗盘90度,再次水平旋转罗盘一周……\n"); 
				Mag_Z_Offset_Flag = 1;
				//printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
				break;
			case 7: Mag_Z_Offset_Flag = 0;
				//printf("磁力计校准完成,请按按键退出校准模式\n");
			default: 
				break;
		}
	}
}

void Key1_Process()
{
	//key_vaule++;			//键值自加
	//if(key_vaule > 7)
		//key_vaule = 0;
	switch(key_vaule)
	{
		case 0: break;
		case 1: printf("校准模式已进入\n");
			printf("接下来进行加速度计校准,校准过程中请勿移动罗盘,请按按键开始……\n"); 
			break;
		case 2: while(Acce_Offset_Flag)
				Get_Acce_Offset(); 
			printf("加计校准完成,请按按键进行下一步校准"); 
			break;
		case 3: //Acce_Offset_Flag = 0;
			printf("接下来进行磁力计X和Y轴校准,请水平旋转罗盘一周……\n"); 
			//Mag_XY_Offset_Flag = 1;
			printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
			break;
		case 4:	while(Mag_XY_Offset_Flag)
				Get_Mag_Offset_XY();
			break;
		case 5: //Mag_XY_Offset_Flag = 0;
			printf("接下来进行磁力计Z轴校准,请转动罗盘90度,再次水平旋转罗盘一周……\n"); 
			//Mag_Z_Offset_Flag = 1;
			printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
			break;
		case 6: while(Mag_Z_Offset_Flag)
				Get_Mag_Offset_Z();
			break;
		case 7: //Mag_Z_Offset_Flag = 0;
			printf("磁力计校准完成,请按按键退出校准模式\n");
		default: 
			break;
	}
}

