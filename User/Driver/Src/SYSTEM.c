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

	RCC_DeInit();//��λRCC�ⲿ�豸�Ĵ�����Ĭ��ֵ 
	
	//ʹ���ⲿ����
	// RCC_HSEConfig(RCC_HSE_ON);
	//�ȴ��ⲿ�����ȶ�
	// HSEStartUpStatus = RCC_WaitForHSEStartUp();
	//����ⲿ���������ɹ����������һ������
	// if(HSEStartUpStatus==SUCCESS)
	{
		//����HCLK��AHBʱ�ӣ�=SYSCLK 64MHz
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		//PCLK1(APB1) = HCLK/2	32MHz
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//PCLK2(APB2) = HCLK   64MHz
		RCC_PCLK2Config(RCC_HCLK_Div1);
	 
		//����ADCʱ��Ƶ��	32MHz
		RCC_ADCCLKConfig(RCC_PCLK2_Div2);

		//FLASHʱ�����
		//�Ƽ�ֵ��SYSCLK = 0~24MHz   Latency=0
		//        SYSCLK = 24~48MHz  Latency=1
		//        SYSCLK = 48~72MHz  Latency=2
		FLASH_SetLatency(FLASH_Latency_2);
		//����FLASHԤȡָ����
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//PLL���� SYSCLK/2 * 16 = 8/2*16 = 64MHz
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
		//����PLL
		RCC_PLLCmd(ENABLE);
		//�ȴ�PLL�ȶ�
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		//ϵͳʱ��SYSCLK����PLL���
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		//�л�ʱ�Ӻ�ȴ�ϵͳʱ���ȶ�
		while(RCC_GetSYSCLKSource()!=0x08);
	}

	//�����Ǹ���ģ�鿪��ʱ��  APB2-64MHZ,APB1-32MHZ
	//����GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	//����AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//����TIM1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//64
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//32
}




/***************************************************
������: void USART2_Init(unsigned long bound)
˵��:	����2��ʼ��
���:	������
����:	��
��ע:	�ϵ��ʼ��������һ��
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
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF_PP; //�����������  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_3;  
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING;  //���ÿ�©����  
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
void TIM1_UP_IRQHandler(void)//5msˢ��һ��
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//����IO����ʱ��
	EXTI_InitTypeDef EXTI_InitStructure;
  
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);   //GPIO���ж���ӳ�䣬PC13
	
	//����EXTI�ĳ�ʼ��������Ա
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;                     //ѡ���˴�ʹ�ܻ���ʧ�ܵ��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //��ʹ����·��ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;       //��ʹ����·�Ĵ�������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                    //����ѡ����·����״̬

	EXTI_Init(&EXTI_InitStructure); 
}





/***************************************************
������: void NVIC_Configuration(void)
˵��:	�ж����ȼ�����
���:	��
����:	��
��ע:	�ϵ�����е���
****************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;//����NVIC��ʼ���ṹ��
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//���ȼ����2������μ�misc.h line80

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	//�ɿ�������ȶ�ʱ��
  NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}




void HardWave_Init(void)
{
	SystemInit();        //ϵͳʱ�ӳ�ʼ��
	RCC_Configuration();	//ʱ������
	//GPIO_Configuration();

	USART2_Init(115200); //�����ڳ�ʼ��
	KEY1_GPIO_Config();		//����IO��ʼ��
	//EXIT_IRQ_PC13_Init(); //�ⲿ�жϳ�ʼ��	
	i2c_sw_gpio_config();           //���ģ��I2C��ʼ��
	Timer1_Configuration();      //TIM4������ȶ�ʱ��
	NVIC_Configuration();        //�ж����ȼ�����
}


