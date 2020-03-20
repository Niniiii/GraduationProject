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
				//printf("���������д�����X��Y��У׼,��ˮƽ��ת����һ�ܡ���\n"); 
				Mag_XY_Offset_Flag = 1;
				//printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
				break; 
			case 5: Mag_XY_Offset_Flag = 0;
				//printf("���������д�����Z��У׼,��ת������90��,�ٴ�ˮƽ��ת����һ�ܡ���\n"); 
				Mag_Z_Offset_Flag = 1;
				//printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
				break;
			case 7: Mag_Z_Offset_Flag = 0;
				//printf("������У׼���,�밴�����˳�У׼ģʽ\n");
			default: 
				break;
		}
	}
}

void Key1_Process()
{
	//key_vaule++;			//��ֵ�Լ�
	//if(key_vaule > 7)
		//key_vaule = 0;
	switch(key_vaule)
	{
		case 0: break;
		case 1: printf("У׼ģʽ�ѽ���\n");
			printf("���������м��ٶȼ�У׼,У׼�����������ƶ�����,�밴������ʼ����\n"); 
			break;
		case 2: while(Acce_Offset_Flag)
				Get_Acce_Offset(); 
			printf("�Ӽ�У׼���,�밴����������һ��У׼"); 
			break;
		case 3: //Acce_Offset_Flag = 0;
			printf("���������д�����X��Y��У׼,��ˮƽ��ת����һ�ܡ���\n"); 
			//Mag_XY_Offset_Flag = 1;
			printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
			break;
		case 4:	while(Mag_XY_Offset_Flag)
				Get_Mag_Offset_XY();
			break;
		case 5: //Mag_XY_Offset_Flag = 0;
			printf("���������д�����Z��У׼,��ת������90��,�ٴ�ˮƽ��ת����һ�ܡ���\n"); 
			//Mag_Z_Offset_Flag = 1;
			printf("�밴�°�����ʼ��ת,��ת��ɺ����ٴΰ��°�������\n");
			break;
		case 6: while(Mag_Z_Offset_Flag)
				Get_Mag_Offset_Z();
			break;
		case 7: //Mag_Z_Offset_Flag = 0;
			printf("������У׼���,�밴�����˳�У׼ģʽ\n");
		default: 
			break;
	}
}

