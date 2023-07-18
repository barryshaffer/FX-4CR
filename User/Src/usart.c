#include "sys.h"
#include "usart.h"
#include "dma.h"
#include "cat_kenwood.h"
////////////////////////////////////////////////////////////////////////////////// 	 
#define IC_706 0x48
#define CIV_ADDR IC_706

unsigned char UartRxLen = 0;
unsigned char UartRxBuf[32];
static UartTxBufDef UTBuffer;
 u8 DMA_IsReady = 1;
u8 DMA3_IsReady = 1;
__attribute__((section (".RAM_D1"))) uint8_t Uart_Send_Buffer[UART_TX_BUF_MAX_SIZE];
__attribute__((section (".RAM_D1"))) uint8_t Uart3_Send_Buffer[UART_TX_BUF_MAX_SIZE];
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//���HAL��ʹ��ʱ,ĳЩ������ܱ����bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

void UartBufInit()
{
  UTBuffer.head = 0;
  UTBuffer.rear = 0;
  UTBuffer.size	= 0;
}

void UartTxBufWrite(unsigned char wdat)
{
	if(UTBuffer.size < UART_TX_BUF_MAX_SIZE)
	{
		UTBuffer.dat[ UTBuffer.rear++ ] = wdat;
		if(UTBuffer.rear >= UART_TX_BUF_MAX_SIZE)
		{
		  UTBuffer.rear = 0;
		}
		UTBuffer.size ++;	
	}
}

void UartTx(uint8_t *wdat, uint8_t len)
{
	for(uint8_t i=0; i<len; i++)
	{
		UartTxBufWrite(wdat[i]);
	}
}

unsigned char UartTxBufRead()
{
	unsigned char rval = 0;
	if(UTBuffer.size > 0)
	{
		rval = UTBuffer.dat[ UTBuffer.head++ ];
		if(UTBuffer.head >= UART_TX_BUF_MAX_SIZE)
		{
		  UTBuffer.head = 0;
		}							
		UTBuffer.size --; 
	}
  
  return rval;
}

unsigned char UART_TxBufSize()
{
	return UTBuffer.size;
}

void usart_rx_analysis(uint8_t wdat)
{
	if(wdat == ';')
	{
		Kenwood_CmdParser(UartRxBuf, UartRxLen);
		UartRxLen = 0;
	}
	else
	{
		UartRxBuf[ UartRxLen] = wdat;
		UartRxLen++;
		if(UartRxLen > USART_RX_LEN-1 )UartRxLen = 0;  
	}
}
void usart3_rx_analysis(uint8_t wdat)
{
	if(wdat == ';')
	{
		Kenwood_CmdParser(UartRxBuf, UartRxLen);
		UartRxLen = 0;
	}
	else
	{
		UartRxBuf[ UartRxLen ] = wdat;
		UartRxLen++;
		if(UartRxLen >USART_RX_LEN -1)UartRxLen = 0;  
	}
}
u8 DMA_IsTxComplete()
{
	return DMA_IsReady;
}

void CopyTBufToDMA()
{
	uint8_t len = UTBuffer.size;
	for(uint8_t i=0; i<len; i++)
	{
		Uart_Send_Buffer[i] = UartTxBufRead();
	}
}
void UART_DMAStart(uint8_t length)
{
	DMA_IsReady = 0;
	USART1->CR3=1<<7;
	MYDMA_Enable(DMA1_Stream7,length);
}
void UART_DMAEnd()
{
	if(DMA1->HISR&(1<<27))	//�ȴ�DMA2_Steam7�������
	{
		DMA1->HIFCR|=1<<27;	//���DMA2_Steam7������ɱ�־
	
		//�����ٴη��� 
		DMA_IsReady = 1;
		DMA1_Stream7->CR&=~(1<<0); 	//�ر�DMA���� 
	}
}
//
void UART_Task()
{
	uint8_t size = 0;
	if (DMA_IsReady)
	{
		size = UART_TxBufSize(); 
		if(size > 0)
		{
			// ���ڷ��ʹ���
			CopyTBufToDMA();
			UART_DMAStart( size );	
		}
	}
	//UART_DMAEnd();
}
////����1�жϷ������
////ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//u8 USART_RX_BUF[USART_RX_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
////����״̬
////bit15��	������ɱ�־
////bit14��	���յ�0x0d
////bit13~0��	���յ�����Ч�ֽ���Ŀ
//u16 USART_RX_STA=0;       //����״̬���	 
  
void USART1_IRQHandler(void)
{
	u8 res;	

	if(USART1->ISR&(1<<5))//���յ�����
	{	 
		res = USART1->RDR; 
		usart_rx_analysis(res);	
		//UART_Task();
	} 
	USART1->ICR|=1<<3;	//����������,������ܻῨ���ڴ����жϷ���������
} 
/*
****************************************************************************
*	�������� ����3
****************************************************************************
*/
void Uart3_CopyTBufToDMA()
{
  uint8_t len = UTBuffer.size;
  for(uint8_t i=0; i<len; i++)
  {
    Uart3_Send_Buffer[i] = UartTxBufRead();
  }
}
void UART3_DMAStart(uint8_t length)
{
	DMA3_IsReady = 0;
	USART3->CR3	=1<<7;           //ʹ�ܴ���1��DMA���� 
	MYDMA_Enable(DMA1_Stream6,length);
}
void UART3_DMAEnd()
{
	if(DMA1->HISR&(1<<21))	//�ȴ�DMA2_Steam7�������
	{
		DMA1->HIFCR|=1<<21;	//���DMA2_Steam7������ɱ�־
	
		//�����ٴη��� 
		DMA3_IsReady = 1;
		DMA1_Stream6->CR&=~(1<<0); 	//�ر�DMA���� 
	}
}
//
void UART3_Task()
{
	uint8_t size = 0;
	if (DMA3_IsReady)
	{
		size = UART_TxBufSize(); 
		if(size > 0)
		{
			// ���ڷ��ʹ���
			//UART3_DMAStart( size );	
			Uart3_CopyTBufToDMA();
			UART3_DMAStart( size );	
		}
	}
	//UART_DMAEnd();
}
//
void USART3_IRQHandler(void)
{
	u8 res;	

	if(USART3->ISR&(1<<5))//���յ�����
	{	 
		res = USART3->RDR; 
		usart3_rx_analysis(res);	
		//UART3_Task();
	} 
	USART3->ICR|=1<<3;	//����������,������ܻῨ���ڴ����жϷ���������
} 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//ע��:USART1��ʱ��,�ǿ���ͨ��RCC_D2CCIP2Rѡ���
//��������һ����Ĭ�ϵ�ѡ�񼴿�(Ĭ��ѡ��rcc_pclk2��Ϊ����1ʱ��)
//pclk2��APB2��ʱ��Ƶ��,һ��Ϊ100Mhz
//bound:������ 
void uart1_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;
	
	UartBufInit();
	
	temp=(pclk2*1000000+bound/2)/bound;	//�õ�USARTDIV@OVER8=0,���������������
	RCC->AHB4ENR|=1<<0;   	//ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<4;  	//ʹ�ܴ���1ʱ�� 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//����������
 	USART1->BRR=temp; 		//����������@OVER8=0 	
	USART1->CR1=0;		 	//����CR1�Ĵ���
	USART1->CR1|=0<<28;	 	//����M1=0
	USART1->CR1|=0<<12;	 	//����M0=0&M1=0,ѡ��8λ�ֳ� 
	USART1->CR1|=0<<15; 	//����OVER8=0,16�������� 
	USART1->CR1|=1<<3;  	//���ڷ���ʹ�� 
	USART1->CR1|=1<<2;  	//���ڽ���ʹ��
	USART1->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	
	
	USART1->CR2=0;		 	//����CR1�Ĵ���
	USART1->CR2|=0<<11;	 	//����ʱ�ӽ�ֹ
	USART1->CR2|=0<<10;	 	//���ݵ͵�ƽ��Ч
	USART1->CR2|=1<<9; 	//����CPHAʹ�����ڵ�2�����ص�ʱ�򱻲��� 
	USART1->CR2|=0<<8;  	//�������һλ,ʹ��Ӧ��ʱ�����岻���������SCLK���� 
	//USART1->CR3=1<<7;           //ʹ�ܴ���1��DMA����   
	MY_NVIC_Init(14,0,USART1_IRQn,4);//��2��������ȼ� 
	USART1->CR1|=1<<0;  	//����ʹ
	USART1_TXDMA_Config(DMA1_Stream7,42);//DMA2,STEAM7,
	MY_NVIC_Init(14,0,DMA1_Stream7_IRQn,4);//��ռ2�������ȼ�0����2
}

void DMA1_Stream7_IRQHandler (void) 
{ 
	
	if(DMA1->HISR&(1<<27))	//�ȴ�DMA1_Steam7�������
	{
		DMA1->HIFCR|=1<<27;	//���DMA1_Steam7������ɱ�־
		DMA_IsReady = 1;
		//USART1->ICR|=1<<6;
		DMA1_Stream7->CR&=~(1<<0); 	//�ر�DMA����  
	}
	
}

//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//ע��:USART1��ʱ��,�ǿ���ͨ��RCC_D2CCIP2Rѡ���
//��������һ����Ĭ�ϵ�ѡ�񼴿�(Ĭ��ѡ��rcc_pclk2��Ϊ����1ʱ��)
//pclk2��APB2��ʱ��Ƶ��,һ��Ϊ100Mhz
//bound:������ 
void uart3_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;
	
	UartBufInit();
	
	temp=(pclk2*1000000+bound/2)/bound;	//�õ�USARTDIV@OVER8=0,���������������
	RCC->AHB4ENR|=1<<3;   	//ʹ��PORTA��ʱ��  
	RCC->APB1LENR|=1<<18;  	//ʹ�ܴ���3ʱ�� 
	GPIO_Set(GPIOD,PIN8|PIN9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOD,8,7);	//PA9,AF7
	GPIO_AF_Set(GPIOD,9,7);//PA10,AF7  	   
	//����������
 	USART3->BRR=temp; 		//����������@OVER8=0 	
	USART3->CR1=0;		 	//����CR1�Ĵ���
	USART3->CR1|=0<<28;	 	//����M1=0
	USART3->CR1|=0<<12;	 	//����M0=0&M1=0,ѡ��8λ�ֳ� 
	USART3->CR1|=0<<15; 	//����OVER8=0,16�������� 
	USART3->CR1|=1<<3;  	//���ڷ���ʹ�� 
	USART3->CR1|=1<<2;  	//���ڽ���ʹ��
	USART3->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	
	
	USART3->CR2=0;		 	//����CR1�Ĵ���
	USART3->CR2|=0<<11;	 	//����ʱ�ӽ�ֹ
	USART3->CR2|=0<<10;	 	//���ݵ͵�ƽ��Ч
	USART3->CR2|=1<<9; 	//����CPHAʹ�����ڵ�2�����ص�ʱ�򱻲��� 
	USART3->CR2|=0<<8;  	//�������һλ,ʹ��Ӧ��ʱ�����岻���������SCLK���� 
	USART3->ICR|=1<<6;
	//USART1->CR3=1<<7;           //ʹ�ܴ���1��DMA����   
	MY_NVIC_Init(14,0,USART3_IRQn,4);//��2��������ȼ� 
	//USART3->CR1|=1<<0;  	//����ʹ
	USART3_TXDMA_Config(DMA1_Stream6,46);//DMA2,STEAM7,
	MY_NVIC_Init(14,0,DMA1_Stream6_IRQn,4);//��ռ2�������ȼ�0����2
}

void DMA1_Stream6_IRQHandler (void) 
{ 
	
	if(DMA1->HISR&(1<<21))	//�ȴ�DMA1_Steam6�������
	{
		DMA1->HIFCR|=1<<21;	//���DMA1_Steam6������ɱ�־
		DMA3_IsReady = 1;
		DMA1_Stream6->CR&=~(1<<0); 	//�ر�DMA����  
	}
	
}









