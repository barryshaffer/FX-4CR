#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//DMA ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 

void ADC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);//����DMAx_CHx
void ADC3_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);//
void DAC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);//
void DAC2_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);
void USART1_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);
void USART3_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//ʹ��һ��DMA����		   
#endif






























