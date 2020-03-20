#ifndef _QMAX981_H_
#define _QMAX981_H_

#include "stm32f10x.h"


#define QMAX981_DELAY				0xff
/*Register Map*/
#define QMAX981_CHIP_ID		    	0x00
#define QMAX981_XOUTL				0x01
#define QMAX981_XOUTH				0x02
#define QMAX981_YOUTL				0x03
#define QMAX981_YOUTH				0x04
#define QMAX981_ZOUTL				0x05
#define QMAX981_ZOUTH				0x06
#define QMAX981_STEP_CNT_L			0x07
#define QMAX981_INT_STAT0			0x0a
#define QMAX981_INT_STAT1			0x0b
#define QMAX981_INT_STAT2			0x0c
#define QMAX981_INT_STAT3			0x0d
#define QMAX981_FIFO_STATE			0x0e
#define QMA7981_STEP_CNT_M			0x0e
#define QMAX981_REG_RANGE			0x0f
#define QMAX981_REG_BW_ODR			0x10
#define QMAX981_REG_POWER_CTL		0x11
#define QMAX981_STEP_SAMPLE_CNT		0x12
#define QMAX981_STEP_PRECISION		0x13
#define QMAX981_STEP_TIME_LOW		0x14
#define QMAX981_STEP_TIME_UP		0x15
#define QMAX981_INTPIN_CFG			0x20
#define QMAX981_INT_CFG				0x21
#define QMAX981_OS_CUST_X		    0x27
#define QMAX981_OS_CUST_Y			0x28
#define QMAX981_OS_CUST_Z			0x29
#define QMAX981_STEP_TIME_UP		0x15
/*ODR SET @lower ODR*/
#define QMA6981_ODR_1000HZ			0x07
#define QMA6981_ODR_500HZ			0x06
#define QMA6981_ODR_250HZ			0x05
#define QMA6981_ODR_125HZ			0x04  
#define QMA6981_ODR_62HZ			0x03   
#define QMA6981_ODR_31HZ			0x02   
#define QMA6981_ODR_16HZ			0x01
#define QMA6981_ODR_HIGH			0x20

/* Accelerometer Sensor Full Scale */
#define QMAX981_RANGE_2G			0x01
#define QMAX981_RANGE_4G			0x02
#define QMAX981_RANGE_8G			0x04
#define QMAX981_RANGE_16G			0x08
#define QMAX981_RANGE_32G			0x0f

/* 0x11 Set the sleep time, when device is in power cycling power saving.*/
#define QMA6981_SLEEP_DUR0			0x00
#define QMA6981_SLEEP_DUR1			0x06
#define QMA6981_SLEEP_DUR2			0x07
#define QMA6981_SLEEP_DUR4			0x08
#define QMA6981_SLEEP_DUR6			0x09
#define QMA6981_SLEEP_DUR10			0x0a
#define QMA6981_SLEEP_DUR25			0x0b
#define QMA6981_SLEEP_DUR50			0x0c
#define QMA6981_SLEEP_DUR100		0x0d
#define QMA6981_SLEEP_DUR500		0x0e
#define QMA6981_SLEEP_DUR1000		0x0f


#define QMAX981_I2C_SLAVE_ADDR		0x12	// AD0 GND 0x12, AD0 VDD 0x13
#define QMAX981_I2C_SLAVE_ADDR2		0x13	// AD0 GND 0x12, AD0 VDD 0x13


void Acceleration_Filter(void);
void QMA7981_read_xyz_g(void);
void QMA7981_read_xyz(int icount);
char QMA6981_Click_Alog(void);
void QMA7981_int_io_detect(void);
char QMA6981_click_status(void);
void qmaX981_irq_hdlr(void);


uint8_t qmaX981_c11hip_id(void);
void qmaX981_init(void);
void Delay_us_tmp(u32 nus);

typedef struct
{
	float accelRaw[3];	// 加速度计原始数据
	float gyroRaw[3];     // 陀螺仪原始数据
	float accelFilter[3];	// 加速度计滤波后数据
	float gyroFilter[3];	// 陀螺仪滤波后数据
}_IMU_Tag;

typedef struct {
	float x;
	float y;
	float z;
}Acceleration;

#endif
