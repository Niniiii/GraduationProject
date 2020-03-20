#ifndef __ANGLE_CALCULATION_H
#define __ANGLE_CALCULATION_H

#include "math.h"
#include "calibration.h"

typedef struct{
	float pitch;
	float yaw;
	float roll;
}Attitude_Angle;//×ËÌ¬½Ç½á¹¹Ìå

void Angle_Calculate(void);
void Angle_Yaw_Calculate(void);
void Get_Angle(void);

#endif
