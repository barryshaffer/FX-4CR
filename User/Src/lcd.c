#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
//#include "usart.h"	  
#include "dwt.h"  
//#include "ltdc.h"
/***************************************************************************************
MCU ��STM32F407VGT6
���� ��8MHZ
��Ƶ ��168MHZ
����˵��:
//-------------------------------------------------------------------------------------
#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_9  //PB9 ������TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10������TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 ������TFT --CS
#define LCD_RST     	GPIO_Pin_12	//PB12������TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13������TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15������TFT - SDI
#define
//VCC:���Խ�5VҲ���Խ�3.3V
//LED:���Խ�5VҲ���Խ�3.3V����ʹ���������IO����(�ߵ�ƽʹ��)
//GND���ӵ�Դ��
//˵��������Ҫ��������ռ��IO�����Խ�LCD_CS�ӵأ�LCD_LED��3.3V��LCD_RST������Ƭ����λ�ˣ�
//�������ͷ�3������IO
//�ӿڶ�����Lcd_Driver.h�ڶ��壬
//������IO�ӷ������������ʵ�ʽ����޸���ӦIO��ʼ��LCD_GPIO_Init()
#define
//-----------------------------------------------------------------------------------------

**********************************************************************************************/
//����LCD��Ҫ����
//Ĭ��Ϊ����
//_lcd_dev lcddev;

/****************************************************************************
* ��    �ƣ�void LCD_GPIO_Init(void)
* ��    �ܣ�STM32_ģ��SPI���õ���GPIO��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʼ��ģ��SPI���õ�GPIO
****************************************************************************/
//void LCD_GPIO_Init(void)
//{

//	GPIO_InitTypeDef  GPIO_InitStructure;
//	      
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 |  GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//      
//}
/****************************************************************************
* ��    �ƣ�void  SPIv_WriteData(u8 Data)
* ��    �ܣ�STM32_ģ��SPIдһ���ֽ����ݵײ㺯��
* ��ڲ�����Data
* ���ڲ�������
* ˵    ����STM32_ģ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
void  SPIv_WriteData(u16 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
	  LCD_SDA_SET; //�������
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

/****************************************************************************
* ��    �ƣ�u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* ��    �ܣ�STM32_Ӳ��SPI��дһ���ֽ����ݵײ㺯��
* ��ڲ�����SPIx,Byte
* ���ڲ��������������յ�������
* ˵    ����STM32_Ӳ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
void SPI_WriteByte(u16 TxData)
{
	//u8 RxData=0;	
	SPI2->CR1|=1<<0;				//SPE=1,ʹ��SPI2
	SPI2->CR1|=1<<9;  				//CSTART=1,��������
	
	while((SPI2->SR&1<<1)==0);		//�ȴ��������� 
	*(vu8 *)&SPI2->TXDR=TxData;		//����һ��byte,�Դ��䳤�ȷ���TXDR�Ĵ���   
	//while((SPI2->SR&1<<0)==0);		//�ȴ�������һ��byte  
	//RxData=*(vu8 *)&SPI2->RXDR;		//����һ��byte,�Դ��䳤�ȷ���RXDR�Ĵ���	
	
	SPI2->IFCR|=3<<3;				//EOTC��TXTFC��1,���EOT��TXTFCλ 
	SPI2->CR1&=~(1<<0);				//SPE=0,�ر�SPI2,��ִ��״̬����λ/FIFO���õȲ���
	//return RxData;					//�����յ�������	
} 
/****************************************************************************
* ��    �ƣ�SPI2_Init(void)
* ��    �ܣ�STM32_SPI2Ӳ�����ó�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����STM32_SPI2Ӳ�����ó�ʼ��
****************************************************************************/
void SPI2_Init(void)	
{
	
	u32 tempreg=0;
	RCC->AHB4ENR|=1<<1;    	//ʹ��PORTBʱ��	
	RCC->AHB4ENR|=1<<2;    	//ʹ��PORTBʱ��	
	RCC->APB1LENR|=1<<14;		//SPI2ʱ��ʹ�� 
	GPIO_Set(GPIOB,PIN10|PIN11|PIN12, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);
	GPIO_Set(GPIOB,PIN13|PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	//GPIO_Set(GPIOB,PIN13|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	//GPIO_Set(GPIOC,PIN5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PD);//����
	//GPIO_AF_Set(GPIOB,12,5);	//PB12,AF5
 	GPIO_AF_Set(GPIOB,13,5);	//PB13,AF5
 	GPIO_AF_Set(GPIOB,15,5);	//PB15,AF5 
	//LCD_BL(1);
	//����SPI��ʱ��Դ
	RCC->D2CCIP1R&=~(7<<12);	//SPI123SEL[2:0]=0,���ԭ��������
	RCC->D2CCIP1R|=0<<12;		//SPI123SEL[2:0]=1,ѡ��pll1_q_ck��ΪSPI1/2/3��ʱ��Դ,һ��Ϊ200Mhz
								//��:spi_ker_ck=200Mhz 
	//����ֻ���SPI�ڳ�ʼ��
	RCC->APB1LRSTR|=1<<14;		//��λSPI2
	RCC->APB1LRSTR&=~(1<<14);	//ֹͣ��λSPI2
	
	SPI2->CR1|=1<<12;			//SSI=1,�����ڲ�SS�ź�Ϊ�ߵ�ƽ	
	SPI2->CFG1|=0<<28;			//MBR[2:0]=0,����spi_ker_ckΪ2��Ƶ.
	SPI2->CFG1|=7<<0;			//DSIZE[4:0]=7,����SPI֡��ʽΪ8λ,���ֽڴ���
	tempreg=(u32)1<<31;			//AFCNTR=1,SPI���ֶ�IO�ڵĿ���
	tempreg|=0<<29;				//SSOE=0,��ֹӲ��NSS���
	tempreg|=1<<26;				//SSM=1,�������NSS��
	tempreg|=1<<25;				//CPOL=1,����״̬��,SCKΪ�ߵ�ƽ
	tempreg|=0<<24;				//CPHA=0,���ݲ����ӵ�2��ʱ����ؿ�ʼ
	tempreg|=0<<23;				//LSBFRST=0,MSB�ȴ���
	tempreg|=1<<22;				//MASTER=1,����ģʽ
	tempreg|=0<<19;				//SP[2:0]=0,Ħ��������ʽ
	tempreg|=1<<17;				//COMM[1:0]=0,����ͨ��
	//tempreg|=2<<4;
	SPI2->CFG2=tempreg;			//����CFG2�Ĵ���	
	SPI2->I2SCFGR&=~(1<<0);		//ѡ��SPIģʽ
	SPI2->CR1|=1<<0;			//SPE=1,ʹ��SPI2
	//SPI2->CFG1|=2<<28;
	//SPI_WriteByte(0xff);	//��������		 
}

/****************************************************************************
* ��    �ƣ�Lcd_WriteIndex(u8 Index)
* ��    �ܣ���Һ����дһ��8λָ��
* ��ڲ�����Index   �Ĵ�����ַ
* ���ڲ�������
* ˵    ��������ǰ����ѡ�п��������ڲ�����
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
   LCD_CS_CLR;
   LCD_RS_CLR;
//#if USE_HARDWARE_SPI   
   SPI_WriteByte(Index);
//#else
   //SPIv_WriteData(Index);
//#endif 
   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�Lcd_WriteData(u8 Data)
* ��    �ܣ���Һ����дһ��8λ����
* ��ڲ�����dat     �Ĵ�������
* ���ڲ�������
* ˵    �����������ָ����ַд�����ݣ��ڲ�����
****************************************************************************/
//void Lcd_WriteData(u16 Data)
void Lcd_WriteData(u8 Data)	
{
   LCD_CS_CLR;
   LCD_RS_SET;
//#if USE_HARDWARE_SPI  	
   SPI_WriteByte(Data);	
//#else
   //SPIv_WriteData(Data);
//#endif 
   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�void LCD_WriteReg(u8 Index,u16 Data)
* ��    �ܣ�д�Ĵ�������
* ��ڲ�����Index,Data
* ���ڲ�������
* ˵    ����������Ϊ��Ϻ�������Index��ַ�ļĴ���д��Dataֵ
****************************************************************************/
void LCD_WriteReg(u8 Index,u8 Data)
{
	Lcd_WriteIndex(Index);
  	Lcd_WriteData(Data);
}

/****************************************************************************
* ��    �ƣ�void Lcd_WriteData_16Bit(u16 Data)
* ��    �ܣ���Һ����дһ��16λ����
* ��ڲ�����Data
* ���ڲ�������
* ˵    �����������ָ����ַд��һ��16λ����
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{	
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);	
}
/*******************************************************************************
* Function Name  : LCD_RD_DATA     �٣�
* Description    : ��LCD���ݺ���
* Input          : None
* Output         : None
* Return         : ������ֵ
*******************************************************************************/
u16 LCD_RD_DATA(void)  //�⺯����
{
	vu16 ram;			//��ֹ���Ż�
//	ram=LCD->LCD_RAM;	
	return ram;	 
}
/****************************************************************************
* ��    �ƣ�void Lcd_Reset(void)
* ��    �ܣ�Һ��Ӳ��λ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��ǰ��ִ��һ�θ�λ����
****************************************************************************/
void Lcd_Reset(void)
{
	//LCD_RST_SET;
	//delay_ms(100);
	LCD_RST_CLR;
	delay_ms(200);
	LCD_RST_SET;
	delay_ms(100);
}
/****************************************************************************
* ��    �ƣ�void Lcd_Init(void)
* ��    �ܣ�Һ����ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��_ILI9225_176X220
****************************************************************************/
void Lcd_Init(void)
{	
	SPI2_Init();
	Lcd_Reset(); //Reset before LCD Init.

//	lcddev.width=320;    //LCD ���
//	lcddev.height=240;   //LCD �߶�
//	lcddev.setxcmd=0X2A;  //����x����ָ��
//	lcddev.setycmd=0X2B;  //����y����ָ��
//	lcddev.wramcmd=0X2C;  //��ʼдgramָ��
	//delay_ms(120);                //ms

	delay_ms(100);
	/* Sleep Out */
	Lcd_WriteIndex(0x11);
	delay_ms(120);    //Delay 120ms
	/* Memory Data Access Control */
	//LCD_WriteReg(0x36,0x60);//���� ������ ���ϵ���
	 Lcd_WriteIndex(0x36);			
	Lcd_WriteData(0x70);
	/* RGB 5-6-5-bit  */
	//LCD_WriteReg(0x3A,0x05);
	 Lcd_WriteIndex(0x3A);			
	Lcd_WriteData(0x05);

	 Lcd_WriteIndex(0xB2);			
	Lcd_WriteData(0x0C);
	Lcd_WriteData(0x0C); 
	Lcd_WriteData(0x00); 
	Lcd_WriteData(0x33); 
	Lcd_WriteData(0x33); 			

	 Lcd_WriteIndex(0xB7);			
	Lcd_WriteData(0x35);

	 Lcd_WriteIndex(0xBB);			
	Lcd_WriteData(0x32); //Vcom=1.35V
					
	 Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x01);

	 Lcd_WriteIndex(0xC3);			
	Lcd_WriteData(0x15); //GVDD=4.8V  ��ɫ���
				
	 Lcd_WriteIndex(0xC4);			
	Lcd_WriteData(0x20); //VDV, 0x20:0v

	 Lcd_WriteIndex(0xC6);			
	Lcd_WriteData(0x0F); //0x0F:60Hz        	

	 Lcd_WriteIndex(0xD0);			
	Lcd_WriteData(0xA4);
	Lcd_WriteData(0xA1); 

	 Lcd_WriteIndex(0xE0);
	Lcd_WriteData(0xD0);   
	Lcd_WriteData(0x08);   
	Lcd_WriteData(0x0E);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x05);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x33);   
	Lcd_WriteData(0x48);   
	Lcd_WriteData(0x17);   
	Lcd_WriteData(0x14);   
	Lcd_WriteData(0x15);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x34);   

	Lcd_WriteIndex(0xE1);     
	Lcd_WriteData(0xD0);   
	Lcd_WriteData(0x08);   
	Lcd_WriteData(0x0E);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x15);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x33);   
	Lcd_WriteData(0x48);   
	Lcd_WriteData(0x17);   
	Lcd_WriteData(0x14);   
	Lcd_WriteData(0x15);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x34);
	
	Lcd_WriteIndex(0x21);
	Lcd_WriteIndex(0x11);
	delay_ms(120);
	Lcd_WriteIndex(0x29);
	//Lcd_WriteIndex(0x11);
	//delay_ms(120);    //Delay 120ms
//	LCD_WriteReg(0xB2, 0x0C);
//    LCD_WriteReg(0xB2, 0x0C); 
//    LCD_WriteReg(0xB2, 0x00);   
//    LCD_WriteReg(0xB2, 0x33);   
//    LCD_WriteReg(0xB2, 0x33);   

//    LCD_WriteReg(0xB7, 0x35);   //VGH=13.26V, VGL=-10.43V

//    LCD_WriteReg(0xBB, 0x32);   //VCOM

//   // LCD_WriteReg(0xC0, 0x2C);   

//    LCD_WriteReg(0xC2, 0x01);   

//    LCD_WriteReg(0xC3, 0x15); //VAP  //5V

//    LCD_WriteReg(0xC4, 0x20);   

//    LCD_WriteReg(0xC6, 0x0F);   

//    LCD_WriteReg(0xD0, 0xA4);   
//    LCD_WriteReg(0xD0, 0xA1);   

//    LCD_WriteReg(0xE0 , 0xD0);   
//    LCD_WriteReg(0xE0, 0x08);   
//    LCD_WriteReg(0xE0, 0x0E);   
//    LCD_WriteReg(0xE0, 0x09);   
//    LCD_WriteReg(0xE0 , 0x09);   
//    LCD_WriteReg(0xE0 , 0x05);   
//    LCD_WriteReg(0xE0, 0x31);   
//    LCD_WriteReg(0xE0, 0x33);   
//    LCD_WriteReg(0xE0, 0x48);   
//    LCD_WriteReg(0xE0, 0x17);   
//    LCD_WriteReg(0xE0 , 0x14);   
//    LCD_WriteReg(0xE0, 0x15);   
//    LCD_WriteReg(0xE0 , 0x31);   
//    LCD_WriteReg(0xE0 , 0x34);   


//    LCD_WriteReg(0xE1, 0xD0);   
//    LCD_WriteReg(0xE1 , 0x08);   
//    LCD_WriteReg(0xE1, 0x0E);   
//    LCD_WriteReg(0xE1 , 0x09);   
//    LCD_WriteReg(0xE1, 0x09);   
//    LCD_WriteReg(0xE1, 0x15);   
//    LCD_WriteReg(0xE1 , 0x31);   
//    LCD_WriteReg(0xE1, 0x33);   
//    LCD_WriteReg(0xE1, 0x48);   
//    LCD_WriteReg(0xE1 , 0x17);   
//    LCD_WriteReg(0xE1 , 0x14);   
//    LCD_WriteReg(0xE1, 0x15);   
//    LCD_WriteReg(0xE1, 0x31);   
//    LCD_WriteReg(0xE1 , 0x34);
//	
//	LCD_WriteReg(0x21, 0x00);
//    LCD_WriteReg(0x29,0x00);      

//    LCD_WriteReg(0x2A, 0x00);   
//    //LCD_WriteReg(0x2A , 0);   
//    LCD_WriteReg(0x2A , 0x00);   
//    LCD_WriteReg(0x2A , 0xEF);   
//     
//    LCD_WriteReg(0x2B, 0x00);   
//    //LCD_WriteReg(0x2B , 0);   
//    LCD_WriteReg(0x2B , 0x00);   
//    LCD_WriteReg(0x2B , 0xEF);   

//	Lcd_WriteIndex(0x13);
//	delay_ms(20);
//	Lcd_WriteIndex(0x29);
//	delay_ms(20);
//	Lcd_WriteIndex(0x36);
//	delay_ms(20);
//	Lcd_WriteData(0x70);
	//LCD_BL(1);
	//Lcd_SetRegion(0, 0,319,239);
}



/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{	
	Lcd_WriteIndex(0x2A);
	Lcd_WriteData_16Bit( Xpos);
	Lcd_WriteIndex(0x2B);
	Lcd_WriteData_16Bit( Ypos);
	Lcd_WriteIndex(0x2C);		
} 
/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
//������ʾ����
void Lcd_SetRegion(u16 xStart, u16 yStart,u16 xEnd,u16 yEnd)
{
	Lcd_WriteIndex(0x2A);
	Lcd_WriteData_16Bit(xStart);	
	Lcd_WriteData_16Bit(xEnd);
	
	Lcd_WriteIndex(0x2B);
	Lcd_WriteData_16Bit(yStart);	
	Lcd_WriteData_16Bit(yEnd);
	
	Lcd_WriteIndex(0x2C);	
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�����xy�������ɫ����
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);
}    

/*****************************************
 �������ܣ���TFTĳһ�����ɫ                          
 ���ڲ�����color  ����ɫֵ                                 
******************************************/
u16 Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//���������ֽ�
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}

/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
//void Lcd_Clear(u16 x0, u16 y0,u16 x1,u16 y1,u16 Color)               
//{	
//   unsigned int i,m;
//   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
//   for(i=0;i<X_MAX_PIXEL;i++)
//    for(m=0;m<Y_MAX_PIXEL;m++)
//    {	
//	  	Lcd_WriteData_16Bit(Color);
//    }  

//}
void Lcd_Color(u16 x0, u16 y0,u16 x1,u16 y1,u16 Color)               
{	
 static  u16 i,m;
   Lcd_SetRegion(x0,y0,x1,y1);
   for(i=x0;i<(x1+1);i++)
    for(m=y0;m<(y1+1);m++)
    {	
	  	Lcd_WriteData_16Bit(Color);
		//Lcd_WriteData(Color);
    } 
	//Lcd_SetRegion(0,0,319,239);	
}

//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
u16 LCD_BGR2RGB(u16 c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}



// ��Բ����
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{//Bresenham�㷨 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//���ߺ�����ʹ��Bresenham �����㷨
void Gui_DrawLine(u16 x1, u16 y1,u16 x2, u16 y2,u16 Color)   
{
u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		 Gui_DrawPoint(uRow,uCol,Color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/*
**************************************************************************
* ���Ⱥ��ַ�
**************************************************************************
*/
void Gui_Unequal_character(u16 x,u16 y,u16 fc)
{
	Gui_DrawLine(x,y+4,x+6,y+4,fc);
	Gui_DrawLine(x,y+7,x+6,y+7,fc);
	Gui_DrawLine(x+5,y+2,x+1,y+9,fc);
}
/*
***************************************************************************
*
***************************************************************************
*/
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	Gui_DrawLine(x,y,x+w,y,0xEF7D);
	Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
	Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
	
}
/*
********************************************************************
*
********************************************************************
*/
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		Gui_DrawLine(x,y,x+w,y,0xEF7D);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
		Gui_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		Gui_DrawLine(x,y,x+w,y,0x2965);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		Gui_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		Gui_DrawLine(x,y,x+w,y,0xffff);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
		Gui_DrawLine(x,y,x,y+h,0xffff);
	}
}


/*
*************************************************************************************
* ��������: ����Ļ��ʾһ͹��İ�ť��
* ��    ��: u16 x1,y1,x2,y2 ��ť�����ϽǺ����½�����
* ��    ��: ��
*************************************************************************************
*/
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
	Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
	Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
	Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
	Gui_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
	Gui_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

/*
*************************************************************************************
* ��������: ����Ļ��ʾһ���µİ�ť��
* ��    ��: u16 x1,y1,x2,y2 ��ť�����ϽǺ����½�����
* ��    ��: ��
*************************************************************************************
*/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
	Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V
	
	Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
	Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
	Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}
/*
*************************************************************************************
* ��������: ����Ļ��ʾһԲ�Ǿ���
* ��    ��: u16 x,y, �������
*			w:��ȣ�h:�߶ȣ�fc:���α�����ɫ��
* ��    ��: ��
*************************************************************************************
*/
void LCD_RectangleWindow(u16 x, u16 y, u16 w, u16 h, u16 fc)
{
	u16 x0,y0;
	for(y0=0;y0<h;y0 += h-1)
	{
		for(x0=2;x0<w-2;x0 +=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
	for(x0=0;x0<w;x0 +=w-1)
	{
		for(y0=2;y0<h-2;y0 +=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
	for(x0=1;x0<w-1;x0 +=w-3)
	{
		for(y0=1;y0<h-1;y0 +=h-3)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
}
//******************************************************************
//��������  LCD_ShowChar
//���ܣ�    ��ʾ����Ӣ���ַ�
//���������(x,y):�ַ���ʾλ����ʼ����
//        	fc:ǰ�û�����ɫ
//			bc:������ɫ
//			num:�ַ�
//			size:�����С
//			mode:ģʽ  0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  
void LCD_ShowChar(u16 x,u16 y,u16 fc,u16 bc,u8 size,u8 mode,u8 num)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);	//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	//if(size==48)
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	//else num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else if(size==32)temp=asc2_3216[num][t];	//����3216����
		//else if(size==40)temp=asc2_4020[num][t];	//����3216����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)Gui_DrawPoint(x,y,fc);
			else if(mode==0)Gui_DrawPoint(x,y,bc);
			temp<<=1;
			y++;
			if(y>=240)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=320)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//******************************************************************
//��������  LCD_ShowString
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʾӢ���ַ���
//���������x,y :�������	 
//width,height:�����С  
//size:		�����С
//mode:		0����䣬1�����ӣ�
//*p:�ַ�����ʼ��ַ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  	  
void LCD_ShowString(u16 x,u16 y,u16 fc,u16 bc,u8 size,u8 mode,char *p)
{         
	u8 x0=x;
	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=319){x=x0;y+=size;}
        if(y>=239)break;//�˳�
        LCD_ShowChar(x,y,fc,bc,size,mode,*p);
        x+=size/2;
        p++;
    }  
}

//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u8 i;
	u32 result=1;
	for(i=0;i<n;i++)
	{
		result*=m;
	}		
	return result;
}
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum_Step(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num,u8 step)
{         	
	u8 t,temp;
	u8 enshow=0;
	u16 fc0;
	//len=(u8)(log10(num));	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		//temp=(u8)log10(num);
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,' ');
				continue;
			}else enshow=1;  
		}
		if(step==len-1-t)fc0=RED;
		else fc0=fc;
			
	 	LCD_ShowChar(x+(size/2)*t,y,fc0,bc,size,0,temp+'0'); 
	}
}
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num)
{         	
	u8 t,temp;
	u8 enshow=0;
	//len=(u8)(log10(num));	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		//temp=(u8)log10(num);
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,' ');
				continue;
			}else enshow=1;
	
		}
	 	LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,temp+'0'); 
	}
}
void LCD_Showvol(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num)
{         	
	u8 t,temp;
	u8 enshow=0;
	//len=(u8)(log10(num));	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		//temp=(u8)log10(num);
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,' ');
				continue;
			}else enshow=1;
	
		}
	 	LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,temp+'0'); 
	}
}
/*
*********************************************************************
* ��ʾ����,��λΪ0,������ʾ  YZ
* x,y:�������
* num:��ֵ(0~999999999);	 
* len:����(��Ҫ��ʾ��λ��)
* size:�����С
* mode:
* [7]:0,�����;1,���0.
* [6:1]:����
* [0]:0,�ǵ�����ʾ;1,������ʾ.
*********************************************************************
*/
void LCD_ShowxNum(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u8 mode,u32 num)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,mode&0X01,'0');  
				else LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,mode&0X01,' ');  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,mode&0X01,temp+'0'); 
	}
}
/*
******************************************************************************
* Ƶ����ʾ����  
* x,y:�������
* freq:��ֵ(0~999999999);
* fc: ������ɫ
* bc: ��ɫ
* size:�����С
******************************************************************************
*/
void LCD_Frequency_Display(u16 x, u16 y, u16 fc, u16 bc, u8 size, u32 freq)
{
	u16 a, b, c;
	a = freq/1000000 ;
	b = freq%1000000/1000 ;
	c = freq%1000/10 ;
	LCD_ShowNum(x, y, fc, bc, 3, size, a);
	LCD_ShowChar(x+(size/2)*3, y, fc, bc,size,0,'.');
	LCD_ShowxNum(x+(size/2)*4, y, fc, bc, 3, size, 0x80, b);
	LCD_ShowChar(x+(size/2)*7,y, fc, bc,size,0,'.');
	LCD_ShowxNum(x+(size/2)*8, y, fc, bc, 2, size, 0x80, c);
}





