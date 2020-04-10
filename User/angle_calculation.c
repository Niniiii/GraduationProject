#include "angle_calculation.h"
	
static float invSqrt(float x);
static float invSqrt(float x)
{
	float xHalf = 0.5f * x;
	int i = *(int *)&x;
	i = 0x5f3759df - (i >> 1);
	x = *(float *)&i;
	x = x * (1.5f - xHalf * x * x);
	return x;
}

extern Acceleration ACC_FILTER;		
extern Magnetic_field Magn_FILTER;
extern Acceleration ACCE_OFFSET;
extern Magnetic_field Mag_Offset;

Attitude_Angle Angle = {0, 0, 0}; //��ʼ��һ����̬�ǽṹ��

Acceleration ACCE_CORRECTED = {0, 0, 0};
float Sin_Pitch;
float Cos_Pitch;
float Sin_Roll;
float Cos_Roll;
void Angle_Calculate(void)//��Ǽ���
{
  	ACCE_CORRECTED.x = ACC_FILTER.x - ACCE_OFFSET.x;//ACCE_X_OFFSET����ֹ100�Σ�ȡƽ��ֵ
  	ACCE_CORRECTED.y = -(ACC_FILTER.y - ACCE_OFFSET.y);//ACCE_X��ACCE_Y��ACCE_Z���������ں���˲�ֵ
  	ACCE_CORRECTED.z = -(ACC_FILTER.z - ACCE_OFFSET.z);
  	//Angle.roll = -  57.3 * atan2f(ACCE_CORRECTED.x, sqrt(ACCE_CORRECTED.y * ACCE_CORRECTED.y + ACCE_CORRECTED.z * ACCE_CORRECTED.z));//����� ���Ȼ���Ƕ�
  	Angle.roll = -57.3 * atan2f(ACCE_CORRECTED.x, sqrtf(((powf(ACCE_CORRECTED.y, 2.0) + powf(ACCE_CORRECTED.z, 2.0)))));
	//Angle.pitch = 57.3 * atan2f(ACCE_CORRECTED.y, sqrt(ACCE_CORRECTED.x * ACCE_CORRECTED.x + ACCE_CORRECTED.z * ACCE_CORRECTED.z));//������
  	Angle.pitch = 57.3 * atan2f(ACCE_CORRECTED.y, sqrtf(((powf(ACCE_CORRECTED.x, 2.0) + powf(ACCE_CORRECTED.z, 2.0)))));
	Sin_Pitch = sinf(Angle.pitch);
  	Cos_Pitch = cosf(Angle.pitch);
  	Sin_Roll = sinf(Angle.roll);
  	Cos_Roll = cosf(Angle.roll); 
}

Magnetic_field Mag_Corrected = {0, 0, 0};
float Magn_x = 0.f;
float Magn_y = 0.f;
float yaw = 0.f;
void Angle_Yaw_Calculate(void)//ƫ���Ǽ���
{
  	Mag_Corrected.x = Magn_FILTER.x - Mag_Offset.x;
  	Mag_Corrected.y = Magn_FILTER.y - Mag_Offset.y;//Mag_Offset�ش�У׼��ֵ
  	Mag_Corrected.z = -(Magn_FILTER.z - Mag_Offset.z);
	
	yaw = atan2f(Mag_Corrected.y, Mag_Corrected.x) * 57.296;

  	if(yaw < 0)
  	{
		yaw += 360;
  	}

	//printf("Hx=%f, Hy=%f, Hz=%f\n",mag_float[0],mag_float[1],mag_float[2]);
 
  	/************��������ǲ���*****************/
  	Magn_y = Mag_Corrected.y * Cos_Roll + Mag_Corrected.z * Sin_Roll;
  	Magn_x = Mag_Corrected.y * Sin_Pitch * Sin_Roll + Mag_Corrected.x * Cos_Pitch - Mag_Corrected.z * Cos_Roll * Sin_Pitch;
  	/***********�����еõ������ƹ۲�Ƕ�*********/
//	Angle.yaw = atan2f(Mag_Corrected.y, Mag_Corrected.x) * 57.296;

//  	if(Angle.yaw < 0)
//  	{
//     		Angle.yaw += 360;
//  	}
	
  	Angle.yaw = atan2f(Magn_y, Magn_x) * 57.296;

  	if(Angle.yaw < 0)
  	{
     		Angle.yaw += 360;
  	}
}

void Get_Angle(void)
{
	Acceleration_Filter();
	Magn_Filter();
	Angle_Calculate();
	Angle_Yaw_Calculate();
  	//printf("roll:%.2f pitch:%.2f yaw:%.2f\n",Angle.roll,Angle.pitch,Angle.yaw);
}
