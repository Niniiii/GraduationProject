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
			printf("У׼ģʽ�ѽ���\n");
			printf("���������м��ٶȼ�У׼,У׼�����������ƶ�����,�밴������ʼ����\n");
			Acce_Offset_Flag = 1;	//�Ӽ�У׼��־λ��1
			break;
		case 2: key_value = 2;
			break;
		case 3: key_value = 3;
			//Acce_Offset_Flag = 0;
			printf("�Ӽ�У׼���,���������д�����X��Y��У׼,��ˮƽ��ת����һ�ܡ���\n"); 
			Mag_XY_Offset_Flag = 1;
			printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
			break; 
		case 4: key_value = 4;
			break;
		case 5: key_value = 5;
			Mag_XY_Offset_Flag = 0;
			printf("���������д�����Z��У׼,��ת������90��,�ٴ�ˮƽ��ת����һ�ܡ���\n"); 
			Mag_Z_Offset_Flag = 1;
			printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
			break;
		case 6: key_value = 6;
			break;
		case 7: key_value = 7;
			Mag_Z_Offset_Flag = 0;
			printf("������У׼���,�밴�����˳�У׼ģʽ\n");
			break;
		default: 
			break;
	}
}
static int time = 0;
void Key1_Process()
{
	//key_value++;			//��ֵ�Լ�
	//if(key_value > 7)
		//key_value = 0;
	switch(key_value)
	{
		case 0: break;
		case 1: //printf("У׼ģʽ�ѽ���\n");
			//printf("���������м��ٶȼ�У׼,У׼�����������ƶ�����,�밴������ʼ����\n");
			//Acce_Offset_Flag = 1; 	
			break;
		case 2: while(Acce_Offset_Flag)
			{
				Get_Acce_Offset(); 
				time++;
				if(time == 30)
				{
					Acce_Offset_Flag = 0;
					printf("�Ӽ�У׼���,�밴����������һ��У׼����\n"); 
				}
			// if(Acce_Offset_Flag = 0)
			// {	
			// 	//printf("�Ӽ�У׼���,�밴����������һ��У׼����\n"); 
			// 	key_value = 0;
			// }
				break;
			}
		case 3: //Acce_Offset_Flag = 0;
			//printf("���������д�����X��Y��У׼,��ˮƽ��ת����һ�ܡ���\n"); 
			//Mag_XY_Offset_Flag = 1;
			//printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
			break;
		case 4:	while(Mag_XY_Offset_Flag)
			{
				Get_Mag_Offset_XY();
				break;
			}
		case 5: //Mag_XY_Offset_Flag = 0;
			//printf("���������д�����Z��У׼,��ת������90��,�ٴ�ˮƽ��ת����һ�ܡ���\n"); 
			//Mag_Z_Offset_Flag = 1;
			//printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
			break;
		case 6: while(Mag_Z_Offset_Flag)
			{
				Get_Mag_Offset_Z();
				break;
			}
		case 7: //ag_Z_Offset_Flag = 0;
			//printf("������У׼���,�밴�����˳�У׼ģʽ\n");
			break;
		default: 
			break;
	}
}

