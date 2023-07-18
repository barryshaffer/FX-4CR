#include "dma.h"																	   	  
#include "dwt.h"
#include "Audio_driver.h"
/*
******************************************************************************	 
* 	��������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;��						 *
* 	ʹ�ã�FX-4C																 *
* 	���ܣ�DMA ��������														 *   
* 	���ߣ�BG2IXD																 *
* 	��������:2020/9/18														 *
* 	�汾��V1.0																 *
* 				����Ȩ���У�����ؾ���											 *									  
******************************************************************************
*/
extern  __attribute__((section (".RAM_D1"))) uint8_t Uart_Send_Buffer[64];
extern  __attribute__((section (".RAM_D1"))) uint8_t Uart3_Send_Buffer[64];
extern  __attribute__((section (".RAM_D1"))) u16 ADC1_Buff0[SAMPLE_SIZE];
extern  __attribute__((section (".RAM_D1"))) u16 ADC1_Buff1[SAMPLE_SIZE];
extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff0[SAMPLE_SIZE ];
extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff1[SAMPLE_SIZE ];
extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff0[SAMPLE_SIZE];	
extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff1[SAMPLE_SIZE];
extern  __attribute__((section (".RAM_D1"))) u16 DAC2_Buff0[SAMPLE_SIZE/2];	
extern  __attribute__((section (".RAM_D1"))) u16 DAC2_Buff1[SAMPLE_SIZE/2];
/*
******************************************************************************************************
* DMA1��������7���ģ�9��ͨ������
* ��������ADC1�����ݴ���
* ������->�洢��ģʽ/16λ���ݿ��/�洢������ģʽ/˫����ģʽ
* DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7������ѡStream7
* chx:DMAͨ��ѡ��,��Χ:1~115(���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 116)��
* ADC1�ǵ�9ͨ����
* par:�����ַ: ADC1->DR
* mar:�洢����ַ: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:���ݴ����� 
******************************************************************************************************
*/
void ADC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2ʱ��ʹ��  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;							//���֮ǰ��stream�ϵ������жϱ�־

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//�����DMA2,ͨ�����+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//�õ���Ӧ��DMAMUXͨ�����Ƶ�ַ
	DMAMUXx->CCR=chx&0XFF;		//ͨ��ѡ��
	
	DMA_Streamx->PAR=(u32)&ADC1->DR;	//DMA�����ַ
	DMA_Streamx->M0AR=(u32)&ADC1_Buff0;		//DMA �洢��0��ַ
	DMA_Streamx->M1AR=(u32)&ADC1_Buff1;		//DMA �洢��1��ַ
	DMA_Streamx->NDTR=SAMPLE_SIZE;		//�������ݵ���Ŀ
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	//DMA_Streamx->CR|=1<<4;	//��������ж�ʹ��
	DMA_Streamx->CR|=0<<5;		//0��DMA ����������
	DMA_Streamx->CR|=0<<6;		//���赽�洢��ģʽ
	DMA_Streamx->CR|=1<<8;		//ѭ��ģʽ(��ʹ��˫����ģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=1<<11;		//�������ݳ���; 0:8λ,1:16λ
	DMA_Streamx->CR|=1<<13;		//�洢�����ݳ���:16λ
	DMA_Streamx->CR|=0<<16;		//�͵����ȼ�
	DMA_Streamx->CR|=1<<18;		//0���������ʱ���л���������1��DMA �������ʱ�л�Ŀ��洢��
	//DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	//DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	 
	DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
	
	//MY_NVIC_Init(0,15,DMA1_Stream0_IRQn,0);//��ռ2�������ȼ�0����2	
	
	//DMA_Streamx->CR|=1<<0;		//����DMA����
}
/*
********************************************************************************************
* DMA1��������6���ģ�115��ͨ������
* ��������ADC3�����ݴ���
* ������->�洢��ģʽ/16λ���ݿ��/�洢������ģʽ/˫����ģʽ
* DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7������ѡStream7
* chx:DMAͨ��ѡ��,��Χ:1~115(���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 116)��
* ADC3�ǵ�115ͨ����
* par:�����ַ: ADC3->DR
* mar:�洢����ַ: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:���ݴ����� 
*********************************************************************************************
*/
void ADC3_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2ʱ��ʹ��  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;							//���֮ǰ��stream�ϵ������жϱ�־

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//�����DMA2,ͨ�����+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//�õ���Ӧ��DMAMUXͨ�����Ƶ�ַ
	DMAMUXx->CCR=chx&0XFF;		//ͨ��ѡ��
	
	DMA_Streamx->PAR=(u32)&ADC3->DR;	//DMA�����ַ
	DMA_Streamx->M0AR=(u32)&ADC_Buff0;		//DMA �洢��0��ַ
	DMA_Streamx->M1AR=(u32)&ADC_Buff1;		//DMA �洢��1��ַ
	DMA_Streamx->NDTR=SAMPLE_SIZE;		//�������ݵ���Ŀ
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	DMA_Streamx->CR|=1<<4;		//��������ж�ʹ��
	DMA_Streamx->CR|=0<<5;		//0��DMA ����������
	DMA_Streamx->CR|=0<<6;		//���赽�洢��ģʽ
	DMA_Streamx->CR|=1<<8;		//ѭ��ģʽ(��ʹ��˫����ģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=1<<11;		//�������ݳ���; 0:8λ,1:16λ
	DMA_Streamx->CR|=1<<13;		//�洢�����ݳ���:16λ
	//DMA_Streamx->CR|=1<<15;
	DMA_Streamx->CR|=2<<16;		//�е����ȼ�
	DMA_Streamx->CR|=1<<18;		//0���������ʱ���л���������1��DMA �������ʱ�л�Ŀ��洢��
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	 
	DMA_Streamx->FCR=0X21;		//FIFO���ƼĴ���
	
	MY_NVIC_Init(0,12,DMA2_Stream0_IRQn,0);//��ռ2�������ȼ�0����2	
	//DMA_Streamx->CR|=1<<0;		//����DMA����
}
/*
********************************************************************************************
* DMA1��������5���ģ�67��ͨ������
* ��������DAC1�����ݴ���
* �Ӵ洢������->����ģʽ/16λ���ݿ��/�洢������ģʽ/˫����ģʽ
* DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7������ѡStream5
* chx:DMAͨ��ѡ��,��Χ:1~115(���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 116)��
*     DAC1�ǵ�67ͨ����
* par:�����ַ: DAC->DHR12R1
* mar:�洢����ַ: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:���ݴ����� 
*********************************************************************************************
*/
void DAC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2ʱ��ʹ��  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;							//���֮ǰ��stream�ϵ������жϱ�־

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//�����DMA2,ͨ�����+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//�õ���Ӧ��DMAMUXͨ�����Ƶ�ַ
	DMAMUXx->CCR=chx&0XFF;		//ͨ��ѡ��
	
	DMA_Streamx->PAR=(u32)&DAC1->DHR12R1;	//DMA�����ַ
	DMA_Streamx->M0AR=(u32)&DAC2_Buff0 ;		//DMA �洢��0��ַ
	DMA_Streamx->M1AR=(u32)&DAC2_Buff1 ;		//DMA �洢��1��ַ
	DMA_Streamx->NDTR=SAMPLE_SIZE/2;		//�������ݵ���Ŀ
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	//DMA_Streamx->CR|=1<<4;		//��������ж�ʹ��
	DMA_Streamx->CR|=0<<5;		//0��DMA ����������
	DMA_Streamx->CR|=1<<6;		//�洢��������ģʽ
	DMA_Streamx->CR|=1<<8;		//ѭ��ģʽ(��ʹ��˫����ģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=1<<11;		//�������ݳ���; 0:8λ,1:16λ
	DMA_Streamx->CR|=1<<13;		//�洢�����ݳ���:16λ
	DMA_Streamx->CR|=1<<16;		//�е����ȼ�
	DMA_Streamx->CR|=1<<18;		//0���������ʱ���л���������1��DMA �������ʱ�л�Ŀ��洢��
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	 
	DMA_Streamx->FCR=0X21;		//FIFO���ƼĴ���
	
	//MY_NVIC_Init(2,0,DMA1_Stream5_IRQn,0);//��ռ2�������ȼ�0����2	
	//DMA_Streamx->CR|=1<<0;		//����DMA����
} 
/*
********************************************************************************************
* DMA1��������6���ģ�68��ͨ������
* ��������DAC1�����ݴ���
* �Ӵ洢������->����ģʽ/16λ���ݿ��/�洢������ģʽ/˫����ģʽ
* DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7������ѡStream6
* chx:DMAͨ��ѡ��,��Χ:1~115(���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 116)��
*     DAC2�ǵ�68ͨ����
* par:�����ַ: DAC->DHR12R2
* mar:�洢����ַ: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:���ݴ����� 
*********************************************************************************************
*/
void DAC2_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2ʱ��ʹ��  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;							//���֮ǰ��stream�ϵ������жϱ�־

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//�����DMA2,ͨ�����+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//�õ���Ӧ��DMAMUXͨ�����Ƶ�ַ
	DMAMUXx->CCR=chx&0XFF;		//ͨ��ѡ��
	
	DMA_Streamx->PAR=(u32)&DAC1->DHR12R2;	//DMA�����ַ
	DMA_Streamx->M0AR=(u32)&DAC2_Buff0 ;		//DMA �洢��0��ַ
	DMA_Streamx->M1AR=(u32)&DAC2_Buff1 ;		//DMA �洢��1��ַ
	DMA_Streamx->NDTR=SAMPLE_SIZE;		//�������ݵ���Ŀ
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	//DMA_Streamx->CR|=1<<4;		//��������ж�ʹ��
	DMA_Streamx->CR|=0<<5;		//0��DMA ����������
	DMA_Streamx->CR|=1<<6;		//�洢��������ģʽ
	DMA_Streamx->CR|=1<<8;		//ѭ��ģʽ(��ʹ��˫����ģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=1<<11;		//�������ݳ���; 0:8λ,1:16λ
	DMA_Streamx->CR|=1<<13;		//�洢�����ݳ���:16λ
	DMA_Streamx->CR|=1<<16;		//�е����ȼ�
	DMA_Streamx->CR|=1<<18;		//0���������ʱ���л���������1��DMA �������ʱ�л�Ŀ��洢��
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	 
	DMA_Streamx->FCR=0X21;		//FIFO���ƼĴ���
	
	//MY_NVIC_Init(2,0,DMA1_Stream5_IRQn,0);//��ռ2�������ȼ�0����2	
	//DMA_Streamx->CR|=1<<0;		//����DMA����
}
//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,��Χ:1~115(���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 116)
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ����� 

void USART1_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2ʱ��ʹ��  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;							//���֮ǰ��stream�ϵ������жϱ�־

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//�����DMA2,ͨ�����+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//�õ���Ӧ��DMAMUXͨ�����Ƶ�ַ
	DMAMUXx->CCR=chx&0XFF;		//ͨ��ѡ��
	
	DMA_Streamx->PAR=(u32)&USART1->TDR;		//DMA�����ַ
	DMA_Streamx->M0AR=(u32)&Uart_Send_Buffer;		//DMA �洢��0��ַ
	DMA_Streamx->NDTR=64;		//DMA ���ݴ�����
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	DMA_Streamx->CR|=1<<4;		//��������ж�ʹ��
	DMA_Streamx->CR|=1<<6;		//�洢��������ģʽ
	DMA_Streamx->CR|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=0<<11;		//�������ݳ���:8λ
	DMA_Streamx->CR|=0<<13;		//�洢�����ݳ���:8λ
	DMA_Streamx->CR|=1<<16;		//�е����ȼ�
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	 
	//DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
	//MY_NVIC_Init(15,0,DMA2_Stream5_IRQn,4);//��ռ2�������ȼ�0����2
} 
//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	while(DMA_Streamx->CR&0X1);	//ȷ��DMA���Ա�����  
	DMA_Streamx->NDTR=ndtr;		//DMA �洢��0��ַ 
	DMA_Streamx->CR|=1<<0;		//����DMA����
}	  
//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,��Χ:1~115(���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 116)
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ����� 

void USART3_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2ʱ��ʹ��  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;							//���֮ǰ��stream�ϵ������жϱ�־

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//�����DMA2,ͨ�����+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//�õ���Ӧ��DMAMUXͨ�����Ƶ�ַ
	DMAMUXx->CCR=chx&0XFF;		//ͨ��ѡ��
	
	DMA_Streamx->PAR=(u32)&USART3->TDR;		//DMA�����ַ
	DMA_Streamx->M0AR=(u32)&Uart3_Send_Buffer;		//DMA �洢��0��ַ
	DMA_Streamx->NDTR=64;		//DMA ���ݴ�����
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	DMA_Streamx->CR|=1<<4;		//��������ж�ʹ��
	DMA_Streamx->CR|=1<<6;		//�洢��������ģʽ
	DMA_Streamx->CR|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=0<<11;		//�������ݳ���:8λ
	DMA_Streamx->CR|=0<<13;		//�洢�����ݳ���:8λ
	DMA_Streamx->CR|=1<<16;		//�е����ȼ�
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	 
	//DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
	//MY_NVIC_Init(15,0,DMA2_Stream5_IRQn,4);//��ռ2�������ȼ�0����2
}


























