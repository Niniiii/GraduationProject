
#include <stdio.h> 
#include <math.h>
#include "qst_sw_i2c.h"
#include "QMAX981.h"
#include "string.h"

uint8_t chip_id_x981 = 0x00;


void Delay_us_tmp(u32 nus)
{
	u32 i, j;
	for(; nus > 0; nus--)
		for(i = 0; i < 200; i++)
			j++;
}



#define QMA7981_MULTI_TAP
#define QMA7981_ANY_MOTION


static unsigned short acc_lsb_div = 0;
unsigned char reg_0x10 = 0;	
unsigned char reg_0x16 = 0;
unsigned char reg_0x18 = 0;
unsigned char reg_0x19 = 0;
unsigned char reg_0x1a = 0;
#if defined(QMA7981_ANY_MOTION)||defined(QMA7981_NO_MOTION)
	unsigned char reg_0x2c = 0;
#endif
#if defined(QMA7981_HAND_UP_DOWN)
	unsigned char reg_0x42 = 0;
#endif


unsigned char qmaX981_write_reg(unsigned char Addr, unsigned char Data)
{
#if defined(USE_SPI)
	return qst_qmaX981_spi_write(Addr, Data);
#else
	return qst_sw_writereg(QMAX981_I2C_SLAVE_ADDR<<1, Addr, Data);
#endif
}

unsigned char qmaX981_read_reg(unsigned char Addr, unsigned char* Buf, unsigned char Len)
{
	return qst_sw_readreg(QMAX981_I2C_SLAVE_ADDR<<1, Addr, Buf, Len);
}


void qmaX981_init(void)
{	
	qmaX981_read_reg(QMAX981_CHIP_ID ,&chip_id_x981, 1);
	printf("chipid:%d\n",chip_id_x981);	
	if((chip_id_x981 >= 0xe0) && (chip_id_x981 <= 0xe7))
	{	
		qmaX981_write_reg(0x36, 0xb6);
		Delay_us_tmp(30000);
		
		qmaX981_write_reg(0x36, 0x00);
		
		//qmaX981_set_range(ACC_RANGE_4G);	// 0.488 mg
		//qmaX981_write_reg(0x0f, 0x02);
		qmaX981_write_reg(0x0f, 0x02);//量程
		acc_lsb_div = 2048;
		
		//0xe0	[65hz			15.48ms]
		//0xe1	[129hz		7.74ms]
		//0xe2	[258hz		3.87ms]
		//0xe3	[512hz		1.935ms]
		//reg_0x10 = 0xe3;//512HZ
		reg_0x10 = 0xe2;//258HZ
		//qmaX981_write_reg(0x10, reg_0x10);
		qmaX981_write_reg(0x10, reg_0x10);//ODR
		
		//qmaX981_write_reg(0x4a, 0x08);	//Force I2C I2C interface
		//qmaX981_write_reg(0x11, 0x80);
		qmaX981_write_reg(0x11, 0x80);//模式寄存器
		
		//qmaX981_write_reg(0x5f, 0x80);
		qmaX981_write_reg(0x5f, 0x80);//设置模式寄存器后，重启内部FSM
		
		//qmaX981_write_reg(0x5f, 0x00);
		qmaX981_write_reg(0x5f, 0x00);
		/*
		// read reg
		//qmaX981_read_reg(0x16, &reg_0x16, 1);
		Single_ReadI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x16 ,&reg_0x16);
		
		//qmaX981_read_reg(0x18, &reg_0x18, 1);
		Single_ReadI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x18 ,&reg_0x18);
		
		//qmaX981_read_reg(0x19, &reg_0x19, 1);
		Single_ReadI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x19 ,&reg_0x19);
		
		//qmaX981_read_reg(0x1a, &reg_0x1a, 1);
		Single_ReadI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x1a ,&reg_0x1a);
		
		//qmaX981_write_reg(0x1f, 0x09);		// 0 step
		//qmaX981_write_reg(0x1f, 0x29);		// 4 step
		//qmaX981_write_reg(0x1f, 0x49);		// 8 step
		//qmaX981_write_reg(0x1f, 0x69);		// 12 step
		//qmaX981_write_reg(0x1f, 0x89);		// 16 step
		//qmaX981_write_reg(0x1f, 0xa9);		// 24 step
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x1f, 0xa9);
		
		//qmaX981_write_reg(0x1f, 0xc9);		// 32 step
		//qmaX981_write_reg(0x1f, 0xe9);		// 40 step
		
		//step int
		#if defined(QMA7981_STEP_INT)
		reg_0x16 |= 0x08;
		reg_0x19 |= 0x08;
		//qmaX981_write_reg(0x16, reg_0x16);
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x16, reg_0x16);
		
		//qmaX981_write_reg(0x19, reg_0x19);
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x19, reg_0x19);
		#endif
		
		#if defined(QMA7981_SIGNIFICANT_STEP)
		//qmaX981_write_reg(0x1d, 0x26);		//every 30 step
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x1d, 0x26);
		
		reg_0x16 |= 0x40;
		reg_0x19 |= 0x40;
		//qmaX981_write_reg(0x16, reg_0x16);
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x16, reg_0x16);
		
		//qmaX981_write_reg(0x19, reg_0x19);
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x19, reg_0x19);
		#endif
		
		#if defined(QMA7981_ANY_MOTION)
		reg_0x18 |= 0x07;	//0x07;
		reg_0x1a |= 0x01;
		reg_0x2c |= 0x00;	//0x01;		// 0x00
		
		//qmaX981_write_reg(0x18, reg_0x18);
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x18, reg_0x18);
		
		//qmaX981_write_reg(0x1a, reg_0x1a);
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x1a, reg_0x1a);
		
		//qmaX981_write_reg(0x2c, reg_0x2c);
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x2c, reg_0x2c);
		
		//qmaX981_write_reg(0x2e, 0xc0);		// 0.488*16*196= 1.5g
		Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x2e, 0xC0);
		//qmaX981_write_reg(0x2e, 0x80);		// 0.488*16*128 = 1g
		//Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x2e, 0x80);
		//qmaX981_write_reg(0x2e, 0x60);		// 0.488*16*96 = 750mg
		//Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x2e, 0x60);
		//qmaX981_write_reg(0x2e, 0x40);		// 0.488*16*64 = 500mg
		//qmaX981_write_reg(0x2e, 0x20);		// 0.488*16*32 = 250mg
		//qmaX981_write_reg(0x2e, 0x18);		// 0.488*16*20 = 156mg
		//Single_WriteI2C(QMAX981_I2C_SLAVE_ADDR<<1, 0x2e, 0x60);
#if defined(QMA7981_SIGNIFICANT_MOTION)
		//qmaX981_write_reg(0x2f, 0x0c|0x01);
		qmaX981_write_reg(0x2f, 0x01);
		reg_0x19 |= 0x01;
		qmaX981_write_reg(0x19, reg_0x19);
#endif
#endif

#if defined(QMA7981_NO_MOTION)
		reg_0x18 |= 0xe0;
		reg_0x1a |= 0x80;
		reg_0x2c |= 0x00;	//1s			//0x24;

		qmaX981_write_reg(0x18, reg_0x18);
		qmaX981_write_reg(0x1a, reg_0x1a);
		qmaX981_write_reg(0x2c, reg_0x2c);
		qmaX981_write_reg(0x2d, 0x14);
#endif

#if defined(QMA7981_HAND_UP_DOWN)
		reg_0x16 |= 0x02;
		reg_0x19 |= 0x02;
				
		qmaX981_write_reg(0x16, reg_0x16);
		qmaX981_write_reg(0x19, reg_0x19);
		// hand down
		reg_0x16 |= 0x04;
		reg_0x19 |= 0x04;
		qmaX981_write_reg(0x16, reg_0x16);
		qmaX981_write_reg(0x19, reg_0x19);
		// hand down	
		qmaX981_read_reg(0x42, &reg_0x42, 1);
	#if 1	// swap xy
		reg_0x42 |= 0x80;		// 0x42 bit 7 swap x and y
		qmaX981_write_reg(0x42, reg_0x42);
	#endif
#endif

#if defined(QMA7981_DATA_READY)
		reg_0x1a |= 0x10;
		qmaX981_write_reg(0x17, 0x10);
		qmaX981_write_reg(0x1a, reg_0x1a);
#endif

#if defined(QMA7981_INT_LATCH)
		qmaX981_write_reg(0x21, 0x1f);	// default 0x1c, step latch mode
#endif
		
		#if defined(QMA7981_TAP_CHECK)
		reg_0x10 = 0xe2;
		qmaX981_write_reg(0x10, reg_0x10);

		g_tap.tap_enable = 1;
		g_tap.tap_num = 0;
		g_tap.msec_tapquiet = 80;//150;
		g_tap.msec_timeout = 250;	
#endif
		
#if defined(QMAX981_AUTO_OFFSET)
		g_offset.enable = 1;
		g_offset.flag = 0;
		g_offset.x_last = 0;
		g_offset.y_last = 0;
		g_offset.x_thres = acc_lsb_div*15/100;
		g_offset.y_thres = acc_lsb_div*15/100;
		g_offset.static_thres = acc_lsb_div*8/100;
		
		printf("lsb=%d, x_thres=%d y_thres=%d static_thres=%d\n",acc_lsb_div, g_offset.x_thres, g_offset.y_thres, g_offset.static_thres);
		printf("short=%d int=%d long=%d\n",sizeof(short),sizeof(int),sizeof(long));
	#endif
	*/
	}
}




unsigned char reg_data[6];
int16_t raw[3];
float acc_raw_x981[3];

int16_t raw1[3];
unsigned char reg_data1[6];
uint16_t cali_count_x981 = 0;
float offset_acc_x981[3] = {0.0f, 0.0f, 0.0f};
#define MAX_CALI_COUNT_X981 100
float accel_calibration_sum_x981[3] = {0.0f, 0.0f, 0.0f};
#define GRAVITY_MSS 9.80665f
float accel_corrected[3] = {0.0f, 0.0f, 0.0f};
float pitch_sensor = 0, roll_sensor = 0;

void QMA7981_read_xyz_g(void)
{
	unsigned char ret = 0, axis = 0;
	ret = qmaX981_read_reg(0x01, reg_data1, 6);
	
	if(ret == 0)
	{
	  return;
	}
	raw1[0] = (int16_t)((int16_t)(reg_data1[1] << 8) | (int16_t)(reg_data1[0]));	
	raw1[1] = (int16_t)((int16_t)(reg_data1[3] << 8) | (int16_t)(reg_data1[2]));	
	raw1[2] = (int16_t)((int16_t)(reg_data1[5] << 8) | (int16_t)(reg_data1[4]));	
	
	raw1[0] = raw1[0] >> 2;
	raw1[1] = raw1[1] >> 2;
	raw1[2] = raw1[2] >> 2;
	
	acc_raw_x981[0] = (raw1[0] * 9.807) / acc_lsb_div;
	acc_raw_x981[1] = (raw1[1] * 9.807) / acc_lsb_div;
	acc_raw_x981[2] = (raw1[2] * 9.807) / acc_lsb_div;
	
	//printf("raw:%.3f %.3f %.3f\n",acc_raw_x981[0],acc_raw_x981[1],acc_raw_x981[2]);
}

//滑动窗口滤波
#define FILTER_N 12
Acceleration ACC_FILTER = {0, 0, 0};
float ACC_X_filter_buf[FILTER_N];
float ACC_Y_filter_buf[FILTER_N];
float ACC_Z_filter_buf[FILTER_N];
unsigned char ACC_Filter_i = 0;
void Acceleration_Filter(void)
{
	unsigned char count = 0;
	float ACC_X_filter_sum = 0;
	float ACC_Y_filter_sum = 0;
	float ACC_Z_filter_sum = 0;
	QMA7981_read_xyz_g();
	ACC_X_filter_buf[ACC_Filter_i] = acc_raw_x981[0];
	ACC_Y_filter_buf[ACC_Filter_i] = acc_raw_x981[1];
	ACC_Z_filter_buf[ACC_Filter_i] = acc_raw_x981[2];
	ACC_Filter_i++;
	if(ACC_Filter_i == FILTER_N)
		ACC_Filter_i = 0;
	for(count = 0;count < FILTER_N;count++)
	{
		ACC_X_filter_sum += ACC_X_filter_buf[count];
		ACC_Y_filter_sum += ACC_Y_filter_buf[count];
		ACC_Z_filter_sum += ACC_Z_filter_buf[count];
	}
	ACC_FILTER.x = ACC_X_filter_sum/FILTER_N;//滤完波后的X轴g
	ACC_FILTER.y = ACC_Y_filter_sum/FILTER_N;//滤完波后的Y轴g
	ACC_FILTER.z = ACC_Z_filter_sum/FILTER_N;//滤完波后的Z轴g
}
