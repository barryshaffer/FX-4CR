/*
**************************************************
* FX-4C ui�ײ����� 
* �汾 V1.1��
* ���ߣ�CG2IXD�� 
* ʱ�䣺2020/9/9��
**************************************************
*/
/*
**************************************************
* LCD������320*240
* 
**************************************************
*/
#include "ui_Driver.h"
#include "lcd.h"
#include "rtc.h"
#include "arm_math.h" 
//#include <stdio.h>
#include "stdlib.h"
#include "Audio_driver.h"
#include "KEY_Control.h"
#include "gpio_set.h"
#include "adc.h"
#include "timer.h"
#include "dsp_filter.h"
#include "24cxx.h"
#include<math.h>
//
//arm_rfft_fast_instance_f32 Spe_FFT;
//extern __IO At24c_addr_GenInfo	vfo[2];
extern u32  Menu_Values[MENU_END];
extern  u16 Rx_amp_adjust[12];
/*
********************************************************************************
* ������ʾ����
* ��С��320*24��������������������ʾʱ�� ��Դ��ѹ����Ϣ
********************************************************************************
*/
	/*
	* ������ʾ���򱳾���ɫ���ú���
	* ��ʾ��������������ɫ�𽥵�����
	* xStart:0,xEnd:319; yXtart:0,yEnd:23��
	*/
void ui_TopBackground(u16 x,u16 y,u16 width,u16 h)
{
	//static u8 start =0;
	u16 x0,y0;
	//if(start ==0)
	//{
		for(y0=0;y0<h;y0++)	/*�߶���24����*/
		{
			for(x0=0;x0<width;x0++)
			{
				Gui_DrawPoint(x0+x,y0+y,24-y0-y);
			}
		}
	//	start =1;
	//}
}
//
/*	������ͼ�λ���	*/
void key_lock_Display(u16 x)
{
	u8 x0,y0;
	LCD_ShowString(x+1,4,RED,BLACK ,16,0,"0");
	for(y0=13;y0<20;y0++)	/*�߶���24����*/
	{
		for(x0=0;x0<11;x0++)
		{
			Gui_DrawPoint(x0+x,y0,RED);
		}
	}
}
/* ATT��ʾ��������ı��궨�� */
#define UI_ATT_START_X 	50
#define UI_ATT_START_Y 	4
#define UI_ATT_TEXT		"ATT"
	/*
	* ATT˥��������ʾ����
	* k��ATT���أ�
	* k=0��ATT�ر�OFF;
	* k=1��ATT����ON��
	*/
void ui_ATT_Display(u8 k)
{
	u16 fc,bc;
	if(k ==2)
	{
		fc =GRAY0;
		bc =RED;
		ui_TopBackground(50,4,32,16);
		LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT2");	
	}
	else
	if(k ==1)
	{
		fc =GRAY0;
		bc =RED;
		ui_TopBackground(50,4,32,16);
		LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT1");
	}
	else
	if(k ==0)	
	{
		fc =GRAY2;
		bc =BLUE2 ;
		ui_TopBackground(50,4,32,16);
		LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT ");
	}
	//LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT");
}

	/*
	* ��С�����������ʾ����С����ǰ��ʾ��λС�����һλ�������ʾ99.9����������ֵ��ʵ����ֵ*10��
	* x:	ˮƽ��㣻
	* y��	��ֱ��㣻
	* fc��	������ɫ��
	* bc��	�����ɫ��
	* font�������С��
	* num:  ������ֵ��ʵ����ֵ*10��num=138��ʾ13.8��
	* text����ֵ������ʾ���ı�������text='V',13.8V;text='W',5.0W;
	* on��	������ʾ�Ŀ��أ�on=0�����ڹرգ�on=1,���ڿ�����
	*/
void ui_Decimal_Dispaly(u16 x,u16 y,u16 fc,u16 bc,u8 font,u16 num)
{
	u8 a,b;
	
	a =num/10;/* �������� */
	b =num%10;/* ����С�� */ 
	LCD_ShowNum(x, y, fc, bc, 2, font, a);				/* ��ʾ����   */
	LCD_ShowChar(x+(font/2)*2, y, fc, bc,font,0,'.');	/* ��ʾС���� */
	LCD_ShowNum(x+(font/2)*3, y, fc, bc, 1, font, b);	/* ��ʾС��   */
	//LCD_ShowChar(x+(font/2)*4, y, fc, bc,font,0,text);	/* ��ʾ����   */	
}

	/*
	* ʱ����ʾ����;�����е���rtc.c�е�RTC_Get_Time(&hour,&min,&sec,&ampm)�������õ�ʱ�䲢��ʾ���趨��λ��
	* x:	ˮƽ��㣻
	* y��	��ֱ��㣻
	* fc��	������ɫ��
	* bc��	�����ɫ��
	* font�������С��
	* on��	������ʾ�Ŀ��أ�on=0�����ڹرգ�on=1,���ڿ�����
	*/
void ui_Tim_Dispaly(u16 x,u16 y,u16 fc,u16 bc,char font,u8 on)
{
	u8 hour,min,sec,ampm;
	static u8 start =0;
	static u8 hour_0;
	static u8 min_0;
	static u8 sec_0;
	/* ��ȡRTCʱ�� */
	RTC_Get_Time(&hour,&min,&sec,&ampm);/* �˺�����rtc.c�ж��� */
	if(start ==0 || hour_0 != hour)
	{
		ui_TopBackground(x,y,12,12);
		LCD_ShowxNum(x, y, fc, bc, 2, font, 0x81, hour);	/* ��ʾСʱ */
		start =1;
		hour_0 =hour;
	}
	if(start ==0 || min_0!= min )
	{
		ui_TopBackground(x+font/2*2,y,18,12);
		if(on==1)
		{
			LCD_ShowChar(x+font/2*2, y, fc, bc,font,1,':');
		}
		else
			LCD_ShowChar(x+font/2*2, y, fc, bc,font,1,' ');
		LCD_ShowxNum(x+font/2*3, y, fc, bc, 2, font, 0x80, min);/* ��ʾ���� */
		start =1;
		min_0 = min;
	}
	if(start ==0 || sec_0 != sec )
	{
		ui_TopBackground(x+font/2*5,y,18,12);
		LCD_ShowChar(x+font/2*5, y, fc, bc, font,1,':');
		LCD_ShowxNum(x+font/2*6, y, fc, bc, 2, font, 0x81, sec);/* ��ʾ�� */	
		start =1;
		sec_0 =sec;
	}
}
/*
*	������ʾͼ��
*	
*/
void ui_Mute_icon(u16 x,u16 y,u16 bc,u8 spk)
{
	u16 x0,y0;
	if(ks.Spk_key==0)
	{
		ui_TopBackground(x-2,0,15,20);
		for(y0=0;y0<8;y0++)
		{
			for(x0=0;x0<3;x0+=1)Gui_DrawPoint(x+x0+1,y+y0+4,YELLOW);
		}
		for(y0=0;y0<16;y0+=1)
		{
			Gui_DrawPoint(x+7,y+y0,YELLOW);
		}
		Gui_DrawLine(x+4, y+4,x+7, y,YELLOW);
		Gui_DrawLine(x+4, y+11,x+7, y+15,YELLOW);
	}
	else
	{
		ui_TopBackground(x-2,0,15,20);
		 Gui_Circle(x+4,y+6,5,YELLOW) ;
		ui_TopBackground(x+2,y+9,5,3);
		
		for(y0=0;y0<5;y0++)
		{
			for(x0=0;x0<2;x0+=1)Gui_DrawPoint(x+x0+1,y+y0+8,YELLOW);
		}
		for(y0=0;y0<5;y0++)
		{
			for(x0=0;x0<2;x0+=1)Gui_DrawPoint(x+x0+6,y+y0+8,YELLOW);
		}
	}
	
	Gui_DrawLine(x+12, y+3,x+18, y+12,bc);
	Gui_DrawLine(x+12, y+12,x+18, y+3,bc);
}
/*
********************************************************************************
* ģʽ Ƶ�� �˲�������ʾ����
* X��㣺0��X�յ㣺319
* Y��㣺24;Y�յ㣺80��
********************************************************************************
*/
	/*
	* ���������x(0,59);y(24,80)Ϊ�๦����ʾ���ڣ�������������ģʽ����ǰƵ�ʵ�ģʽ��VFO/CH��ѡ���
	* �������ASCII 12;
	*/
/* ���ڹ���ģʽ��VFO��ʾ���ڵ�����ʹ�С�ĺ궨�� */
#define UI_MODE_VFO_X		1	/* mode X���*/
#define UI_MODE_VFO_Y		26	/* mode Y���*/
#define UI_MODE_VFO_W		46	/* mode W���*/
#define UI_MODE_VFO_H		28	/* mode H�߶�*/
//typedef struct
//{
//	char *Mode[6];/* ģʽ��ʾ�ı����� */
//	char *V_CH[2];/* VFO/CHת����ʾ�ı����� */
//	char *Vfo[2]; /* VFO A/Bת����ʾ�ı����� */
//	char *RT[2];  /* TX/RX�ı����� */
//}TWXT_tybedef;
const  TWXT_tybedef text =
{
	/* ģʽ��ʾ�ı����� */
	{
		" CW L "," CW U "," LSB  "," LSB  "," USB  "," USB  "," AM   "," FM   "
	},
	/* VFO/CHת����ʾ�ı����� */
	{
		" VFO  "," CH   ",
	},
	/* VFOת����ʾ�ı����� */
	{
		"A ","B ",
	},
	/* TX/RX�ı����� */
	{
		"T=R","T=R","RX","RX"
	}
};
   /* 
	* �๦�ܴ�����ʾ����
	* rt_key�� �շ�ת��
	* modeidx��ģʽmodeѡ��
	* vfo_reg;  
	* 		    λ0��VFO A/Bѡ��0��VFO_A,1: VFO_B;
	*		    λ1���ŵ�CH/VFOѡ��0��VFO��1��CH;
	*		    λ2����̨�Ĺ���ģʽ��0���շ�ģʽ��1��ֻ����ģʽ��
	*			λ3-λ5��������
	*			λ6��ͬƵ/��Ƶ�շ�ѡ��0���շ�ͬƵ��1���շ���Ƶ��
	*			λ7���շ�ת����0������RX��1������TX��
	* ch_num�� �ŵ���ֵ��
	*/
void ui_Vfo_Display(u8 ch_num)
{
	static u8 vfo_0;
	u16 fc;
	static u8 clk =0;
	static u8 rt_rx0;
	static u8 ab_0;
	static u8 tr_0;
	static u8 sim_dif0;
	static u8 ch_vfo0;
	static u8 ch_num0;
	//static u8 mode_0;
	if( ab_0 != ks.A_B || ch_vfo0 != ks.ch_vfo || ch_num0 != ch_num || tr_0 != TR_READ || sim_dif0 != ks.sim_dif || rt_rx0 != ks.rt_rx || clk ==0)
	{
		if(ks.rt_rx ==1 )/* ���ջ�ģʽ */
		{
			ui_TopBackground(12,4,16,16);
			fc =GRAY0;
		}
		else/* �շ��Ż�ģʽ */
		{
			ui_TopBackground(12,4,16,16);
			fc = RED;
		}
		
		if(TR_READ ==CONTROL_TX && ks.sim_dif ==1)/* ��Ƶ����TX */
		{
			vfo_0 = ~ks.A_B &0x01;/* ֻ����Ƶ����״̬VFO_A/VFO_B����CH_A/CH_B���� */
			
		}
		else/* ͬƵ���估����״̬ */
		{
			vfo_0 = ks.A_B &0x01;/* VFO_A/VFO_B����CH_A/CH_B������ */
		}		
		
		/* ��̨����ģʽ��ʾ */
		//LCD_RectangleWindow(2, 2, 36, 20, GRAY1);/* ���ڱ߿� */
		ui_TopBackground(12,4,24,16);
		LCD_ShowString(12, 4, fc, BLUE  , 16, 1,text.RT [ks.rt_rx<<1 | ks.sim_dif]);/* 0:��ʾ��RX��, 1:��ʾ��TX�� */
		if(ks.rt_rx==0 && ks.sim_dif ==1 )LCD_ShowString(20, 4, GRAY0 , BLUE  , 16, 1,"/");
		
		/* MODE��ʾ */
//		if(mode_0 != vfo[VFO_A].Mode)
//		{
//			LCD_RectangleWindow(1, 30, 52, 20, GRAY1);/* ���ڱ߿� */
//			
//			LCD_ShowString(3, 32, GRAY0, RED , 16, text.Mode [ vfo[VFO_A].Mode ] );
//			mode_0 = vfo[VFO_A].Mode;
//		}
		/* VFO��ʾ���ڵľ��α��� */
		LCD_RectangleWindow(1, 47, 52, 19, GRAY1);
		/* VFO/CH��ʾ */
		LCD_ShowString(3, 48,YELLOW, GRAY4  , 16, 0,text.V_CH [ks.ch_vfo]);
		
		//LCD_ShowString(35, 56, GRAY0, GRAY4 , 0,16, text.Vfo [ks.A_B]);
		if(ks.ch_vfo==0)
		{
			LCD_ShowString(35, 48, GRAY0 , GRAY4  , 16,1, text.A_B[vfo_0 ]);
		}
		else
		{
			LCD_ShowNum(35, 48, GRAY0, GRAY4 , 2, 16, ch_num-2);
		}
		//mode_0 = vfo[VFO_A].Mode;
		ab_0 = ks.A_B; 
		ch_vfo0 = ks.ch_vfo;
		ch_num0 = ch_num;
		tr_0 = TR_READ;
		sim_dif0 = ks.sim_dif;
		rt_rx0 = ks.rt_rx;
		clk =1;
		
	}
	/*
	*/
	/************************************************************************************************/
//		/* ��Ƶ��MODE/VFO��ʾ���ڵľ��α��� */
//		LCD_RectangleWindow(UI_MODE_VFO_X, UI_MODE_VFO_Y+37, UI_MODE_VFO_W, 16, GRAY2);
//		/* ��Ƶ��VFO��ʾ */
//		LCD_ShowString(UI_MODE_VFO_X+2, UI_MODE_VFO_Y+39,YELLOW, GRAY4 , 12, V_CH_text[(vfo_reg>>1) &0x01]);
//		if(((vfo_reg>>1) &0x01)==0)
//		{
//			LCD_ShowString(UI_MODE_VFO_X+26, UI_MODE_VFO_Y+39, GRAY1, GRAY4 , 12, Vfo_text[~vfo&0x01]);
//		}
//		else
//		{
//			LCD_ShowNum(UI_MODE_VFO_X+26, UI_MODE_VFO_Y+39, GRAY1, GRAY4 , 2, 12, ch_num);
//		}
	/* ��Ƶ��MODE��ʾ */
	//LCD_ShowString(UI_MODE_VFO_X+2, UI_MODE_VFO_Y+14+29, GRAY0, bc , 12, Mode_text[modeidx]);
}
/*
********************************************************************************
* �˲�����ʾ������Ƶ����ʾ�Ҳ�x(219,319)*y(24,60)����
* �������ɱ��ͼ���Լ������ı���ʾ��
********************************************************************************
*/
// typedef struct
//{
//	char *text;
//	u8 width;
//	u8 spe_width;
//}Filter_parameters;
const  Filter_parameters Fil_p[] =
{
	{0,0,0},
	{" 50Hz  ",	0,	2},/* 0 */
	{" 100Hz ",	2,	3},/* 1 */
	{" 200Hz ",	4,	4},/* 2 */
	{" 300Hz ",	6,	5},/* 3 */
	{" 500Hz ",	8,	6},/* 4 */
	{" 800Hz ",	10,	7},/* 5 */
	
	{" 1.5k  ",	12, 12},/* 6 */
	{" 1.8k  ",	14, 14},/* 7 */
	{" 2.1k  ",	16, 17},/* 8 */
	{" 2.4k  ",	18, 19},/* 9 */
	{" 2.7k  ",	20, 22},/* 10 */
	{" 3.0k  ",	22, 24},/* 11 */
	
	{" 6.0k  ",	24, 48},/* 12 */
	{" 9.0k  ",	26, 72},/* 13 */
	{" 5.0k  ",	28, 40},/* 14 */
	{"  10k  ",	30, 80},/* 15 */
	//{" 5.0k  ",	26},/* 14 */
	//{"  10k  ",	28},/* 15 */
};
	/*
	*	�˲���������ʾ����
	*	***��ں���***
	*	x y	 	�������
	*	fc 		������ɫ	Color
	*	bc		������ɫ
	*	k 		��ʾ��� width
	*	mode	ģʽ��ʾ�� -1���±ߴ�LSB,0: AM/FM,1���ϱߴ���
	*	****���ں��� ��****
	*/
void ui_Filter_Display(u16 x,u16 y,u16 fc,u16 bc,u8 idx,int16_t mode)
{
	
	int16_t  x0,m;
  	int16_t  y0;
	u8 k;

	k = Fil_p[idx].width ; 
	Lcd_Color(x-32,y,x+32,y+32,bc);//
	LCD_RectangleWindow(x-32, y, 64, 34, GRAY2);/* ���ڱ߿� */
	
	if(mode==-1)m=-k/2-5;//LSB
	else
	if(mode==0)m=0;	//AM&FM
	else 	
	if(mode==1)m=k/2+5;//USB
	for(y0=0;y0<12;y0+=1)
	{		
		for(x0=(-y0/4-k/2-1);x0<(k/2+y0/4+2);x0+=1)
		{	
			/*if(y0<1||x0<(-y0/4-k/2)||x0>(k/2+y0/4)) */Gui_DrawPoint(x+x0-m,y+y0+6,fc);
			//else			
			//	Gui_DrawPoint(x+x0,y+y0+4,bc);
		}
	}					
	for(x0=-32;x0<32;x0+=1)Gui_DrawPoint(x+x0,y+18,GRAY2);//ˮƽ����
	
	for(y0=0;y0<16;y0+=1)Gui_DrawPoint(x,y+y0+3, GRAY2);//��ֱ����
	
	LCD_ShowString(x-21,y+20,YELLOW,BLACK,12,0,Fil_p[idx].text);
	
 }
/*
 *******************************************************************************
 * Ƶ�״���ָʾ
 *******************************************************************************
*/
void ui_Spe_Filter_Display(u16 fil_idx,int8_t mode,u8 clk,u16 bc,u8 h_mode)
{
	u16 x0,x1;
	static u16 spe_k;
	static int8_t mode0;
	static u16 bfo;
	//static u8 clk0;
	u8 h;
	if(h_mode)h=182;
	else h=239;
	/* Ƶ�״�����ʾ */
	if(spe_k != Fil_p[fil_idx].spe_width || mode0 != mode || clk>0 || bfo != sd.Dsp_Bfo )
	{
		x0=sd.Dsp_Bfo*0.008f + Fil_p[fil_idx].spe_width/2;
		x1=sd.Dsp_Bfo*0.008f - Fil_p[fil_idx].spe_width/2;
		Lcd_Color(159-53,130,159+53,h,BLACK);
		
		if(mode==1)Lcd_Color(159-x0,130,159-x1,h,bc);//LSB
		else
		if(mode==0)Lcd_Color(159-Fil_p[fil_idx].spe_width/2,130,159+Fil_p[fil_idx].spe_width/2,h,bc);//AM&FM
		else 	
		if(mode==-1)Lcd_Color(159+x1,130,159+x0,h,bc);//USB
		spe_k = Fil_p[fil_idx].spe_width;
		mode0 = mode;
		bfo = sd.Dsp_Bfo;
		//clk0 = clk;
		//if(--clk<1)clk=0;
		//f0=f;
	}
}
/*
 *******************************************************************************
 * ��г������ʶָʾ
 *******************************************************************************
*/
void set_pos(u16 x,u16 y,u16 fc)
{
	 u8 x0,y0;
	for(y0=0;y0<7;y0+=1)
	{
		for(x0=y0;x0<7-y0;x0+=1)
		{
			Gui_DrawPoint(x+x0,y+y0,fc);
		}
	}
//	for(y0=0;y0<2;y0+=1)
//	{
//		for(x0=0;x0<16;x0+=1)
//		{
//			Gui_DrawPoint(x+x0,y+y0,fc);
//		}
//	}
}
/*
********************************************************************************
* ������ʾ
********************************************************************************
*/
/*
 * StepsDisplay struct
*/
 typedef struct
{
    uint16_t x;
     char *text;
}StepsDisplayGenInfo;
// StepsDisplay
const StepsDisplayGenInfo Steps_Info[] =
{
	{160,  "   1Hz"},
	{144, "  10Hz"},
	{128, " 100Hz"},
	{ 96, "  1KHz"},
	{ 80, " 10KHz"},
	{ 64, "100KHz"},
	{ 32, "   MHz"},
	{ 96, "   5KHz"},
	{ 0,  "  9KHz"}
	//{ 0,  "   MHz"}
};
	/*
	* ������ʾ����
	*/
void ui_Step_Display(u16 x,u16 y,int8_t idx)
{
	static u8 idx_0;
	//static u8 x0;
//	LCD_RectangleWindow(252, 62, 64, 16, GRAY2);/* ���ڱ߿� */
//	LCD_ShowString(254,64,YELLOW,GRAY5 ,12,0,"STEP");
//	LCD_ShowString(278,64,GRAY0 ,BLACK,12,0,Steps_Info[idx].text );
	
	if(idx >=0 && idx <7 )
	{
		set_pos( x+Steps_Info[idx_0].x, y, BLACK );
		set_pos( x+Steps_Info[idx].x, y, YELLOW );
		//set_pos( x+Steps_Info[idx_0].x, y, BLACK );
		idx_0 = idx;
	}
	
//	if(idx_0 != idx)
//	{
//		set_pos( x+Steps_Info[idx_0].x, y, BLACK );
//		idx_0 = idx;
//	}
}
/*
********************************************************************************
* RIT Ƶ��΢����ʾ
********************************************************************************
*/
void ui_RIT_Display(int16_t data )
{
	u16 fc,zi_fc;
	static u8 clk =0;
	static u8 rtt_key0;
	static int8_t data_0;
	static u8 rit_0;
	if(sd.rit_delay)sd.rit_delay--;
	if(sd.rit_delay<1){sd.rit_delay=0;ks.RIT_key =0;}
	if(data_0 != data || rtt_key0 != ks.Enc0_Use  || clk==0||rit_0!=ks.RIT_key)
	{
		if(ks.RIT_key ==1)
		{
			fc = RED;
			zi_fc = GRAY0;
		}
		else 
		{
			fc = GRAY2 ;
			if(data==0)zi_fc = GRAY2;
			else zi_fc = GRAY0;
		}
		//if(ks.RIT_key ==0)data =0;
		LCD_RectangleWindow(252, 61, 64, 14, fc);/* ���ڱ߿� */
		LCD_ShowString(254,62,zi_fc,GRAY5 ,12, 0," RIT ");
		if(data<0)
		{
			LCD_ShowString(284,62,zi_fc,BLACK ,12,0,"-");/* С��0��ʾ���� */
			data = 0-data;/* �������ֵ����0��ȥ�����Ľ��������������ľ���ֵ��*/
		}
		else 
			LCD_ShowString(284,62,zi_fc,BLACK ,12,0,"+"); /* ����0��ʾ���� */
		LCD_ShowNum(290, 62, zi_fc, BLACK , 4, 12, data);/* ��ʾ��ֵ */
		
		data_0 = data;
		rtt_key0 = ks.Enc0_Use;
		clk =1;
		rit_0 =ks.RIT_key;
	}
}
/*
********************************************************************************
* ��ǿ��ʾ��������ʾ פ����ʾ���� 
********************************************************************************
*/
/*
*
*/

typedef struct
{
	u16 tx_bc;
	u16 rx_bc;
	char *text;/* ��ʾ���ı� */
	//char *tx_text;/* ��ʾ���ı� */
}RxTx_Window_parameters;
const RxTx_Window_parameters RxTx_wp[] =
{
	{RED,BLACK," T ",},
	{BLACK,GREEN2, " R "}
};
void ui_Signal_scale(u16 x,u8 y,int16_t m,u16 fc)
{
	u8 x0,y0,i;
	if(m<0)m *=-1;
	if(m>160)m=160;
	
	for(x0=0;x0<4;x0+=1)
	{
		for(y0=0;y0<7;y0+=1)
		{
			for(i=0;i<165;i++)
			{
				if(m>i)Gui_DrawPoint(44+x+x0+i,16+y0+y,fc);
				else Gui_DrawPoint(48+x+x0+m,16+y0+y,BLACK );
			}
		}
		
	}
}
	/*
	* 	�ź���ʾ����
	* 
	*/
void ui_Signal_intensity_Display(u16 x,u16 y,u16 fc,u16 bc,u16 riss,u8 RT_key)
{
	static u8 sql_timer;
	static u8 start =0;
	static u8 start_ptt =0;
	static u8 RT_key_0;
	static float32_t pow_0;
	static float32_t riss_0;
	//static float32_t meon_pow;
	static u8 p;
	//static float32_t riss_1[32];
	u16 x0,y0,i;
	//u16 Signal_table;
	//u16 fwd[10],ref[10];
	
	if(start ==0)/* ��������ִֻ��һ�� */
	{
		/*��ǿǿ��ָʾ */	
		
		//LCD_ShowString(x+32,y,YELLOW,BLACK,16,0,"S");
		LCD_ShowString(x+55,y,YELLOW,BLACK,12,0,"1  3  5  7  9");
			
		LCD_ShowString(149+x,y,RED,BLACK,12,0,"20  40 60dB");	
		/*S ��ǿ��̶��� */
		for(i=0;i<81;i+=9)	//RX�̶���S0��S9
		{
			for(y0=0;y0<2;y0+=1)
			{
				for(x0=0;x0<2;x0+=1)
				{
					Gui_DrawPoint(48+x+x0+i,12+y0+y,YELLOW);
				}
			}
		}
		for(i=81;i<155;i+=12)	//RX�̶���S9��S9+60db
		{
			for(y0=0;y0<2;y0+=1)
			{
				for(x0=0;x0<2;x0+=1)
				{
					Gui_DrawPoint(48+x0+i+x,12+y0+y,YELLOW);
				}
			}
		}
		/*S ��ǿ��̬��ʾ���±߿��� */	
		for(y0=0;y0<11;y0+=10)	
		{
			for(x0=0;x0<165;x0+=1)
			{
				Gui_DrawPoint(x0+48+x,y0+14+y,GRAY0);	
			}
		}
		/*���书�ʿ̶��� */
		for(i=0;i<155;i+=15)	//TX�̶���
		{
			for(y0=0;y0<2;y0+=1)
			{
				for(x0=0;x0<2;x0+=1)
				{
					Gui_DrawPoint(48+x0+i+x,25+y0+y,YELLOW);
				}
			}
		}
		//LCD_ShowString(x+32,23+y,YELLOW,BLACK,16,0,"Po");
		LCD_ShowString(62+x,27+y,YELLOW,BLACK,12,0,"2");
		LCD_ShowString(116+x,27+y,YELLOW,BLACK,12,0,"10");//���书�ʿ̶�ָʾ
		LCD_ShowString(191+x,27+y,YELLOW,BLACK,12,0,"20w");//���书�ʿ̶�ָʾ
		
		//ads.cw_alc_gain[vfo[VFO_A].Band_id] = AT24CXX_ReadLenByte(ADDR_CW_POW_GAIN(vfo[VFO_A].Band_id),2);//CW��������ϵ����ȡ
		
		start =1;/* ִ����һ�������������ظ�ִ�У� */
	}
	
	/* �շ�״̬��ʾ */
	if(start_ptt <1 || RT_key_0 != RT_key)
	{
		
		/* �շ�״̬��ʾ */
		LCD_RectangleWindow(x+21, y, 26, 18, RxTx_wp[RT_key].rx_bc);/* ���ڱ߿� */
		LCD_RectangleWindow(x+21, y+22, 26, 18, RxTx_wp[RT_key].tx_bc);/* ���ڱ߿� */
		//LCD_ShowString(x-29, y,  GRAY0,RxTx_wp[RT_key].bc , 16,0, RxTx_wp[RT_key].text );
		//LCD_ShowString(x, y, GRAY0,RxTx_wp[RT_key].tx_bc , 16,0, RxTx_wp[RT_key].text );
		LCD_ShowString(x+22,y+1,YELLOW,RxTx_wp[RT_key].rx_bc,16,0," S ");
		LCD_ShowString(x+22,23+y,YELLOW,RxTx_wp[RT_key].tx_bc,16,0," Po");
		/* TXפ��SWR��ʾ�������� */
		//Lcd_Color(252,98,319,180,BLACK);
		if(RT_key ==CONTROL_TX)
		{
//			if( RT_key_0 != RT_key)
//			{ 
//				start_ptt =0;
//				RT_key_0 = RT_key;
//			}
			//LCD_RectangleWindow(x-1, y-1, 26, 18, BLACK);/* ���ڱ߿� */
			//LCD_RectangleWindow(x-1, y-1, 26, 18, RED);/* ���ڱ߿� */
			Lcd_Color(160,132,160,180,BLACK);
			Lcd_Color(5,130,70,180,BLACK);
			LCD_RectangleWindow(5, 131, 64, 20, RED);/* ���ڱ߿� */
			LCD_ShowString(55,133,YELLOW,GRAY5 ,16, 1, "W");
			LCD_RectangleWindow(5, 153, 64, 20, RED);/* ���ڱ߿� */
			LCD_ShowString(9,155,YELLOW ,BLACK , 16, 0, "SWR "); /* ����0��ʾ���� */
				
			//start_ptt++;
		}
		else 
		{
			Lcd_Color(160,132,160,180,BLACK);
			Lcd_Color(5,130,70,180,BLACK);
		}
		if( RT_key_0 != RT_key)
		{ 
			start_ptt =0;
			RT_key_0 = RT_key;
		}
		//RT_key_0 = RT_key;
		//start_ptt =1;
		if( RT_key_0 == RT_key)
		{ 
			start_ptt =1;
		}
//		if( start_ptt>2)start_ptt =2;
	}
	
	/* TXפ��SWR��ʾ */
	if(RT_key ==CONTROL_TX)
	{
//		for(i=0;i<5;i+=1)	//TX�̶���
//		{
//			ads.fwd+=Get_Adc1(ADC_CH11);/* ������ */
//			ads.ref+= Get_Adc1(ADC_CH10);/* ������ */
//		}
		//ads.fwd = Get_Adc1(ADC_CH11);/* ������ */
		//ads.ref = Get_Adc1(ADC_CH10);/* ������ */
		//arm_mean_f32((float32_t *)fwd ,10, (float32_t *)&ads.fwd );
		//arm_mean_f32((float32_t *)ref ,10, (float32_t *)&ads.ref );
		//ads.fwd/=5;
		//ads.ref/=5;
		/*
		*	P=V*V/R
		*	���ʼ��㹫ʽ pow = ((fwd*2.5f/65535) * (fwd*2.5f/65535))/50*100;
		*	��ʽĩβ֮���Գ���100����Ϊ���ʼ�ⲿ�ֵ�˥����20db;
		*	���ǰ����򻯣�����֪�������м��㣻���ѽ���Ŵ�10����
		*/
		//ads.pow = ads.fwd * 0.00038f * ads.fwd *0.00038f * 0.02f;/* 16λADC */
		//ads.pow = ads.fwd * 0.012207f * ads.fwd *0.012207f * 0.0975f;/* 12λADC */
		//if(vfo[VFO_A].Mode >= DEMOD_LSB )ads.pow = ads.fwd * 0.012207f * ads.fwd *0.012207f * 0.15f;/* 12λADC */
		//ads.pow = ads.fwd * 0.0118f * ads.fwd *0.0118f * 0.2f;/* 12λADC */
		//ads.pow = ads.fwd * 0.00078f * ads.fwd *0.00078f * 0.2f;/* 16λADC */
		//if(ads.pow <0)ads.pow*=-1;
		
		//if(ads.pow>1)ads.swr = (ads.fwd*ads.fwd + ads.ref*ads.ref*14.0f)*10
		//						/(ads.fwd*ads.fwd - ads.ref*ads.ref*14.0f);/* פ������ */
		
		//LCD_RectangleWindow(252, 98, 64, 20, RED);/* ���ڱ߿� */
		//LCD_ShowString(256,100,YELLOW,GRAY5 ,16,"SWR");
		if(ads.swr<20)
		{	
			ui_Decimal_Dispaly(34,155,GRAY0,BLACK,16, ads.swr);/* פ��С��2��ֵ��ʾ��ɫΪ��ɫ */
			//sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
		}
		else
		if(ads.swr>=20 && ads.swr <30)
		{
			ui_Decimal_Dispaly(34,155,GRAY0,YELLOW2 ,16, ads.swr);/* פ������2��С��3��ֵ��ɫΪ��ɫ */
			//if(sd.Pow >100)sd.Pow =100;
		}
		else
		if(ads.swr>=30)
		{
			//if(sd.Pow >100)sd.Pow =100;
			//if(ads.swr>=99)ads.swr=99;
			ui_Decimal_Dispaly(34,155,GRAY0,RED ,16, ads.swr);/* פ������3��ֵ��ɫΪ��ɫ */
		}
		if(++p>5)p=0;
		
		if(ads.pow > pow_0)pow_0 =ads.pow;
		if(ads.pow <= pow_0)
		{
			if(ads.pow >=3)
			{
				if(ads.pow < pow_0-3)pow_0 -=pow_0*0.1f;
			}
			else pow_0 -=pow_0*0.1f;
		}
		ads.Signal_table = (u8)(pow_0 * 0.75f);
			
		if(p==0)ui_Decimal_Dispaly(9,133,GRAY0,BLACK  , 16, (u16)pow_0);/* ��ʾ��ֵ */
		//if(sd.menu_page ==1)sd.Pow =50;
		if(sd.Pow ==50&&vfo[VFO_A].Mode <  DEMOD_LSB)
		{			
			if(ads.pow>=49&&ads.pow<=50)AT24CXX_WriteLenByte(vfo[VFO_A].Band_id*2 + ADDRPOWER_GAIN , ads.pow_gain [vfo[VFO_A].Band_id ],2);
			if(sd.menu_page ==1)LCD_ShowNum(160,132,GRAY1,BLACK, 4,16,ads.pow_gain [vfo[VFO_A].Band_id ]);
		}
	}
	else
	{
		
		//ads.amp_adjust_num = (u8)((riss -78)*0.2f);
		//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
		riss_0 = riss ;
//		if(riss > riss_0)riss_0 = riss ;
//		if( vfo[VFO_A].Mode >= DEMOD_LSB && vfo[VFO_A].Mode <= DEMOD_USB_DIG)
//		{
//			riss_0 -= 20;
//		}
		
//		//if(riss < (u16)riss_0)
//		{
//			riss_0 -= riss_0 * 0.01f;
//		}
		
		//riss_0 = riss;
		
		if((u16)riss_0 <=54)ads.Signal_table = (u16)(riss_0*1.5f);
		else
		if((u16)riss_0 >54 )ads.Signal_table = (u16)(81 + (riss_0 -54)*1.2f );
		
		/* ������� */
		if(sd.Sql>1 || ads.tx_delay >10)
		{
			if(riss>sd.Sql+1)
			{
				ads.Af_mute =1;/* ��Ƶ���� */
				ui_TopBackground(232,4,8,16);
				sql_timer =50;
			}
			else
			if(riss<sd.Sql )
			{
				if(ks.Enc2_Use ==1||vfo[VFO_A].Mode > DEMOD_USB_DIG)
				{
					ads.Af_mute =0;/* ��Ƶ�ر� */	
					ui_Mute_icon(220,4,RED,ks.Spk_key);
				}
				else
				{
					sql_timer--;
					if(sql_timer<1)
					{
						sql_timer =0;
						ads.Af_mute =0;/* ��Ƶ�ر� */	
						ui_Mute_icon(220,4,RED,ks.Spk_key);
					}
				}
			}
		}
		else
		{
			ads.Af_mute =1;/* ��Ƶ���� */
			ui_TopBackground(232,4,8,16);
		}
	}
	
	/* RX�ź�ǿ�Ⱥ�TX���书�ʶ�̬��ʾ */
	if(ads.Signal_table >155)ads.Signal_table =155;/* �޶����ֵ�����ⳬ����ʾ��Χ */
	
	for(y0=0;y0<7;y0+=1)/* ��λ��̬��ʾ���ĸ߶� 7������ */
	{
		if(RT_key ==CONTROL_RX)
		{
			for(i=0;i<155;i+=1)/* ��λ��̬��ʾ����� 3������ */					
			{
				if(i<ads.Signal_table)
				{
					Gui_DrawPoint(x+48+0+i,y+16+y0,fc);/* ��ɫ��ʾ */
				}
				else if(i>ads.Signal_table)
					Gui_DrawPoint(x+48+0+i,y+16+y0,bc);/* ���� */
			}
		}
		else
		{
			for(i=0;i<155;i+=1)/* ��λ��̬��ʾ����� 3������ */					
			{
				if(i<ads.Signal_table)
				{
					Gui_DrawPoint(x+48+i,y+16+y0,RED);/* ����S9��ʾ��ɫ */
				}
				else if(i>ads.Signal_table)
					Gui_DrawPoint(x+48+i,y+16+y0,bc);/* ���� */
			}
		}
	}
	
//	if(pow_1<ads.Signal_table)
//	{
//		pow_1=ads.Signal_table;
//	}
//	if(++k ==11) k =0;
//	if(pow_1>=ads.Signal_table)
//	{
//		
//		if(k>5)pow_1 -=pow_1*0.1f;
//		
//	}
//	if(RT_key ==CONTROL_RX)ui_Signal_scale(x, y,pow_1,GREEN);
//	else ui_Signal_scale(x, y,pow_1,RED);
}
/*
********************************************************************************
* ������ʾ����
********************************************************************************
*/
/* ������ʾ��Ϣ�ṹ�� */
typedef struct
{
	u16 x;/* ���X*/
	u16 y;/* ���Y*/
	//u8 font;
	char *text;/* ��ʾ���ı� */
}UI_BandDisplay_Information,UI_RxBandDisplay_Information;
/* �շ�����ģʽ������ʾ����Ϣ */
const  UI_BandDisplay_Information BandDispaly_path[] =
{
	//{0,0,0,},
	/* ��һ�� */
	{ 50,	130,	" 80m "  },
	{120,	130,	" 60m "  },
	{190,	130,	" 40m "  },
	/* �ڶ��� */
	{ 50,	158,	" 30m "  },
	{120,	158,	" 20m "  },
	{190,	158,	" 17m "  },
	/* ������ */
	{ 50,	186,	" 15m "  },
	{120,	186,	" 12m "  },
	{190,	186,	" 10m "  },
	//{ 90,	186,	"  6m "  }
	//{160,100,24," 40m "  },
	//{230,100,24," 40m "  },
};
/* ���ջ�ģʽ������ʾ���� */
const  UI_RxBandDisplay_Information Rx_BandDispaly_path[] =
{
	//{0,0,0,},
	/* ��һ�� */
	{ 15,	130,	"  MW "  },
	{ 65,	130,	"120M "  },
	{115,	130,	" 90m "  },
	{165,	130,	" 75m "  },
	{215,	130,	" 60m "  },
	{265,	130,	" 49m "  },
	/* �ڶ��� */
	{ 15,	150,	" 41m "  },
	{ 65,	150,	" 31m "  },
	{115,	150,	" 25m "  },
	{165,	150,	" 22m "  },
	{215,	150,	" 19m "  },
	{265,	150,	" 16m "  },
	/* ������ */
	{ 15,	170,	" 15m "  },
	{ 65,	170,	" 13m "  },
	{115,	170,	" 11m "  },
	{165,	170,	"  6m "  },
	{215,	170,	"  2m "  },
	{265,	170,	" 2m5 "  }
};
	/*
	* �շ��Ż��Ĳ�����ʾ
	* mode: ����ģʽ��0���շ�ģʽ������ʾ��1�����ջ�ģʽ������ʾ��
	* font: ASCII�ַ����壻
	* band_idx: ����ѡ����Ϣ��
	*/
void ui_BandDisplay(u8 mode,u8 band_idx)
{
	u8 i;
	u16 fc;
	
	//Lcd_Color(0,130,319,210,BLACK);/* ��ʾ�������� */
	
	if(mode ==0)/* �շ�ģʽ������ʾ */
	{
		Lcd_Color(0,130,319,210,BLACK);/* ��ʾ�������� */
		for(i=0;i<9;i++)
		{
			if(i &band_idx)fc =RED;/* ѡ�еĲ�������Ϊ��ɫ */
			else
				fc =GRAY0;
			LCD_ShowString( BandDispaly_path[i].x ,  BandDispaly_path[i].y , fc, GRAY4 , 24, 0, BandDispaly_path[i].text );
			//LCD_RectangleWindow(u8 x, u8 y, u8 w, u8 h, u16 fc);
		}
	}
	//else
	if(mode ==1)/* �շ�ģʽ������ʾ */
	{
		Lcd_Color(0,130,319,210,BLACK);/* ��ʾ�������� */
		for(i=0;i<18;i++)/*���ջ�ģʽ������ʾ */
		{
			if(i &band_idx)fc =RED;/* ѡ�еĲ�������Ϊ��ɫ */
			else
				fc =GRAY0;
			LCD_ShowString( Rx_BandDispaly_path[i].x , Rx_BandDispaly_path[i].y , fc, GRAY4 , 16,0, Rx_BandDispaly_path[i].text );
			//LCD_RectangleWindow(u8 x, u8 y, u8 w, u8 h, u16 fc);
		}
	}
}
///*
//********************************************************************************
//* Ƶ�׼��ٲ�ͼ��ʾ
//********************************************************************************
//*/
//typedef struct
//{
//	u16 x;/* ���X*/
//	u16 y;/* ���Y*/
//	//u8 font;
//	char *text;/* ��ʾ���ı� */
//}SPE_Text_typedef;
//const SPE_Text_typedef	Spe_text[] =
//{
//	{  10,222,"5"},
//	{  40,222,"4"},
//	{  70,222,"3"},
//	{ 100,222,"2"},
//	{ 130,222,"1"},
//	{ 160,222,"0"},
//	{ 190,222,"1"},
//	{ 220,222,"2"},
//	{ 250,222,"3"},
//	{ 280,222,"4"},
//	{ 310,222,"5"},
//};
/*
* Ƶ����ʾ������
* ʵ��Ƶ���Լ��ٲ�ͼ��ʾ��
* ��ڲ�����
* x yΪ��ʼ���꣬����y�ǵײ���������������ʾ���߶���spe_h+falls_h��
* width����ȣ�
* spe_h: Ƶ�׸߶ȣ�
* falls_h���ٲ�ͼ�߶ȣ�
* mode: ������
*/
//float32_t FFT_in[FFT_LENGTH*2];
/*__attribute__((section (".RAM_D1")))*/ //float32_t input[512];
//void ui_Spectrum_Dispaly(u16 x,u16 y,u16 width,u16 spe_h,u16 falls_h)
//{
//	static u16 clk =0;
//	 u16 x0,y0,fc;
//	static u16 k;
//	static float32_t input[512];
//	static  uint16_t  falls[20][320];
//	static u16 h;
//	 u16 i;
//	static float32_t FFT_in[FFT_LENGTH*2];
//	//static float32_t FFT_out[FFT_LENGTH];
//	if(clk ==0)
//	{
//		/* */
//		for(x0=0;x0<width;x0++)
//		{
//			Gui_DrawPoint(x0+x,y0+y,GRAY1);
//		}
//		for(x0=10;x0<width;x0+=30)
//		{
//			for(y0=0;y0<2;y0+=1)
//				Gui_DrawPoint(x0+x,y0+y+1,YELLOW );
//		}
//		for(i=0;i<11;i++)
//		{
//			LCD_ShowString(Spe_text[i].x-2 ,Spe_text[i].y ,YELLOW ,BLACK , 12, 0, Spe_text[i].text ); /* ����0��ʾ���� */
//		}
//		clk =1;
//	}
//	if(spe.fft_flang ==1)
//	{			
//		for(i=0;i<FFT_LENGTH;i++)
//		{		
//			 FFT_in[i*2] = spe.fft_buf [i];
//			 FFT_in[i*2+1] =0;
//		}
//		//arm_cfft_f32(&arm_cfft_sR_f32_len1024, spe.spe_buf, 0, 1);
//		arm_rfft_fast_f32(&Spe_FFT ,  FFT_in,   FFT_in,0 );	//FFT���㣨��4��
//		/***ȡ��FFTģֵ***/
//		arm_cmplx_mag_f32(  FFT_in, FFT_in,512);
//			
//		for(k=0;k<width;k++)
//		{
//			/* �ɲ������ */
//			
//			for(i=input[k+1];i>0;i--)
//			//{
//				Gui_DrawPoint(k+1+x,y-i-20,BLACK);	
//			//}
//			/* �²���������ȡ */
//			input[k] = FFT_in[k-1]*0.025 +  FFT_in[k]*0.05 +  FFT_in[k+1]*0.025;
//			falls[h][k] = (u16)input[k];
//			//input[k]=log2(input[k]);
//			arm_sqrt_f32( input[k]*0.0015625f,&input[k]);
//			//input[k]*=0.015625f;
//			/* ���θ߶����� */
//			//input[k] = FFT_out[k];
//			if(input[k]>falls_h)input[k]  = 60;
//			
//			if(input[k]<falls_h)input[k] = (u16)input[k]&0x2f;
//			
//			/* ���Ʋ��� */	
//			if(k>0)
//			{
//				/* ���߱�ʶ��ʾ */
//				if(k==width*0.5f)fc=YELLOW;
//				else
//				fc=BLUE;
//				/* Ƶ����ʾ */
//				
//				for(i = input[k];i>0;i--)
//				{
//					Gui_DrawPoint(k,y-i-20,fc);
//					
//				}
//				
//				/* �ٲ�ͼ��ʾ */
//				for(i=0;i<falls_h;i++)
//				{
//					if(i<=h)
//					Gui_DrawPoint(k+x,y+i-20, (falls[h-i][k]<<2));
//					if(i>h)
//					Gui_DrawPoint(k+x,y+i-20, (falls[20+h-i][k]<<2));
//				}
//			}
//			/* �ٲ�ͼ����ʱ�� */
//			if(k==0)
//			{
//				if(++h >falls_h-1)h =0;
//			}
//		}
//		spe.fft_flang =0;
//	}
//}
/*
********************************************************************************
*	Ƶ����ʾ��δʹ�ã�
********************************************************************************
*/
void ui_Spectrum_Dispaly_2(u16 x,u16 y,u16 width,u8 spe_h,u8 falls_h,u16 *pSrc)
{
	u16 i;
	static u8 h;
	static u16 k;
	static u16 *Pin;
	static u16 **fall;
	
	h++;if(h >19)h=0;
	for(k=0;k<width;k++)
	{
		//*Pin =*pSrc;
		for(i=*(Pin+k+1);i>0;i--)
		{
			Gui_DrawPoint(k+1+x,y-i-20,BLACK);
		}
		*(Pin+k) =(*(pSrc+k-1)>>2)+((*pSrc+k)>>1)+(*(pSrc+k+1)>>2);
		
		*(*(fall+k)+h) = *(Pin+k);
		
		if(*(Pin+k) >spe_h)*(Pin+k) = spe_h;
		if(k>0)
		{
			for(i = *(Pin+k);i>0;i--)
			{
				Gui_DrawPoint(k,y-i-20,GRAY0);	
			}
		}
		for(i=0;i<falls_h;i++)
		{
			if(i<=h)
			Gui_DrawPoint(k+x,y+i-19, *(*(fall+k)+h-i));
			if(i>h)
			Gui_DrawPoint(k+x,y+i-19, *(*(fall+k)+h-i+20));
		}
		//Pin ++;
		//pSrc ++;
		
	}		
}
/*
********************************************************************************
	�˵���ʾ����
********************************************************************************
*/
/*
*	x/y: ���
*	h��  �߶�
*	fc: ������ɫ
*	bc: ������ɫ
*/
void ui_Meun_Window(u16 x, u16 y,u16 h, u16 fc, u16 bc)
{
	u16 x0,y0;
	Lcd_Color(x, y,319,h,bc) ;
	for(y0=0;y0<81;y0+=80)
	{
		for(x0=0;x0<280;x0+=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
	for(x0=0;x0<280;x0+=279)
	{
		for(y0=0;y0<81;y0+=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
}
/*
********************************************************************************
*	���ܣ���ʾ���������������Ĳ���ѡ������ 
********************************************************************************
*/
/*
*	����в���ʾ�������߹�����ʾ����
*/
const  SideFeatureText_tybedef Function_L_text[]=
{
	{"VOL",	2,	 90},
	{"SQL",	2,	107},
};
//
void ui_SideFunction_Display()
{
	u8 i;
	u16 fc,fc0;
	static u8 clk;
	//static u8 enc2_key0;
	//static u8 enc2_long0;
	static u8 vol0;
	static u8 spl0;
	static u8 enc2_use0;
	if(enc2_use0 != ks.Enc2_Use)
	{
		enc2_use0 = ks.Enc2_Use;
		clk=0;
	}
	if(clk ==0 || vol0!=sd.Vol || spl0 !=sd.Sql)
	{
		for(i=0;i<2;i++)
		{
			if(i==ks.Enc2_Use && ks.EC1_12 ==0)
			{
				fc =RED;
				fc0 =GRAY0;
			}
			else 
			{
				fc =GRAY2;
				fc0 = GRAY2;
			}				
			LCD_RectangleWindow(Function_L_text[i].x, Function_L_text[i].y, 46, 14, fc);/* ���ڱ߿� */
			LCD_ShowString(Function_L_text[i].x+6,Function_L_text[i].y+1,fc0 ,BLACK , 12, 1, Function_L_text[i].name );
		}
		if(vol0!=sd.Vol || clk ==0)LCD_ShowNum(Function_L_text[0].x+30,Function_L_text[0].y+1,GRAY1,BLACK, 2,12,sd.Vol);
		if(spl0!=sd.Sql || clk ==0)LCD_ShowNum(Function_L_text[1].x+30,Function_L_text[1].y+1,GRAY1,BLACK, 2,12,sd.Sql);
		
		vol0=sd.Vol;
		spl0=sd.Sql;
		clk =1;
	}		
}
/*
*********************************************************************************************
	
*********************************************************************************************
*/
/*
*	dit/dah���ַ���ʾ����
*/
void cw_sign_dit_dah(u16 x,u16 y,u16 fc,u8 mode)
{
	u8 x0,y0;
	if(mode >0)
	{
		for(x0=0;x0<6;x0++)//dah
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
		for(x0=0;x0<2;x0++)//dit
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+10+x,y0+y,fc);
		}
	}
	else
	{
		for(x0=0;x0<2;x0++)//dah
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
		for(x0=0;x0<6;x0++)//dit
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+6+x,y0+y,fc);
		}
	}
}
/*
*	�Ҳ����ʾ��
*/
//

const  SideFeatureText_tybedef  KeySwitch_MA_text[]=
{
	{"CW A",	3,	71},
	{"CW M",	3,	71},
	{"E",		43,	71}
	//{" - .",	278,	77},
	//{" . -",	278,	77},
};
//
const  SideFeatureText_tybedef  DSP_noise_text[]=
{
	{"NR",	6,	71},
	{"NB",	33,	71}
};
//
const  SideFeatureText_tybedef Function_CW_text[]=
{
	{"SIVOL",	254,	78},
	{"SITON",	254,	93},
	{"KEYSP",	254,	108},
};
//
const  SideFeatureText_tybedef Function_SSB_text[]=
{
	{" MIC ",	254,	78},
	{" DSP ",	254,	93},
	{" LEN ",	254,	108}
};
//
void ui_SideFunction_R_Display(u8 rt_key)
{
	int8_t  i;
	u16 fc,fc0;
	//static u8 rtkey0;
	static u8 vfo_a_mode0;
	static u8 clk;
	static u8 menukey0;
	//static u8 f_key0;
	static u8 enc2_key0;
	static u8 enc2_long0;
	static u8 mode0;
	static u8 nr_key0;
	static u8 not_key0;
	static u8 cw_keysw0;
	static u8 cw_Exercise0;
	static u8 cw_ab0;
	static u16 Menu_Values0[3];
	static u16 Menu_Values1[3]; 
	static u16 cw_Values[3]; 
	static u16 nr_Values[3];
	
	//if(rt_key !=CONTROL_RX)clk =0;
	nr_Values[0] = sd.mic_gain;
	nr_Values[1] = nr.dsp_nr_strength;
	nr_Values[2] = nr.dsp_nr_delaybuf_len;
	
	cw_Values[0] = sd.CW_vol;
	cw_Values[1] = sd.CW_Sidetone;
	cw_Values[2] = sd.CW_Speed ;
	for(i=0;i<3;i++)
	{
		if(Menu_Values0[i] != cw_Values[i] || Menu_Values1[i] != nr_Values[i])enc2_key0 = ~ks.EC1_12 *0x01;
	}
	if(menukey0 != ks.FunctionWindow_Use )enc2_long0 = ~ks.EC1_12_LONG &0x01;
	
	if(rt_key ==CONTROL_RX )//&& ks.FunctionWindow_Use ==0)//�๦�ܴ����ѽ��ռ��)
	{
		if(clk ==0 || enc2_key0 != ks.EC1_12 || enc2_long0 != ks.EC1_12_LONG || mode0 != vfo[VFO_A].Mode || nr_key0 != ks.NR_key 
			|| not_key0 != ks.NOT_key || cw_keysw0 != ks.CW_keySW || cw_Exercise0 != ks.CW_Exercise || cw_ab0 != ks.CW_AB )
		{
			if(clk ==0|| vfo_a_mode0!=vfo[VFO_A].Mode)
			{
				Lcd_Color(252, 77,319,127,BLACK ) ;/* ��ʾ�������� */
				clk =1;
				
				vfo_a_mode0=vfo[VFO_A].Mode;
			}
			if(vfo[VFO_A].Mode < DEMOD_LSB )
			{
				Lcd_Color(0, 68,54,86,BLACK ) ;/* ��ʾ�������� */
				//LCD_ShowString(KeySwitch_MA_text[ks.CW_AB+3].x+1,KeySwitch_MA_text[ks.CW_AB+3].y,GRAY0 ,BLACK , 12, 0, KeySwitch_MA_text[ks.CW_AB+3].name );
				cw_sign_dit_dah(30,77,YELLOW,ks.CW_AB);
				if(ks.CW_Exercise ==1)
				{
					fc =RED;
					fc0 =GRAY0;
					//fc1 =YELLOW;
				}
				else 
				if(ks.CW_Exercise ==0)	
				{
					fc =GRAY2;
					fc0 = GRAY2;
					//fc1 = GRAY2;
				}
				if(sd.menu_page==1)ks.CW_keySW =1;
				//cw_sign_dit_dah(285,105,fc1,ks.CW_AB);
				LCD_RectangleWindow(KeySwitch_MA_text[0].x-2, KeySwitch_MA_text[2].y-1, 52, 14, RED);/* ���ڱ߿� */
				//LCD_ShowString(KeySwitch_MA_text[i].x+1,KeySwitch_MA_text[i].y,BLACK ,BLACK , 12, 0, "   ");
				LCD_ShowString(KeySwitch_MA_text[2].x+1,KeySwitch_MA_text[2].y,fc0 ,BLACK , 12, 0, KeySwitch_MA_text[2].name );
				//LCD_RectangleWindow(KeySwitch_MA_text[ks.CW_keySW].x-2, KeySwitch_MA_text[ks.CW_keySW].y-1, 31, 14, RED);/* ���ڱ߿� */
				//LCD_ShowString(KeySwitch_MA_text[i].x+1,KeySwitch_MA_text[i].y,BLACK ,BLACK , 12, 0, "   ");
				LCD_ShowString(KeySwitch_MA_text[ks.CW_keySW].x+1,KeySwitch_MA_text[ks.CW_keySW].y,GRAY0 ,BLACK , 12, 0, KeySwitch_MA_text[ks.CW_keySW].name );
				
			}
			else
			{
				Lcd_Color(0, 68,54,86,BLACK ) ;/* ��ʾ�������� */
				for(i=0;i<2;i++)
				{
					if(i==0 && ks.NR_key ==1)
					{
						fc =RED;
						fc0 =GRAY0;
					}
					else 
					if(i==0 && ks.NR_key ==0)	
					{
						fc =GRAY2;
						fc0 = GRAY2;
					}
					//
					if(i==1 && ks.NOT_key ==1)
					{
						fc =RED;
						fc0 =GRAY0;
					}
					else 
					if(i==1 && ks.NOT_key ==0)	
					{
						fc =GRAY2;
						fc0 = GRAY2;
					}
					LCD_RectangleWindow(DSP_noise_text[i].x-5, DSP_noise_text[i].y-1, 25, 14, fc);/* ���ڱ߿� */
					//LCD_ShowString(DSP_noise_text[i].x+1,DSP_noise_text[i].y,BLACK ,BLACK , 12, 0, "   ");
					LCD_ShowString(DSP_noise_text[i].x+1,DSP_noise_text[i].y,fc0 ,BLACK , 12, 0, DSP_noise_text[i].name );
				}
			}
			//
			for(i=0;i<3;i++)
			{
				if(i==ks.EC1_12-1 && ks.EC1_12_LONG ==0)
				{
					fc =RED;
					fc0 =GRAY0;
				}
				else 
				if(i!=ks.EC1_12-1 && ks.EC1_12_LONG ==0)	
				{
					fc =GRAY2;
					fc0 = GRAY2;
				}
				//if(i<2)
				//{
				//	LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 31, 19, fc);/* ���ڱ߿� */
				//}
				//else 
				//LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* ���ڱ߿� */
				
				if(vfo[VFO_A].Mode < DEMOD_LSB)
				{
					LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* ���ڱ߿� */
					LCD_ShowString(Function_CW_text[2-i].x+2,Function_CW_text[2-i].y,fc0 ,BLACK , 12, 0, Function_CW_text[2-i].name );
					LCD_ShowNum(Function_CW_text[2-i].x+42,Function_CW_text[2-i].y,fc0 ,BLACK , 3,12,cw_Values[2-i]);
				}
				else
				{
					LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 64, 14, fc);/* ���ڱ߿� */
					LCD_ShowString(Function_SSB_text[i].x+2,Function_SSB_text[i].y,fc0 ,BLACK , 12, 0, Function_SSB_text[i].name );
					LCD_ShowNum(Function_SSB_text[i].x+42,Function_SSB_text[i].y,fc0 ,BLACK , 3,12,nr_Values[i]);
				}
			}
			//
			enc2_key0 = ks.EC1_12; 
			enc2_long0 = ks.EC1_12_LONG;
			mode0 = vfo[VFO_A].Mode;
			nr_key0 = ks.NR_key;
			not_key0 = ks.NOT_key;
			cw_keysw0 = ks.CW_keySW;
			menukey0 = ks.FunctionWindow_Use;
			cw_Exercise0 = ks.CW_Exercise;
			cw_ab0 = ks.CW_AB;
			//clk =1;
			for(i=0;i<3;i++)
			{
				Menu_Values0[i] = cw_Values[i];
				Menu_Values1[i] = nr_Values[i];
			}
		}
//		/**/
//		if( spe.fft_max >39)
//		{
//			for(i=0;i<3;i++)
//			{
//				if(i==ks.EC1_12-1 && ks.EC1_12_LONG ==0)
//				{
//					fc =RED;
//					fc0 =GRAY0;
//				}
//				else 
//				if(i!=ks.EC1_12-1 && ks.EC1_12_LONG ==0)	
//				{
//					fc =GRAY2;
//					fc0 = GRAY2;
//				}
//				//if(i<2)
//				//{
//				//	LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 31, 19, fc);/* ���ڱ߿� */
//				//}
//				//else 
//				//LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* ���ڱ߿� */
//				
//				if(vfo[VFO_A].Mode < DEMOD_LSB)
//				{
//					LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* ���ڱ߿� */
//					LCD_ShowString(Function_CW_text[2-i].x+2,Function_CW_text[2-i].y,fc0 ,BLACK , 12, 0, Function_CW_text[2-i].name );
//					LCD_ShowNum(Function_CW_text[2-i].x+42,Function_CW_text[2-i].y,fc0 ,BLACK , 3,12,Menu_Values[7-i]);
//				}
//				else
//				{
//					LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 64, 14, fc);/* ���ڱ߿� */
//					LCD_ShowString(Function_SSB_text[i].x+2,Function_SSB_text[i].y,fc0 ,BLACK , 12, 0, Function_SSB_text[i].name );
//					LCD_ShowNum(Function_SSB_text[i].x+42,Function_SSB_text[i].y,fc0 ,BLACK , 3,12,Menu_Values[i+13]);
//				}
//			}
//			spe.fft_max =0;
//		}
	}
	else
	{
		clk =0;	
	}
	
	if( spe.fft_max >39)
		{
			for(i=0;i<3;i++)
			{
				if(i==ks.EC1_12-1 && ks.EC1_12_LONG ==0)
				{
					fc =RED;
					fc0 =GRAY0;
				}
				else 
				if(i!=ks.EC1_12-1 && ks.EC1_12_LONG ==0)	
				{
					fc =GRAY2;
					fc0 = GRAY2;
				}
				//if(i<2)
				//{
				//	LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 31, 19, fc);/* ���ڱ߿� */
				//}
				//else 
				//LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* ���ڱ߿� */
				
				if(vfo[VFO_A].Mode < DEMOD_LSB)
				{
					LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* ���ڱ߿� */
					LCD_ShowString(Function_CW_text[2-i].x+2,Function_CW_text[2-i].y,fc0 ,BLACK , 12, 0, Function_CW_text[2-i].name );
					LCD_ShowNum(Function_CW_text[2-i].x+42,Function_CW_text[2-i].y,fc0 ,BLACK , 3,12,cw_Values[2-i]);
				}
				else
				{
					LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 64, 14, fc);/* ���ڱ߿� */
					LCD_ShowString(Function_SSB_text[i].x+2,Function_SSB_text[i].y,fc0 ,BLACK , 12, 0, Function_SSB_text[i].name );
					LCD_ShowNum(Function_SSB_text[i].x+42,Function_SSB_text[i].y,fc0 ,BLACK , 3,12,nr_Values[i]);
				}
			}
			spe.fft_max =0;
		}
}
/*
********************************************************************************
*	������ʾ�� 
********************************************************************************
*/
void ui_LongStrip_DisplayWindow(u16 x,u16 y,u8 var)
{
	 u8 x0,y0,i;
	static u8 var_0 ;
	
	if(var_0 != var)
	{
		var/=2;
		if(var >50)var =50;
		for(x0=var_0;x0<3+var_0;x0++)
		{
			for(y0=0;y0<14;y0++)
			{
				Gui_DrawPoint(x0+x,y0+y-6,BLACK);
			}
		}
	
		for(x0=var;x0<3+var;x0++)
		{
			for(y0=0;y0<14;y0++)
			{
				Gui_DrawPoint(x0+x,y0+y-6,RED);
			}
		}
	
		for(i=0;i<52;i++)
		{
			for(y0=0;y0<2;y0++)
			{
				if(i<var+3)Gui_DrawPoint(x+i,y0+y,RED);
				else
					Gui_DrawPoint(x+i,y0+y,GRAY2);
			}
		}
		var_0 = var;
	}
}
/*
********************************************************************************
*	������ʾ 
********************************************************************************
*/
void ui_VOL_DisPlay(u16 VOL_van,char *zi,u8 num)
{
	//u16 fc;
	static u8 num0;
	static u8 clk =0;
	//static u16 vol_van0;
	//static u16 sdvol0;
	//static u8 mark_vol0;
	if(ks.Enc2_Use ==num )
	{
		if(num0 != num){clk=0;num0 = num;}
		if(sd.Enc2_delay >0)
		{
			sd.Enc2_delay -=1;
			ads.vol_timer =VOL_WINDOW_DELAY;
			//ks.agc_m_adjust =0;
			//if(ads.vol_timer<1)ads.vol_timer =0;	
			if(clk ==0)
			{
				//Lcd_Color(125, 140,80,164,BLACK ) ;/* ��ʾ�������� */
				Lcd_Color(60, 131,234,156,GRAY4 ) ;/* ��ʾ�������� */
				if(ks.Enc2_Use ==2)Lcd_Color(117, 136,168,151,BLACK );
				else Lcd_Color(134, 136,185,151,BLACK );
				clk =1;
			}
			
			LCD_RectangleWindow(59, 130, 177, 28, GRAY4);/* ���ڱ߿� */
			LCD_RectangleWindow(60, 131, 175, 26, GRAY4);/* ���ڱ߿� */
			LCD_ShowString(67,132,YELLOW ,GRAY4 , 24, 0, zi); /* ����0��ʾ���� */
			if(ks.Enc2_Use ==2)
			{
				if(TR_READ ==CONTROL_TX)sd.Enc2_delay =VOL_WINDOW_DELAY;
				ui_Decimal_Dispaly(180,132,GRAY0,GRAY4 ,24,VOL_van);/* ��ʾ��ֵ */
				ui_LongStrip_DisplayWindow(117,143,VOL_van/2);/* ����ָʾ����ʾ */
			}
			else if(ks.Enc2_Use ==6)
			{
				LCD_Showvol(180, 132, GRAY0, GRAY4 , 4, 24, VOL_van);/* ��ʾ��ֵ */
			}
			else
			{
				LCD_Showvol(193, 132, GRAY0, GRAY4 , 3, 24, VOL_van);/* ��ʾ��ֵ */
				//VOL_van/=2;
				ui_LongStrip_DisplayWindow(134,143,VOL_van);/* ����ָʾ����ʾ */
			}
		}
	}
	//else
	if(clk ==1)
	{
		if(sd.Enc2_delay<1)
		{
			ks.Enc2_Use =0;
			ks.EC1_12 =0;
			ks.EC1_12_LONG =0;
			ads.vol_timer =0;
			ks.agc_m_adjust =0;
			ks.IF_shift =0;
			Lcd_Color(59, 130,240,158,BLACK ) ;/* ��ʾ�������� */
			sd.spe_fil_time = SPE_FIL_TIME;//Ƶ�״�����ʾ�ظ���ʱ��־
			clk =0;
		}
	}
}
/*
********************************************************************************
*	������ʾ 
********************************************************************************
*/
void ui_Power_Display(u8 data)
{
	//u16 fc;
	static u8 clk =0;
	//static u8 data_0;
	//static u8 rt_key0;
	//static u8 Enc2_Use0;
	
	if(ks.Enc2_Use ==2|| (clk>>1)&0x01)
	{
		if(sd.Enc2_delay >0)
		{
			sd.Enc2_delay -=1;
			if((clk&0x01) ==0)
			{
				Lcd_Color(180, 130,245,159,BLACK ) ;/* ��ʾ�������� */
				clk |=1<<0;
				clk |=1<<1;
			}
			//if(vfo[VFO_A].Mode ==DEMOD_AM)data /=2;
			LCD_RectangleWindow(180, 131, 64, 27, RED);/* ���ڱ߿� */
			LCD_ShowString(230,132,YELLOW ,BLACK , 24,0, "W"); /* ����0��ʾ���� */
			ui_Decimal_Dispaly(182,132,GRAY0,BLACK  ,24,data);/* ��ʾ��ֵ */
			//data_0 = data;
		}
		else
		{
			if(clk&0x01)
			{
				//ks.Enc2_Use =0;
				ks.PWR_key =0;
				Lcd_Color(180, 131,245,159,BLACK ) ;/* ��ʾ�������� */
				//LCD_RectangleWindow(252, 98, 64, 27, GRAY2);/* ���ڱ߿� */
				//LCD_ShowString(302,99,GRAY2 ,BLACK , 24,0, "W"); /* ����0��ʾ���� */
				//ui_Decimal_Dispaly(254,99,GRAY2,BLACK  ,24,data);/* ��ʾ��ֵ */
				//clk |=~(1<<0);
				sd.spe_fil_time = SPE_FIL_TIME;//Ƶ�״�����ʾ�ظ���ʱ��־
				clk =0;
			}
		}
	}
}
/*
********************************************************************************
*	DSP_NR ������ʾ 
********************************************************************************
*/
void ui_dspNR_Display(u8 nr_key,u8 nr_select)
{
	static u8 clk=0;
	//static u8 nr_key0;
	//static u8 nr_select0;
	//u8 fc,bc;
	if(clk==0 || nr_key !=0)
	{
		Lcd_Color(252, 98,319,200,BLACK ) ;/* ��ʾ�������� */
		LCD_RectangleWindow(252, 98, 64, 63, RED);/* ���ڱ߿� */
		Gui_DrawLine(284, 99,284, 115,GRAY2);
		//Gui_DrawLine(252, 98,316, 98,GRAY2);
		Gui_DrawLine(253, 116,314, 116,GRAY2);
		Gui_DrawLine(253, 131,314, 131,GRAY2);
		Gui_DrawLine(253, 146,314, 146,GRAY2);
		//Gui_DrawLine(252, 161,316, 161,GRAY2);
		
		//Gui_DrawLine(252, 98,252, 161,GRAY2);
		//Gui_DrawLine(316, 98,316, 161,GRAY2);
		clk =1;
	}
}









