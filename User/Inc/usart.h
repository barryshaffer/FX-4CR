#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "stdio.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//����1��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/6
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��  
//V1.1 20180815
//��USART1_IRQHandler��������������������,��ֹ���ڽ����������������bug
////////////////////////////////////////////////////////////////////////////////// 

#define USART_RX_LEN  			32  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define UART_TX_BUF_MAX_SIZE	64
extern u8 DMA_IsReady ;
typedef struct{
	unsigned char head;
	unsigned char rear;
	unsigned char size;
	unsigned char dat[UART_TX_BUF_MAX_SIZE];
}UartTxBufDef;

extern uint8_t Uart_Send_Buffer[UART_TX_BUF_MAX_SIZE];
extern u8  USART_RX_BUF[USART_RX_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void uart1_init(u32 pclk2,u32 bound);
void uart3_init(u32 pclk2,u32 bound);

u8 DMA_IsTxComplete(void);
void CopyTBufToDMA(void);
void UART_DMAStart(uint8_t length);
void UartTx(uint8_t *wdat, uint8_t len);
unsigned char UART_TxBufSize(void);
void UART_DMAEnd(void);
void UART_Task(void);
void UART3_Task(void);
void UART3_DMAEnd(void);
#endif	   
















