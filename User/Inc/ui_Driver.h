/*
*******************************************************
*
*
*******************************************************
*/
#ifndef __uiDriver_H
#define __uiDriver_H		
#include "sys.h"	 
#include "stdlib.h"
#include "arm_math.h"

typedef struct
{
	char *text;
	u8 width;
	u8 spe_width;
}Filter_parameters;
extern const  Filter_parameters Fil_p[];
//
typedef struct
{
	char *Mode[8];/* ģʽ��ʾ�ı����� */
	char *V_CH[2];/* VFO/CHת����ʾ�ı����� */
	char *A_B[2]; /* VFO A/Bת����ʾ�ı����� */
	char *RT[4];  /* TX/RX�ı����� */
}TWXT_tybedef;
extern  const  TWXT_tybedef text;

typedef struct
{
	char *name;/* ģʽ��ʾ�ı����� */
	u16  x;
	u16  y;
}SideFeatureText_tybedef;
extern  const  SideFeatureText_tybedef Function_L_text[];
/* ������ʾ���� */
void ui_Mute_icon(u16 x,u16 y,u16 bc,u8 spk);
void ui_Spe_Filter_Display(u16 fil_idx,int8_t mode,u8 clk,u16 bc,u8 h_mode);
void ui_TopBackground(u16 x,u16 y,u16 width,u16 h);/* ���������ɫ */
void key_lock_Display(u16 x);//������
void ui_ATT_Display(u8 k);  /* ATT��ʾ     */
void ui_Decimal_Dispaly(u16 x,u16 y,u16 fc,u16 bc,u8 font,u16 num);/*С����ʾ*/
void ui_Tim_Dispaly(u16 x,u16 y,u16 fc,u16 bc,char font,u8 on);
void ui_Vfo_Display(u8 ch_num);
void ui_Filter_Display(u16 x,u16 y,u16 fc,u16 bc,u8 idx,int16_t mode);
void ui_Step_Display(u16 x,u16 y,int8_t idx);
void ui_RIT_Display(int16_t data );
void ui_Signal_intensity_Display(u16 x,u16 y,u16 fc,u16 bc,u16 irss_po,u8 RT_key);
void ui_BandDisplay(u8 mode,u8 band_idx);

void ui_Spectrum_Dispaly(u16 x,u16 y,u16 width,u16 spe_h,u16 falls_h);
void ui_Spectrum_Dispaly_2(u16 x,u16 y,u16 width,u8 spe_h,u8 falls_h,u16 *pSrc);
void ui_Meun_Window(u16 x, u16 y,u16 h, u16 fc, u16 bc);
void ui_LongStrip_DisplayWindow(u16 x,u16 y,u8 var);
void ui_VOL_DisPlay(u16 VOL_van,char *zi,u8 num);
void ui_Power_Display(u8 data );
void ui_dspNR_Display(u8 nr_key,u8 nr_select);
void ui_SideFunction_Display(void);
void ui_SideFunction_R_Display(u8 rt_key);
#endif
