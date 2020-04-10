#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include "QMAX981.h"
#include "qmc6308.h"
#include "stmflash.h"

void Get_Acce_Offset(void);
void Get_Mag_Offset_XY(void);
void Get_Mag_Offset_Z(void);
void Write_Offset_to_Flash(void);
void Read_Offset_to_Flash(void);
#endif
