#ifndef __SI5351A_IIC_H
#define __SI5351A_IIC_H
#include "sys.h"
//#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniSTM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
   	   		   
////IO��������
//#define SI5351A_SDA_IN()  {GPIOB->MODER&=~(3<<(14*2));GPIOB->MODER|=0<<14*2;}	//PB14��������;
//#define SI5351A_SDA_OUT() {GPIOB->MODER&=~(3<<(14*2));GPIOB->MODER|=1<<14*2;} 	//PB14����Ϊ���;
//#define SI5351A_SCL_IN()  {GPIOC->MODER&=~(3<<(8*2));GPIOB->MODER|=0<<8*2;}		//PC8����Ϊ����;
////IO�����궨��	 
//#define SI5351A_IIC_SCL(x)    GPIO_Pin_Set(GPIOC,PIN8,x)	//���SCL
//#define SI5351A_READ_SCL	  GPIO_Pin_Get(GPIOC,PIN8)    	//����SCL

//#define SI5351A_IIC_SDA(x)    GPIO_Pin_Set(GPIOB,PIN14,x)	//���SDA	 
//#define SI5351A_READ_SDA  	  GPIO_Pin_Get(GPIOB,PIN14)   	//����SDA 


//IIC���в�������
//void SI5351A_IIC_Init(void);                //��ʼ��IIC��IO��				 
//void SI5351A_IIC_Start(void);				//����IIC��ʼ�ź�
//void SI5351A_IIC_Stop(void);	  			//����IICֹͣ�ź�
//void SI5351A_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
//u8 SI5351A_IIC_Read_Byte(u8 ack);//IIC��ȡһ���ֽ�
//u8 SI5351A_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
//void SI5351A_IIC_Ack(void);					//IIC����ACK�ź�
//void SI5351A_IIC_NAck(void);				//IIC������ACK�ź�

//void SI5351A_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 SI5351A_IIC_Read_One_Byte(u8 daddr,u8 addr);	
void SI5351A_Init(void);

#define SI_CLK0_CONTROL	16			// Register definitions
#define SI_CLK1_CONTROL	17
#define SI_CLK2_CONTROL	18
#define SI_SYNTH_PLL_A	26
#define SI_SYNTH_PLL_B	34
#define SI_SYNTH_MS_0		42
#define SI_SYNTH_MS_1		50
#define SI_SYNTH_MS_2		58
/*	��ʼ��λ���� */
#define SI5351_CLK0_PHASE_OFFSET			165
#define SI5351_CLK1_PHASE_OFFSET			166
#define SI5351_CLK2_PHASE_OFFSET			167
#define SI_PLL_RESET		177

#define SI_R_DIV_1		0x00			// R-division ratio definitions
#define SI_R_DIV_2		0x10
#define SI_R_DIV_4		0x20
#define SI_R_DIV_8		0x30
#define SI_R_DIV_16		0x40
#define SI_R_DIV_32		0x50
#define SI_R_DIV_64		0x60
#define SI_R_DIV_128	0x70

#define SI_CLK_SRC_PLL_A	0x00
#define SI_CLK_SRC_PLL_B	0x20

#define XTAL_FREQ	25000000
// Crystal frequency

void si5351aOutputOff(uint8_t clk);
void si5351aSetFrequency_LoA(u32 Xtalfreq,uint32_t frequency);
void si5351aSetFrequency_Bfo(u32 Xtalfreq,uint32_t frequency);
void si5351aSetFrequency_FFT(uint32_t bfo,uint32_t fft);

#endif //SI5351A_H
