#include "bsp_key.h"

void KEY1_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = KEY1_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStruct);	
	
}

static unsigned char Acce_Offset_Flag = 0;
static unsigned char Mag_XY_Offset_Flag = 0;
static unsigned char Mag_Z_Offset_Flag = 0;
unsigned char key_count = 0;
unsigned char key_value = 0;
void Key1_Scan()
{
	key_count++;
	if(key_count > 7)
		key_count = 0;
	switch(key_count)
	{
		case 1: key_value = 1;
			printf("校准模式已进入\n");
			printf("接下来进行加速度计校准,校准过程中请勿移动罗盘,请按按键开始……\n");
			Acce_Offset_Flag = 1;	//加计校准标志位置1
			break;
		case 2: key_value = 2;
			break;
		case 3: key_value = 3;
			//Acce_Offset_Flag = 0;
			printf("加计校准完成,接下来进行磁力计X和Y轴校准,请水平旋转罗盘一周……\n"); 
			Mag_XY_Offset_Flag = 1;
			printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
			break; 
		case 4: key_value = 4;
			break;
		case 5: key_value = 5;
			Mag_XY_Offset_Flag = 0;
			printf("接下来进行磁力计Z轴校准,请转动罗盘90度,再次水平旋转罗盘一周……\n"); 
			Mag_Z_Offset_Flag = 1;
			printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
			break;
		case 6: key_value = 6;
			break;
		case 7: key_value = 7;
			Mag_Z_Offset_Flag = 0;
			printf("磁力计校准完成,请按按键退出校准模式\n");
			break;
		default: 
			break;
	}
}
static int time = 0;
void Key1_Process()
{
	//key_value++;			//键值自加
	//if(key_value > 7)
		//key_value = 0;
	switch(key_value)
	{
		case 0: break;
		case 1: //printf("校准模式已进入\n");
			//printf("接下来进行加速度计校准,校准过程中请勿移动罗盘,请按按键开始……\n");
			//Acce_Offset_Flag = 1; 	
			break;
		case 2: while(Acce_Offset_Flag)
			{
				Get_Acce_Offset(); 
				time++;
				if(time == 30)
				{
					Acce_Offset_Flag = 0;
					printf("加计校准完成,请按按键进行下一步校准……\n"); 
				}
			// if(Acce_Offset_Flag = 0)
			// {	
			// 	//printf("加计校准完成,请按按键进行下一步校准……\n"); 
			// 	key_value = 0;
			// }
				break;
			}
		case 3: //Acce_Offset_Flag = 0;
			//printf("接下来进行磁力计X和Y轴校准,请水平旋转罗盘一周……\n"); 
			//Mag_XY_Offset_Flag = 1;
			//printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
			break;
		case 4:	while(Mag_XY_Offset_Flag)
			{
				Get_Mag_Offset_XY();
				break;
			}
		case 5: //Mag_XY_Offset_Flag = 0;
			//printf("接下来进行磁力计Z轴校准,请转动罗盘90度,再次水平旋转罗盘一周……\n"); 
			//Mag_Z_Offset_Flag = 1;
			//printf("请按下按键开始旋转,旋转完成后请再次按下按键……\n");
			break;
		case 6: while(Mag_Z_Offset_Flag)
			{
				Get_Mag_Offset_Z();
				break;
			}
		case 7: //ag_Z_Offset_Flag = 0;
			//printf("磁力计校准完成,请按按键退出校准模式\n");
			break;
		default: 
			break;
	}
}

