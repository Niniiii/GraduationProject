#include "calibration.h"

extern void Acceleration_Filter(void);
extern void Magn_Filter(void); 
extern Acceleration ACC_FILTER;
extern Magnetic_field Magn_FILTER;

Acceleration ACCE_OFFSET = {0, 0, 0};//加速度零漂结构体
void Get_Acce_Offset(void)//得到加速度零漂
{
	int i = 0;
	float Acce_x_sum = 0;
	float Acce_y_sum = 0;
	float Acce_z_sum = 0;
	for(i = 0;i<100;i++)
	{
		Acceleration_Filter();
		Acce_x_sum += ACC_FILTER.x;
		Acce_y_sum += ACC_FILTER.y;
		Acce_z_sum += ACC_FILTER.z;
	}
	ACCE_OFFSET.x = Acce_x_sum/100;
	ACCE_OFFSET.y = Acce_y_sum/100;
	ACCE_OFFSET.z = Acce_y_sum/100;
}

Magnetic_field Mag_Offset = {0, 0, 0};
Magnetic_field Mag_Max = {0, 0, 0};
Magnetic_field Mag_Min = {0, 0, 0};
void Get_Mag_Offset_XY(void)
{
	Magn_Filter();
	if(Magn_FILTER.x > Mag_Max.x)
		Mag_Max.x = Magn_FILTER.x;
	if(Magn_FILTER.x < Mag_Min.x)
		Mag_Min.x = Magn_FILTER.x;
	if(Magn_FILTER.y > Mag_Max.y)
		Mag_Max.y = Magn_FILTER.y;
	if(Magn_FILTER.x < Mag_Min.x)
		Mag_Min.y = Magn_FILTER.y;
	Mag_Offset.x = (Mag_Max.x + Mag_Min.x)/2;
	Mag_Offset.y = (Mag_Max.y + Mag_Min.y)/2;
}
void Get_Mag_Offset_Z(void)
{
	Magn_Filter();
	if(Magn_FILTER.z > Mag_Max.z)
		Mag_Max.z = Magn_FILTER.x;
	if(Magn_FILTER.z < Mag_Min.z)
		Mag_Min.z = Magn_FILTER.z;
	Mag_Offset.z = (Mag_Max.z + Mag_Min.z)/2;
}
