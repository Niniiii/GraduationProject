/**
  ******************************************************************************
  * @file    qmcX983.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief    qmcX983驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 指南者 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "qmc6308.h"
#include "qst_sw_i2c.h"
#include "stdio.h"
#include "math.h"

static unsigned char sensor_mask[4] = {
	0x80,
	0xA0,
	0xB0,
	0xC0
};

static qmc6308_map g_map;

static int qmc6308_read_block(u8 addr, u8 *data, u8 len)
{
#if defined(QST_USE_SW_I2C)
	return qst_sw_readreg(QMC6308_IIC_ADDR, addr, data, len);
#else
	I2C_Bus_set_slave_addr(QMC6308_IIC_ADDR);
	return I2C_BufferRead(data,addr,len);
#endif
}

/*
static int qmc6308_write_block(u8 addr, u8 *data, u8 len)
{	
	int ret = 0;

	mutex_lock(&g_qmc6308->op_lock);
	ret = i2c_smbus_write_i2c_block_data(g_qmc6308->client, addr, len, data);
	mutex_unlock(&g_qmc6308->op_lock);
	if(ret < 0)
		return ret;
	return 0;
}
*/

static int qmc6308_write_reg(u8 addr, u8 data)
{	
#if defined(QST_USE_SW_I2C)
	return qst_sw_writereg(QMC6308_IIC_ADDR, addr, data);
#else
	I2C_Bus_set_slave_addr(QMC6308_IIC_ADDR);
	return I2C_ByteWrite(addr,data); 
#endif

}

static void qmc6308_set_layout(int layout)
{
	if(layout == 0)
	{
		g_map.sign[AXIS_X] = 1;
		g_map.sign[AXIS_Y] = 1;
		g_map.sign[AXIS_Z] = 1;
		g_map.map[AXIS_X] = AXIS_X;
		g_map.map[AXIS_Y] = AXIS_Y;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 1)
	{
		g_map.sign[AXIS_X] = -1;
		g_map.sign[AXIS_Y] = 1;
		g_map.sign[AXIS_Z] = 1;
		g_map.map[AXIS_X] = AXIS_Y;
		g_map.map[AXIS_Y] = AXIS_X;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 2)
	{
		g_map.sign[AXIS_X] = -1;
		g_map.sign[AXIS_Y] = -1;
		g_map.sign[AXIS_Z] = 1;
		g_map.map[AXIS_X] = AXIS_X;
		g_map.map[AXIS_Y] = AXIS_Y;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 3)
	{
		g_map.sign[AXIS_X] = 1;
		g_map.sign[AXIS_Y] = -1;
		g_map.sign[AXIS_Z] = 1;
		g_map.map[AXIS_X] = AXIS_Y;
		g_map.map[AXIS_Y] = AXIS_X;
		g_map.map[AXIS_Z] = AXIS_Z;
	}	
	else if(layout == 4)
	{
		g_map.sign[AXIS_X] = -1;
		g_map.sign[AXIS_Y] = 1;
		g_map.sign[AXIS_Z] = -1;
		g_map.map[AXIS_X] = AXIS_X;
		g_map.map[AXIS_Y] = AXIS_Y;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 5)
	{
		g_map.sign[AXIS_X] = 1;
		g_map.sign[AXIS_Y] = 1;
		g_map.sign[AXIS_Z] = -1;
		g_map.map[AXIS_X] = AXIS_Y;
		g_map.map[AXIS_Y] = AXIS_X;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 6)
	{
		g_map.sign[AXIS_X] = 1;
		g_map.sign[AXIS_Y] = -1;
		g_map.sign[AXIS_Z] = -1;
		g_map.map[AXIS_X] = AXIS_X;
		g_map.map[AXIS_Y] = AXIS_Y;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 7)
	{
		g_map.sign[AXIS_X] = -1;
		g_map.sign[AXIS_Y] = -1;
		g_map.sign[AXIS_Z] = -1;
		g_map.map[AXIS_X] = AXIS_Y;
		g_map.map[AXIS_Y] = AXIS_X;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
	else		
	{
		g_map.sign[AXIS_X] = 1;
		g_map.sign[AXIS_Y] = 1;
		g_map.sign[AXIS_Z] = 1;
		g_map.map[AXIS_X] = AXIS_X;
		g_map.map[AXIS_Y] = AXIS_Y;
		g_map.map[AXIS_Z] = AXIS_Z;
	}
}

static int qmc6308_get_chipid(void)
{
	unsigned char chipid = 0;
	int ret = 0; 	

	ret = qmc6308_read_block(QMC6308_CHIP_ID_REG, &chipid , 1);
	if(ret == 0){
		printf("%s: qmc6308_get_chipid failed\n",__func__);
		return 0;
	}
	printf("qmc6308_get_chipid chipid = 0x%x\n", chipid);
	
	if((chipid & sensor_mask[0]) != sensor_mask[0] ||
		(chipid & sensor_mask[1]) != sensor_mask[1] ||
		(chipid & sensor_mask[2]) != sensor_mask[2] ||
		(chipid & sensor_mask[3]) != sensor_mask[3])
	{
		ret = 0;	
	}

	return 1;
}



Magnetic_field Magn = {0, 0, 0};
//int qmc6308_read_mag_xyz(float *data)
float mag_float[3];
int qmc6308_read_mag_xyz(void)
{
	int res;
	unsigned char mag_data[6];
	short hw_d[3] = {0};
	int t1 = 0;
	unsigned char rdy = 0;
	//float mag_float[3];

	/* Check status register for data availability */
	while(!(rdy & 0x01) && (t1 < 5))
	{
		rdy = QMC6308_STATUS_REG;
		res = qmc6308_read_block(QMC6308_STATUS_REG, &rdy, 1);
		t1++;
	}

	mag_data[0] = QMC6308_DATA_OUT_X_LSB_REG;

	res = qmc6308_read_block(QMC6308_DATA_OUT_X_LSB_REG, mag_data, 6);
	if(res == 0)
	{
		return 0;
	}
	
//	printf("qmc6308 mag_data[%02x, %02x, %02x, %02x, %02x, %02x]\n",
//		mag_data[0], mag_data[1], mag_data[2],
//		mag_data[3], mag_data[4], mag_data[5]);

	hw_d[0] = (short)(((mag_data[1]) << 8) | mag_data[0]);	//三轴原始数据(LSB)
	hw_d[1] = (short)(((mag_data[3]) << 8) | mag_data[2]);
	hw_d[2] = (short)(((mag_data[5]) << 8) | mag_data[4]);
	//printf("qmc6308 data: %d, %d, %d\n", data[0], data[1], data[2]);

	//Unit:mG  1G = 100uT = 1000mG
//	hw_d[0] = (int)(hw_d[0]*1000/g_qmc6308->sensitivity);
//	hw_d[1] = (int)(hw_d[1]*1000/g_qmc6308->sensitivity);
//	hw_d[2] = (int)(hw_d[2]*1000/g_qmc6308->sensitivity);
	
	//mag_float[0] = (float)hw_d[0] / 10;		// ut
	//mag_float[1] = (float)hw_d[1] / 10;		// ut
	//mag_float[2] = (float)hw_d[2] / 10;		// ut
	
	Magn.x = mag_float[0] = (float)hw_d[0] / 1000;		// G		//三轴
	Magn.y = mag_float[1] = (float)hw_d[1] / 1000;		// G
	Magn.z = mag_float[2] = (float)hw_d[2] / 1000;		// G
	
	//data[AXIS_X] = (g_map.sign[AXIS_X]*mag_float[g_map.map[AXIS_X]]);
	//data[AXIS_Y] = (g_map.sign[AXIS_Y]*mag_float[g_map.map[AXIS_Y]]);
	//data[AXIS_Z] = (g_map.sign[AXIS_Z]*mag_float[g_map.map[AXIS_Z]]);
	
	return res;
}

//滑动窗口滤波
#define FILTER_N 12
Magnetic_field Magn_FILTER;
float Magn_X_filter_buf[FILTER_N];
float Magn_Y_filter_buf[FILTER_N];
float Magn_Z_filter_buf[FILTER_N];
unsigned char Mag_Filter_i = 0;
void Magn_Filter(void)
{
	unsigned char count = 0;
	float Magn_X_filter_sum = 0;
	float Magn_Y_filter_sum = 0;
	float Magn_Z_filter_sum = 0;
	qmc6308_read_mag_xyz();
	Magn_X_filter_buf[Mag_Filter_i] = Magn.x;
	Magn_Y_filter_buf[Mag_Filter_i] = Magn.y;
	Magn_Z_filter_buf[Mag_Filter_i] = Magn.z;
	Mag_Filter_i++;
	if(Mag_Filter_i == FILTER_N)
		Mag_Filter_i = 0;
	for(count = 0;count < FILTER_N;count++)
	{
		Magn_X_filter_sum += Magn_X_filter_buf[count];
		Magn_Y_filter_sum += Magn_Y_filter_buf[count];
		Magn_Z_filter_sum += Magn_Z_filter_buf[count];
	}
	Magn_FILTER.x = Magn_X_filter_sum/FILTER_N;//滤完波后的X轴磁数据(高斯）
	Magn_FILTER.y = Magn_Y_filter_sum/FILTER_N;//滤完波后的Y轴
	Magn_FILTER.z = Magn_Z_filter_sum/FILTER_N;//滤完波后的Z轴
}

/* Set the sensor mode */
int qmc6308_set_mode(unsigned char mode)
{
	int err = 0;
	unsigned char ctrl1_value = 0;
	
	err = qmc6308_read_block(QMC6308_CTL_REG_ONE, &ctrl1_value, 1);
	ctrl1_value = (ctrl1_value&(~0x03))|mode;
	printf("qmc6308_set_mode, 0x0A = [%02x]->[%02x]", QMC6308_CTL_REG_ONE,ctrl1_value);
	err = qmc6308_write_reg(QMC6308_CTL_REG_ONE, ctrl1_value);

	return err;
}

int qmc6308_set_output_data_rate(unsigned char rate){
	
	int err = 0;
	unsigned char ctrl1_value = 0;
	
	err = qmc6308_read_block(QMC6308_CTL_REG_ONE, &ctrl1_value, 1);
	ctrl1_value = (ctrl1_value& (~0xE8)) | (rate << 5);
	printf("qmc6308_set_output_data_rate, 0x0A = [%02x]->[%02x]", QMC6308_CTL_REG_ONE,ctrl1_value);
	err = qmc6308_write_reg(QMC6308_CTL_REG_ONE, ctrl1_value);

	return err;	
}

static int qmc6308_enable(void)
{
	int ret;

	ret = qmc6308_write_reg(0x0d, 0x40);
	ret = qmc6308_write_reg(QMC6308_CTL_REG_TWO, 0x08);
/*
	err = qmc6308_write_reg(QMC6308_CTL_REG_ONE, 0x63);
*/
	ret = qmc6308_set_output_data_rate(OUTPUT_DATA_RATE_200HZ);
	ret = qmc6308_set_mode(QMC6308_H_PFM_MODE);
	//usleep_range(20000,30000);
	
	return ret;
}

int qmc6308_init(void)
{
	int ret = 0;

	ret = qmc6308_get_chipid();
	if(ret==0)
	{
		return 0;
	}
	qmc6308_set_layout(0);
	qmc6308_enable();

	{
		unsigned char ctrl_value;
		qmc6308_read_block(QMC6308_CTL_REG_ONE, &ctrl_value, 1);
		printf("qmc6308  0x%x=0x%x \n", QMC6308_CTL_REG_ONE, ctrl_value);
		qmc6308_read_block(QMC6308_CTL_REG_TWO, &ctrl_value, 1);
		printf("qmc6308  0x%x=0x%x \n", QMC6308_CTL_REG_TWO, ctrl_value);
		qmc6308_read_block(0x0d, &ctrl_value, 1);
		printf("qmc6308  0x%x=0x%x \n", 0x0d, ctrl_value);
	}
	return 1;
}

