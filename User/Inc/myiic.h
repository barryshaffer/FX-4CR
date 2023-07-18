#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
   	   		   
//IO��������
#define SDA_IN()  {GPIO_Set(GPIOB,PIN7,GPIO_MODE_IN,0,1,GPIO_PUPD_PU);}//PC11����ģʽ
#define SDA_OUT() {GPIO_Set(GPIOB,PIN7,GPIO_MODE_OUT,0,1,GPIO_PUPD_PU);}//PC11���ģʽ
//#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}//����
//#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;}//���
//IO��������	 
#define IIC_SCL(x)    GPIO_Pin_Set(GPIOB,PIN6,x)//SCL
#define IIC_SDA(x)    GPIO_Pin_Set(GPIOB,PIN7,x)//SDA	 
#define READ_SDA   	  GPIO_Pin_Get(GPIOB,PIN7)//����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź� 
#endif
















