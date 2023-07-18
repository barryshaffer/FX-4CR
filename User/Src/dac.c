#include "dac.h"
#include "dwt.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//FX4-4C
//DAC ��������	   
//BG2IXD
//��������:2020/9/18
//�汾��V1.0
//��Ȩ���У�����ؾ���
////////////////////////////////////////////////////////////////////////////////// 	

//DACͨ��1����ʱ���DSP��������Ƶ������ʱ���DSP����ӹ������Ƶ�źţ�
//ͨ���󼶵Ŀ���ת������Ӧ��·��
//DACͨ��2����ʱ����AGC�����AGC���Ƶ�ѹ��

//DACͨ��1�����ʼ��
void Dac_Init(void)
{   	
	RCC->APB1LENR|=1<<29;	//ʹ��DACʱ��	   
	RCC->AHB4ENR|=1<<0;  	//ʹ��PORTAʱ��	  
	GPIO_Set(GPIOA,PIN4,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA4,ģ������,����   
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,���� 
	/* DAC1 */
	DAC1->CCR=0;
	DAC1->MCR&=~(7<<0);		//MODE1[2:0]����
	DAC1->MCR |=0<<0;			//MODE1[2:0]=0,DACͨ��1���ӵ��ⲿ����,��ʹ��BUFFER
	
	DAC1->CR=0;				//DR�Ĵ�������
	DAC1->CR|=1<<0;			//ʹ��DAC1 
	DAC1->CR|=1<<1;			//TEN1=0,ʹ�ô������� 
	DAC1->CR|=5<<2;			//TSEL[3:0]=0,��ʱ��TIM6 TRGO����
	DAC1->CR|=0<<6;			//WAVE1[1:0]=0,��ʹ�ò��η���
 	DAC1->CR|=1<<12;		//DMAEN1=0,DAC1 DMAʹ��    
	DAC1->CR|=0<<14;		//CEN1=0,DAC1��������ͨģʽ 

	DAC1->CR|=1<<0;			//ʹ��DAC1 
	DAC1->DHR12R1=0;
	/* DAC2 */
	
	DAC1->MCR&=~(7<<16);		//MODE1[2:0]����
	DAC1->MCR |=0<<16;			//MODE1[2:0]=0,DACͨ��1���ӵ��ⲿ����,��ʹ��BUFFER
	
	DAC1->CR|=1<<16;		//ʹ��DAC2 
	DAC1->CR|=0<<17;		//TEN1=0,��ʹ�ô������� 
	DAC1->CR|=0<<18;		//TSEL[3:0]=0,�������
	DAC1->CR|=0<<22;		//WAVE1[1:0]=0,��ʹ�ò��η���
 	//DAC1->CR|=1<<28;		//DMAEN1=0,DAC1 DMA��ʹ��    
	DAC1->CR|=0<<30;		//CEN1=0,DAC1��������ͨģʽ 

	DAC1->CR|=1<<16;		//ʹ��DAC2 
	DAC1->DHR12R2=0;
}
//����ͨ��1�����ѹ
//vol:0~3300,����0~3.3V
void Dac1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC1->DHR12R1=temp;
}
//����ͨ��1�����ѹ
//vol:0~3300,����0~3.3V
void Dac2_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC1->DHR12R2=temp;
}


















































