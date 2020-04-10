#include "calibration.h"

typedef union
{
	uint16_t b[2];
	float a;
}Data;

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
		Acce_z_sum += ACC_FILTER.z - 2048; //2048为当前量程下的sensetivity
	}
	ACCE_OFFSET.x = Acce_x_sum/100;
	ACCE_OFFSET.y = Acce_y_sum/100;
	ACCE_OFFSET.z = Acce_z_sum/100;
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


void Write_Offset_to_Flash()
{
	Data Write_Acce_Offset_x;
	Data Write_Acce_Offset_y;
	Data Write_Acce_Offset_z;
	Write_Acce_Offset_x.a = ACCE_OFFSET.x;
	Write_Acce_Offset_y.a = ACCE_OFFSET.y;
	Write_Acce_Offset_z.a = ACCE_OFFSET.z;

	Data Write_Mag_Offset_x;
	Data Write_Mag_Offset_y;
	Data Write_Mag_Offset_z;
	Write_Mag_Offset_x.a = Mag_Offset.x;
	Write_Mag_Offset_y.a = Mag_Offset.y;
	Write_Mag_Offset_z.a = Mag_Offset.z;

	STMFLASH_Write(0x801C800, Write_Acce_Offset_x.b, 2);
	STMFLASH_Write(0x801D000, Write_Acce_Offset_y.b, 2);
	STMFLASH_Write(0x801D800, Write_Acce_Offset_z.b, 2);
	STMFLASH_Write(0x801E000, Write_Mag_Offset_x.b, 2);
	STMFLASH_Write(0x801E800, Write_Mag_Offset_y.b, 2);
	STMFLASH_Write(0x801F000, Write_Mag_Offset_z.b, 2);

}

void Read_Offset_to_Flash()
{
	Data Read_Acce_Offset_x;
	Data Read_Acce_Offset_y;
	Data Read_Acce_Offset_z;

	Data Read_Mag_Offset_x;
	Data Read_Mag_Offset_y;
	Data Read_Mag_Offset_z;

	STMFLASH_Read(0x801C800, Read_Acce_Offset_x.b, 2);
	STMFLASH_Read(0x801D000, Read_Acce_Offset_y.b, 2);
	STMFLASH_Read(0x801D800, Read_Acce_Offset_z.b, 2);
	STMFLASH_Read(0x801E000, Read_Mag_Offset_x.b, 2);
	STMFLASH_Read(0x801E800, Read_Mag_Offset_y.b, 2);
	STMFLASH_Read(0x801F000, Read_Mag_Offset_z.b, 2);

	
	ACCE_OFFSET.x = Read_Acce_Offset_x.a;
	ACCE_OFFSET.y = Read_Acce_Offset_y.a;
	ACCE_OFFSET.z = Read_Acce_Offset_z.a;
	
//	if(Read_Mag_Offset_x.a == 0xFF || Read_Mag_Offset_y.a == 0xFF || Read_Mag_Offset_z.a == 0xFF)
//	{
//		Mag_Offset.x = 0;
//		Mag_Offset.y = 0;
//		Mag_Offset.z = 0;
//	}	
//	else
//	{
		Mag_Offset.x = Read_Mag_Offset_x.a;
		Mag_Offset.y = Read_Mag_Offset_y.a;
		Mag_Offset.z = Read_Mag_Offset_z.a;

//	}
}
