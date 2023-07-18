#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//ADC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

#define ADC_CH0 0 //ͨ�� 0
#define ADC_CH1 1 //ͨ�� 1
#define ADC_CH2 2 //ͨ�� 2
#define ADC_CH3 3 //ͨ�� 3
#define ADC_CH4 4 //ͨ�� 4
#define ADC_CH5 5 //ͨ�� 5
#define ADC_CH6 6 //ͨ�� 6
#define ADC_CH7 7 //ͨ�� 7
#define ADC_CH8 8 //ͨ�� 8
#define ADC_CH9 9 //ͨ�� 9
#define ADC_CH10 10 //ͨ�� 10
#define ADC_CH11 11 //ͨ�� 11
#define ADC_CH12 12 //ͨ�� 12
#define ADC_CH13 13 //ͨ�� 13
#define ADC_CH14 14 //ͨ�� 14
#define ADC_CH15 15 //ͨ�� 15
#define ADC_CH16 16 //ͨ�� 16
#define ADC_CH17 17 //ͨ�� 17
#define ADC_CH18 18 //ͨ�� 18  	      	    
	   									   
void Adc1_Init(void); 				//ADC1��ʼ��
void Adc1_Channel_Selection(u8 ch);
u16  Get_Adc1(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc1_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ 
void Adc2_Init(void); 				//ADC2��ʼ��
u16  Get_Adc2(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc2_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ
void Adc3_Init(void); 				//ADC3��ʼ��
u16  Get_Adc3(u8 ch); 				//���ĳ��ͨ��ֵ 
void Adc3_Channel_Selection(u8 ch);//ADC3ͨ���任
void Adc2_Channel_Selection(u8 ch);
#endif 















