#include "gpio_set.h"


/*
*
*/
/*
***************************************************
*	
***************************************************
*/
void GPIO_Init()
{
	RCC->AHB4ENR|=1<<0;     //ʹ��PORT Aʱ�� 
	RCC->AHB4ENR|=1<<1;     //ʹ��PORT Bʱ�� 
	RCC->AHB4ENR|=1<<2;     //ʹ��PORT Cʱ�� 
	RCC->AHB4ENR|=1<<3;     //ʹ��PORT Dʱ��
	RCC->AHB4ENR|=1<<4;     //ʹ��PORT Dʱ��
	SYSCFG->PMCR =0;
	//SYSCFG->PMCR |=1<<27;
	SYSCFG->PMCR |=1<<26;
	//
	//	��Դ����
	//
	GPIO_Set(GPIOB,PIN5,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU);   /* POWER_SET */
	//
	// �շ����Ƽ� PTT
	//
	GPIO_Set(GPIOB,PIN14,GPIO_MODE_IN,1,0,GPIO_PUPD_PU);   /* PTT */
	//
	// �շ�ת������IO
	//
	GPIO_Set(GPIOD,PIN7,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU);   // TX/RX
	//
	// ��������
	//
	GPIO_Set(GPIOD,PIN15,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD);   // KEY_BL
	//
	// AMP�߷�
	//
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU);   //LIF ATT
	//
	// IF ATT
	//
	GPIO_Set(GPIOC,PIN1,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU);   //LIF ATT
	//
	// �������� SQL
	//
	//GPIO_Set(GPIOB,PIN0,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD);   //SQL
	//
	// CW ���
	//
	GPIO_Set(GPIOD,PIN10,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);   // CWK_A
	GPIO_Set(GPIOD,PIN11,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);   // CWK_B
	// 
	//	��������
	//
	GPIO_Set(GPIOD,PIN14,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU);
	//
	//	������Ƶ����ָʾ
	//
	//GPIO_Set(GPIOC,PIN8,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
	//
	//	��������ָʾ
	//
	GPIO_Set(GPIOC,PIN9,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
	//
	//	MIC����
	//
	GPIO_Set(GPIOA,PIN13,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU);
	GPIO_Set(GPIOB,PIN4,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU);
	//
	// �������
	//
	GPIO_Set(GPIOC,PIN8,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
	GPIO_Set(GPIOB,PIN4,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);
	//
	// ��������
	//
	//GPIO_Set(GPIOB,PIN3,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD);   // CWK_B
	//
	//�����ͨ���ο���
	//
	GPIO_Set(GPIOD,PIN3,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 80M
	GPIO_Set(GPIOD,PIN4,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 60M
	GPIO_Set(GPIOD,PIN5,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 40M
	GPIO_Set(GPIOD,PIN6,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 30M
	//GPIO_Set(GPIOD,PIN11,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU); // 20M
	//GPIO_Set(GPIOD,PIN10,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU); // 17M/15
	//GPIO_Set(GPIOD,PIN9,GPIO_MODE_OUT, 0,0,GPIO_PUPD_PU); // 12M/10M
	//GPIO_Set(GPIOD,PIN8,GPIO_MODE_OUT, 0,0,GPIO_PUPD_PU); // Radio
	//
	// �����ͨ���ο���
	//
	GPIO_Set(GPIOD,PIN0,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 80M
	GPIO_Set(GPIOC,PIN12,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 60M/40M
	GPIO_Set(GPIOD,PIN1,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 30M/20
	GPIO_Set(GPIOC,PIN10,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 17M/15M
	GPIO_Set(GPIOD,PIN2,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 12M/10M
	GPIO_Set(GPIOA,PIN14,GPIO_MODE_OUT,0,0,GPIO_PUPD_PD); // 6M
	
	TR_CONTROL(CONTROL_RX);
/*
******************************************************************************
*	����IO
******************************************************************************
*/
	//GPIO_Set(GPIOA,PIN3,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
//	GPIO_Set(GPIOA,PIN14,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,���� 
//	GPIO_Set(GPIOA,PIN13,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
//	GPIO_Set(GPIOB,PIN2,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,����
//	GPIO_Set(GPIOE,PIN7,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
//	GPIO_Set(GPIOE,PIN8,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,���� 
//	GPIO_Set(GPIOE,PIN9,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
//	GPIO_Set(GPIOE,PIN10,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,����
//	GPIO_Set(GPIOE,PIN15,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
//	GPIO_Set(GPIOE,PIN5,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,���� 
//	GPIO_Set(GPIOE,PIN6,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
//	GPIO_Set(GPIOC,PIN13,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,����
//	GPIO_Set(GPIOC,PIN14,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
//	GPIO_Set(GPIOC,PIN15,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,���� 
	//GPIO_Set(GPIOA,PIN13,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
	//GPIO_Set(GPIOB,PIN2,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,����
}
