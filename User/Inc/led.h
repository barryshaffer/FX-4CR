#ifndef __LED_H
#define __LED_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//LED ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//LED�˿ڶ���
#define LED0(x)			GPIO_Pin_Set(GPIOD,PIN14,x)		// DS0
#define LED1(x)			GPIO_Pin_Set(GPIOD,PIN15,x)		// DS1 

void LED_Init(void);	//��ʼ��		 				    
#endif

















