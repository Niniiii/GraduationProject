#include "angle_calculation.h"
#include <stdio.h>
	
float invSqrt(float x);
float invSqrt(float x)
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

Attitude_Angle Angle = {0, 0, 0}; //初始化一个姿态角结构体

Acceleration ACCE_CORRECTED = {0, 0, 0};
float Sin_Pitch;
float Cos_Pitch;
float Sin_Roll;
float Cos_Roll;
void Angle_Calculate(void)//倾角计算
{
  	ACCE_CORRECTED.x = ACC_FILTER.x - ACCE_OFFSET.x;//ACCE_X_OFFSET：静止100次，取平均值
  	ACCE_CORRECTED.y = ACC_FILTER.y - ACCE_OFFSET.y;//ACCE_X，ACCE_Y，ACCE_Z：滑动窗口后的滤波值
  	ACCE_CORRECTED.z = ACC_FILTER.z - ACCE_OFFSET.z;
  	Angle.roll = -57.3 * atanf(ACCE_CORRECTED.x * invSqrt(ACCE_CORRECTED.y * ACCE_CORRECTED.y + ACCE_CORRECTED.z * ACCE_CORRECTED.z));//横滚角 弧度换算角度
  	Angle.pitch = 57.3 * atanf(ACCE_CORRECTED.y * invSqrt(ACCE_CORRECTED.x * ACCE_CORRECTED.x + ACCE_CORRECTED.z * ACCE_CORRECTED.z));//俯仰角
  	Sin_Pitch = sinf(Angle.pitch);
  	Cos_Pitch = cosf(Angle.pitch);
  	Sin_Roll = sinf(Angle.roll);
  	Cos_Roll = cosf(Angle.roll); 
}

Magnetic_field Mag_Corrected = {0, 0, 0};
float Magn_x = 0;
float Magn_y = 0;
void Angle_Yaw_Calculate(void)//偏航角计算
{
  	Mag_Corrected.x = Magn_FILTER.x - Mag_Offset.x;//Mag_Offset地磁校准的值
  	Mag_Corrected.y = Magn_FILTER.y - Mag_Offset.y;
  	Mag_Corrected.z = Magn_FILTER.z - Mag_Offset.z;
 
  	/************磁力计倾角补偿*****************/
  	Magn_x = Mag_Corrected.x * Cos_Roll + Mag_Corrected.z * Sin_Roll;
  	Magn_y = Mag_Corrected.x * Sin_Pitch * Sin_Roll + Mag_Corrected.y * Cos_Pitch - Mag_Corrected.z * Cos_Roll * Sin_Pitch;
  	/***********反正切得到磁力计观测角度*********/
  	Angle.yaw = atan2f(Magn_x, Magn_y) * 57.296;

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
  	printf("roll:%.2f pitch:%.2f yaw:%.2f\n",Angle.roll,Angle.pitch,Angle.yaw);
}
