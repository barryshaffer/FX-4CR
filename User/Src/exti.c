#include "exti.h"
//#include "delay.h" 
//#include "led.h" 
//#include "key.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//�ⲿ�ж�0�������
//void EXTI0_IRQHandler(void)
//{ 
//	static u8 led0sta=1,led1sta=1;
//	
//	EXTI_D1->PR1=1<<0;  //���LINE0�ϵ��жϱ�־λ  
//	if()	 
//	{
//		 
//	}		 
//}	
////�ⲿ�ж�1�������
//void EXTI1_IRQHandler(void)
//{
//	
//	EXTI_D1->PR1=1<<1;  //���LINE2�ϵ��жϱ�־λ   
//	if()	  
//	{	 
//		 
//	}		 
//}
////�ⲿ�ж�3�������
//void EXTI3_IRQHandler(void)
//{
//	static u8 led0sta=1,led1sta=1;	
//	delay_ms(10);		//����
//	EXTI_D1->PR1=1<<3;  //���LINE3�ϵ��жϱ�־λ   
//	if(KEY0==0)	 
//	{
//		led1sta=!led1sta;
//		led0sta=!led0sta;
//		LED1(led1sta);
//		LED0(led0sta); 
//	}		 
//}
////�ⲿ�ж�10~15�������
//void EXTI15_10_IRQHandler(void)
//{
//	static u8 led0sta=1;
//	delay_ms(10);		//����
//	EXTI_D1->PR1=1<<13;  //���LINE13�ϵ��жϱ�־λ  
//	if(KEY2==0)	 
//	{		
//		led0sta=!led0sta;
//		LED0(led0sta);	 		
//	}		  
//}
	   
//�ⲿ�жϳ�ʼ������
//��ʼ��PH2,PH3,PC13,PA0Ϊ�ж�����.
void EXTIX_Init(void)
{
	
	//Ex_NVIC_Config(GPIO_D,10,1);		//�½��ش���
	//Ex_NVIC_Config(GPIO_D,11,1); 		//�½��ش���
	Ex_NVIC_Config(GPIO_B,14,1);		//�½��ش���
	//Ex_NVIC_Config(GPIO_D,11,FTIR); 		//�½��ش���
	//Ex_NVIC_Config(GPIO_H,3,FTIR); 		//�½��ش���
 	//Ex_NVIC_Config(GPIO_A,0,RTIR); 	 	//�����ش��� 
	MY_NVIC_Init(15,0,EXTI15_10_IRQn,4);	//��ռ3�������ȼ�2����2
	//MY_NVIC_Init(0,9,EXTI1_IRQn,0);		//��ռ2�������ȼ�2����2	   
	//MY_NVIC_Init(1,2,EXTI15_10_IRQn,2);	//��ռ1�������ȼ�2����2	   
	//MY_NVIC_Init(0,2,EXTI0_IRQn,2);		//��ռ0�������ȼ�2����2	   
}












