#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//2.8��/3.5��/4.3��/7�� TFTҺ������	  
//֧������IC�ͺŰ���:ILI9341/NT35310/NT35510/SSD1963��		    
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/8
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved	 
//////////////////////////////////////////////////////////////////////////////////	 
//����˵��
//V1.1 20180608
//������RGBLCD����֧��(ʹ��LTDC).
//V1.2 20180806
//FMCƵ�ʸ�Ϊ220Mhz,��������LCD_Init����FMCҺ��������ز���
//////////////////////////////////////////////////////////////////////////////////	 

#define RED  	0xf800
#define RED1  	0x8000
#define RED2  	0x4000
#define RED3  	0x2000
#define GREEN	0x07e0
#define GREEN1	32<<5
#define GREEN2	16<<5
#define GREEN3	8<<5
#define GREEN4	4<<5
#define GREEN5	2<<5
#define GREEN6	1<<5

#define BLUE 	0x001f
#define BLUE1 	0x0010
#define BLUE2 	0x0008
#define BLUE3 	0x0004
#define BLUE4 	0x0002

#define WHITE	0xffff
#define BLACK	0x0000

#define YELLOW  0xFFE0
#define YELLOW0  32<<11|64<<5
#define YELLOW1  16<<11|32<<5
#define YELLOW2  8<<11|16<<5
#define YELLOW3  4<<11|8<<5

#define GRAY0   0xEF7D   	    //��ɫ0  11101 111011 11101
#define GRAY1   0xC618      	//��ɫ1  11000 110000 11000
#define GRAY2   0x8410      	//��ɫ2  10000 100000 10000
#define GRAY3   0x4208      	//��ɫ2  01000 010000 01000
#define GRAY4   0x2104      	//��ɫ2  00100 001000 00100
#define GRAY5   0x1082      	//��ɫ2  00010 000100 00010
#define GRAY6   0x0841      	//��ɫ2  00001 000010 00001
//�����Գ���ʹ�õ���ģ��SPI�ӿ�����
//�����ɸ��Ľӿ�IO���ã�ʹ����������4 IO������ɱ���Һ��������ʾ
/******************************************************************************
�ӿڶ�����Lcd_Driver.h�ڶ��壬����ݽ����޸Ĳ��޸���ӦIO��ʼ��LCD_GPIO_Init()
#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_9  //PB9 ������TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10������TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 ������TFT --CS
#define LCD_RST     	GPIO_Pin_12	//PB12������TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13������TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15������TFT - SDI
*******************************************************************************/
//#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
////#define LCD_LED        	GPIO_Pin_9  //MCU_PB9--->>TFT --BL
//#define LCD_RS         	GPIO_Pin_10	//PB11--->>TFT --RS/DC
//#define LCD_CS        	GPIO_Pin_11 //MCU_PB11--->>TFT --CS/CE
//#define LCD_RST     	GPIO_Pin_12	//PB10--->>TFT --RST
//#define LCD_SCL        	GPIO_Pin_13	//PB13--->>TFT --SCL/SCK
//#define LCD_SDA        	GPIO_Pin_15	//PB15 MOSI--->>TFT --SDA/DIN


//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//Һ�����ƿ���1�������궨��
//#define LCD_BL(x)		GPIO_Pin_Set(GPIOC,PIN5,x)//����

#define	LCD_RS_SET  	GPIO_Pin_Set(GPIOB,PIN10,1)//PBout(10)=1//GPIOB->BSRR|=1<<10    
#define	LCD_CS_SET  	GPIO_Pin_Set(GPIOB,PIN11,1)//PBout(11)=1//GPIOB->BSRR|=1<<11   
#define	LCD_RST_SET  	GPIO_Pin_Set(GPIOB,PIN12,1)//PBout(12)=1//GPIOB->BSRR|=1<<12
#define	LCD_SCL_SET  	GPIO_Pin_Set(GPIOB,PIN13,1)//PBout(13)=1//GPIOB->BSRR|=1<<13   
#define	LCD_SDA_SET  	GPIO_Pin_Set(GPIOB,PIN15,1)//PBout(15)=1//GPIOB->BSRR|=1<<15    
//#define	LCD_LED_SET  	LCD_CTRL->BSRR=LCD_LED   

//Һ�����ƿ���0�������궨��
#define	LCD_RS_CLR  	GPIO_Pin_Set(GPIOB,PIN10,0)//PBout(10)=0//GPIOB->BRR|=1<<10   
#define	LCD_CS_CLR  	GPIO_Pin_Set(GPIOB,PIN11,0)//PBout(11)=0//GPIOB->BRR|=1<<11  
#define	LCD_RST_CLR  	GPIO_Pin_Set(GPIOB,PIN12,0)//PBout(12)=0//GPIOB->BRR|=1<<12   
#define	LCD_SCL_CLR  	GPIO_Pin_Set(GPIOB,PIN13,0)//PBout(13)=0//GPIOB->BRR|=1<<13   
#define	LCD_SDA_CLR  	GPIO_Pin_Set(GPIOB,PIN15,0)//PBout(15)=0//GPIOB->BRR|=1<<15     
//#define	LCD_LED_CLR  	LCD_CTRL->BRR=LCD_LED 


//#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //�������
//#define LCD_DATAIN     LCD_DATA->IDR;   //��������

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

u32 LCD_Pow(u8 m,u8 n);
//void delay_ms(u32 a);
void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void LCD_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u16 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_WriteData_16Bit(u16 Data);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
u16 Lcd_ReadPoint(u16 x,u16 y);

void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
//void Lcd_Clear(u16 x0, u16 y0,u16 x1,u16 y1,u16 Color  );
void Lcd_Color(u16 x0, u16 y0,u16 x1,u16 y1,u16 Color); 

u16 LCD_BGR2RGB(u16 c);
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);
void Gui_Unequal_character(u16 x,u16 y,u16 fc);
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
//void DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 m);
//void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
//void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_RectangleWindow(u16 x, u16 y, u16 w, u16 b, u16 fc);
void LCD_ShowChar(u16 x,u16 y,u16 fc,u16 bc,u8 size,u8 mode,u8 num);
void LCD_ShowString(u16 x,u16 y,u16 fc,u16 bc,u8 size,u8 mode,char *p);
void LCD_ShowNum_Step(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num,u8 step);
void LCD_ShowNum(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num);
void LCD_Showvol(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num);
void LCD_ShowxNum(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u8 mode,u32 num);
void LCD_Frequency_Display(u16 x, u16 y, u16 fc, u16 bc, u8 size, u32 freq);
#endif
