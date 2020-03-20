#include "stm32f10x_it.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "system.h"
#include "qst_sw_i2c.h"
#include "bsp_key.h"
#include <stdio.h> 
#include "qmc6308.h"


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef  USE_FULL_ASSERT
 
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number */
 
  printf("\n\r Wrong parameter value detected on\r\n");
  printf("       file  %s\r\n", file);
  printf("       line  %d\r\n", line);
    
  /* Infinite loop */
  /* while (1)
  {
  } */
}
#endif
 
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);
 
  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {
  }
 
  return ch;
}
 

void RCC_Configuration(void)
{
	//ErrorStatus HSEStartUpStatus;

	RCC_DeInit();//复位RCC外部设备寄存器到默认值 
	
	//使能外部晶振
	// RCC_HSEConfig(RCC_HSE_ON);
	//等待外部晶振稳定
	// HSEStartUpStatus = RCC_WaitForHSEStartUp();
	//如果外部晶振启动成功，则进行下一步操作
	// if(HSEStartUpStatus==SUCCESS)
	{
		//设置HCLK（AHB时钟）=SYSCLK 64MHz
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		//PCLK1(APB1) = HCLK/2	32MHz
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//PCLK2(APB2) = HCLK   64MHz
		RCC_PCLK2Config(RCC_HCLK_Div1);
	 
		//设置ADC时钟频率	32MHz
		RCC_ADCCLKConfig(RCC_PCLK2_Div2);

		//FLASH时序控制
		//推荐值：SYSCLK = 0~24MHz   Latency=0
		//        SYSCLK = 24~48MHz  Latency=1
		//        SYSCLK = 48~72MHz  Latency=2
		FLASH_SetLatency(FLASH_Latency_2);
		//开启FLASH预取指功能
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//PLL设置 SYSCLK/2 * 16 = 8/2*16 = 64MHz
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
		//启动PLL
		RCC_PLLCmd(ENABLE);
		//等待PLL稳定
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		//系统时钟SYSCLK来自PLL输出
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		//切换时钟后等待系统时钟稳定
		while(RCC_GetSYSCLKSource()!=0x08);
	}

	//下面是给各模块开启时钟  APB2-64MHZ,APB1-32MHZ
	//启动GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	//启动AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//启动TIM1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//64
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//32
}




/***************************************************
函数名: void USART2_Init(unsigned long bound)
说明:	串口2初始化
入口:	波特率
出口:	无
备注:	上电初始化，运行一次
****************************************************/
void USART2_Init(unsigned long bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	USART_InitTypeDef USART_InitStructure;
	
	/*config	USART clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	/*USART2 GPIO config*/	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_2;  
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF_PP; //复用推挽输出  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_3;  
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING;  //复用开漏输入  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
	/*USART2 mode Config*/	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2,ENABLE); 
}





void Timer1_Configuration(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_DeInit(TIM1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  TIM_TimeBaseStructure.TIM_Period=  5000;
  TIM_TimeBaseStructure.TIM_Prescaler= (64 - 1);
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);
  TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
  TIM_Cmd(TIM1, ENABLE);
}


uint16_t cnt_50ms = 0;
void TIM1_UP_IRQHandler(void)//5ms刷新一次
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
	{
		cnt_50ms++;
		Key1_Scan();
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


void EXIT_IRQ_PC13_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//启用IO复用时钟
	EXTI_InitTypeDef EXTI_InitStructure;
  
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);   //GPIO与中断线映射，PC13
	
	//外设EXTI的初始化变量成员
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;                     //选择了待使能或者失能的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //被使能线路的模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;       //被使能线路的触发边沿
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                    //定义选中线路的新状态

	EXTI_Init(&EXTI_InitStructure); 
}





/***************************************************
函数名: void NVIC_Configuration(void)
说明:	中断优先级定义
入口:	无
出口:	无
备注:	上电后运行单次
****************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;//定义NVIC初始化结构体
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//优先级组别2，具体参见misc.h line80

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	//飞控任务调度定时器
  NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}




void HardWave_Init(void)
{
	SystemInit();        //系统时钟初始化
	RCC_Configuration();	//时钟配置
	//GPIO_Configuration();

	USART2_Init(115200); //主串口初始化
	KEY1_GPIO_Config();		//按键IO初始化
	//EXIT_IRQ_PC13_Init(); //外部中断初始化	
	i2c_sw_gpio_config();           //软件模拟I2C初始化
	Timer1_Configuration();      //TIM4任务调度定时器
	NVIC_Configuration();        //中断优先级设置
}


