#include "KEY_Control.h"
#include "sys.h"
#include "rtc.h"
#include "dwt.h"
#include "led.h"  
#include "mpu.h"   
#include "lcd.h" 
#include "timer.h" 
#include "arm_math.h" 
#include "key.h" 
#include "adc.h"
#include "ui_Driver.h"
#include "dsp_filter.h"
#include "gpio_set.h"
#include "Audio_driver.h"
#include "encode.h"
#include "24cxx.h"
#include "Si5351AI2C.h"
#include "softdds.h"
#include "wm8978.h"
#include "usart.h"
//#include "sys.h"
//#include "arm_const_structs.h"
/************************************************************/
/*	FX-4C�ٿس���											*/
/*	�汾��V1.0												*/
/*  ���ߣ�������  BG2IXD                                     */
/*  ʱ�䣺2020/9/20��                                        */
/*	QQ: 1413894726											*/
/*	���䣺bg2ixd2010@qq.com									*/
/************************************************************/
	__IO Key_Reg_type  *KEY;
	 KeyManagement_TypeDef		ks;
	Storage_Data 				sd;
__IO At24c_addr_GenInfo		vfo[2];
/**/
const BandGenInfo   Band_Info[ COUPLING_MAX];
const BandGenInfo   RxBand_Info[ RADIO_MAX];

extern float32_t	lms1_nr_delay[LMS_NR_DELAYBUF_SIZE_MAX+BUFF_LEN];
//
extern float32_t errsig1[64+10];
extern float32_t errsig2[64+10];
extern float32_t result[64+10];
//
// LMS Filters for RX
extern __IO Dspnr_typedef nr;
extern arm_lms_norm_instance_f32	lms1Norm_instance;
extern arm_lms_instance_f32	lms1_instance;
extern float32_t	lms1StateF32[LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN];
extern float32_t	lms1NormCoeff_f32[LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN];
/* IIR��Ƶ�˲��� */
extern arm_biquad_casd_df1_inst_f32 IIR_AF;
extern float32_t AF_State[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
extern float32_t  AF_Values;
/*	Riss��ͨ */
extern arm_biquad_casd_df1_inst_f32 RISS;
extern float32_t Riss_State[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
extern float32_t  Riss_Values;
/* IIR���ߴ��˲��� */
extern arm_biquad_casd_df1_inst_f32 IIR_LPF_I;
extern arm_biquad_casd_df1_inst_f32 IIR_LPF_Q;
extern float32_t IIRStateF32_I[IIR_NUM_TAPS]; /* ͬ�� I ͨ��״̬���棬��СnumStages*4 */
extern float32_t IIRStateF32_Q[IIR_NUM_TAPS]; /* ͬ�� Q ͨ��״̬���棬��СnumStages*4 */
extern float32_t  IIR_LP_Values;			   /* IIR�˲�������ϵ�� */
/* ��ȡFIR�˲��� */
extern arm_fir_decimate_instance_f32 DECIMATE_I;//��ȡ�˲��� I
extern arm_fir_decimate_instance_f32 DECIMATE_Q;//��ȡ�˲��� Q
extern float32_t DecimState_I[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR��ȡ�˲���״̬����I */
extern float32_t DecimState_Q[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR��ȡ�˲���״̬����Q */
/* FIR�ڲ��˲��� */
extern arm_fir_interpolate_instance_f32 INTERPOLATE_I;
extern arm_fir_interpolate_instance_f32 INTERPOLATE_Q;
extern float32_t InterpState_I[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR�ڲ��˲���״̬����I */
extern float32_t InterpState_Q[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR�ڲ��˲���״̬����Q */
/* FIR�ߴ��˲��� */
extern arm_fir_instance_f32 FIR_LPF_I;
extern arm_fir_instance_f32 FIR_LPF_Q;
extern float32_t FIR_State_I[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];
extern float32_t FIR_State_Q[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];
/**/
extern SpectrumProcessing_typedef	spe;
/**/
extern  Audio_Digital_Signal	 ads;
/**/
/*
 *	FFT
*/
//extern  arm_rfft_fast_instance_f32 Spe_FFT;
//arm_rfft_fast_instance_f32  Spe_FFT;	//FFTָ��	
u8 ucKeyCode;
const u16 Addr_OFFSET[2]=
{
	ADDR_RT_OFFSET,
	ADDR_RX_OFFSET
};
u16  Rx_amp_user[10];
u8  Rx_phase_user[10];
u16  Rx_amp_adjust[12];
/**/
const u16 CH_ADDR[8] =
{
	ADDR_CH_NUM(VFO_A) ,
	ADDR_CH_NUM(VFO_B) ,
	ADDR_CH_NUM(CHA_NUM),
	ADDR_CH_NUM(CHB_NUM),
	ADDR_CH_NUM(RXVFO_A),
	ADDR_CH_NUM(RXVFO_B) , 
	ADDR_CH_NUM(RXCHA_NUM) ,
	ADDR_CH_NUM(RXCHB_NUM) 
};
/*
*******************************************************************************************************
* POWER��Դ����
*******************************************************************************************************
*/

/*
*	@��Դ�������� PowerControl_Init������gpio_set.h���壩ִ��
*	@�˺����ǹػ��ٿ�
*/
void PowerControl_off(void)
{
	//static u8 power_key =0;
	//ucKeyCode = bsp_GetKey();//��ȡ��ֵ
	if(ucKeyCode != KEY_NONE)//�а�������
	{
		if(ucKeyCode == KEY_8_LONG )//power_key++;
		{
			Lcd_Color(0,0,319,239,BLACK);
			LCD_ShowNum(152,110,GRAY1,BLUE2, 2,32,73);
			WM8978_SPKvol_Set(0);
			WM8978_HPvol_Set(0,0);
			delay_ms (2000);
	
			if(POWER_READ == 0 )POWER_SET_OFF;
		}
	}
}
void Key_Management_Set(void)
{
	//static u8 ucKeyCode;
	//static u8 radio_mode;
	//static u8 Same_Diffrenge;
	//static u8 ch_vfo;
	//static u8 a_b;
	//ucKeyCode = bsp_GetKey();//��ȡ��ֵ
	if (ucKeyCode != KEY_NONE)//�а�������
	{
		//��������
		if(ks.key_lock== KEYLOCK_ON)//��������
		{						
			// ���ܼ��趨
			if(ks.F_11==0)
			{
				//*RIT ����
				if(sd.ritatt_ptt !=1 )
				{
					PTT_IN();
					if( ucKeyCode==KEY_5_UP )
					{
						ks.RIT_key = ~ks.RIT_key &0x01;		//RIT ���أ�0���رգ�1������
						if(ks.RIT_key >0) sd.rit_delay =VOL_WINDOW_DELAY*2;
					}
					if( ucKeyCode==KEY_5_LONG&&sd.ritatt_ptt ==0)
					{
						ks.RIT_LONG = ~ks.RIT_LONG &0x01;		//RIT���� ���أ�0���رգ�1������
						//if(ks.RIT_LONG >1) ks.RIT_LONG =0;
					}
				}
				
				//PWR ���ʵ���
				if( ucKeyCode==KEY_8_UP)
				{
					ks.PWR_key =~ ks.PWR_key &0x01;		//PWR ���ʵ������أ�0��OFF,1:ON��
					if(ks.PWR_key)sd.Enc2_delay =VOL_WINDOW_DELAY;
					else sd.Enc2_delay =0;
					sd.spe_fil_time = SPE_FIL_TIME;//Ƶ�״�����ʾ�ظ���ʱ��־
				}
				if( ucKeyCode==KEY_8_LONG)
				{
					ks.PWR_LONG = ~ks.PWR_LONG &0x01;		//PWR ���ʵ������س�����0��OFF,1:ON��
					//sd.Enc2_delay =VOL_WINDOW_DELAY;
					//if(ks.PWR_LONG >1) ks.PWR_LONG =0;
				}
			}
		}
	}
}
/*
***************************************************************************
*	��ֵ��ȡ�밴��������
***************************************************************************
*/
void KeyCode_lock_settings()
{
	ucKeyCode = bsp_GetKey();//��ȡ��ֵ
	if (ucKeyCode != KEY_NONE)//�а�������
	{
		sd.key_BL_delay = 200;//���������ӳ�10��رգ�
		if( ucKeyCode==KEY_11_LONG )// ������ON/OFF;0:OFF ������������;1:ON ��F���⣬���а�����������
		{
			ks.key_lock = ~ks.key_lock & 0x01;
		}
	}
}
/*
***************************************************************************
*	���ο��ƹ���
***************************************************************************
*/
/* 
* 	�շ�ģʽҵ�ನ�λ���
*/
const BandGenInfo   Band_Info[ COUPLING_MAX] =
{
	//{1800000, 2000000, "160m" },
    { 35,	140,	" 80m ",    3500000,   3800000,  4000000},/* 80m */
    {105,	140,	" 60m ",	5300000,   5360000,  5500000},/* 60m */
    {175,	140,	" 40m ",	7000000,   7050000,  7350000},/* 40m */
    {245,	140,	" 30m ",	9998000,  10100000, 10150000},/* 30m */
	
    { 35,	170,	" 20m ",    14000000, 14270000, 14350000},/* 20m */
    {105,	170,	" 17m " ,	18068000, 18100000, 18168000},/* 17m */
    {175,	170,	" 15m ",	21000000, 21200000, 21450000},/* 15m */
    {245,	170,	" 12m ",	24800000, 24900000, 25000000},/* 12m */
	
    {105,	200,	" 10m ",	27000000, 29200000, 30000000},/* 10m */
    {175,	200,	"  6m ",	50000000, 50200000, 54000000},/* 6m  */
};
/*
*	���ջ�ģʽ���λ���
*/
const BandGenInfo   RxBand_Info[ RADIO_MAX] =
{
	 //{150000, 285000, "LW" },
    {  4,	140,	"     ",    522000,     945000,	  	  1710000},/* MW */
    { 67,	140,	"     ",	2300000,    2400000,	  2498000},/* 120m */
    {130,	140,	" 90m ",	3200000,    3300000,	  3400000},/* 90m */
    {193,	140,	" 75m ",	3900000,    3950000,	  4000000},/* 75m */
    {256,	140,	" 60m ", 	4700000,    5500000,	  5600000},/* 60m */
	
    {  4,	170,	" 49m ",	5900000,    6000000,	  7000000},/* 49m */
    { 67,	170,	" 41m " ,	7100000,    7200000,	  7800000},/* 41m */
    {130,	170,	" 31m ",	9400000,    9500000,	  9900000},/* 31m */
	{193,	170,	" 25m ",	11600000, 	 12000000,	 13100000},/* 25m */
    {256,	170,	" 22m ",	13570000, 	 13700000,	 13870000},/* 22m */
	
    {  4,	200,	" 19m ",	15100000, 	 15500000,	 15800000},/* 19m */
    { 67,	200,	" 16m ",	17480000, 	 17700000,	 17900000},/* 16m */
    {130,	200,	" 13m ",	21450000,	 21500000,	 21750000},/* 15m */
    {193,	200,	" 11m ",	25600000, 	 27000000,	 27405000},/* 13m */
    {256,	200,	" 10m ",	28000000, 	 29000000,	 30000000},/* 11m */
	
	//{ 67,	208,	"  6m ",	50000000, 	 51000000,	 54000000},/* 6m */
	//{64000000, 	 88000000,	108000000},/* FM */
	//{130,	208,	" 2m5 ",	118000000, 120000000,	137000000},/* 2m5 */
	//{193,	208,	" 2m  ",	137000000, 145000000,	174000000},/* 2m */
};
/*
*	Key_Band_Control()
*	���ο��ƺ���
*/
void Key_Band_Control()
{
	u8 i;
	u16 fc;
	static u8 clk=0;
	static u16 Band_bp;
	static u16 Band_0;
	static u16 RxBand_0;
	static u32 freq_0;
	static u32 Band_Switch;/* ����IO���� */
	//static u16 Timing;
	//static u8 clk =0;/* ������� */
	static u8 id0;	 /* ���μ�� */
	//static u8 Rx_id0;	 /* ���μ�� */
	static u8 mode0; /* ģʽ��� */
	//static u32 freq; /* Ƶ�ʼ�� */
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE)//�а����������������
	{
		/* ���̹���ת��
		* 0�����ܼ���
		* 1������ת����
		*/
		if( ucKeyCode == KEY_11_UP )//F���
		{
			ks.F_11 = ~ks.F_11&0x01;//0����尴�������ǰ����ϴ������ע�Ĺ��ܣ�1:�ǲ���ѡ���ܣ������ϵ�С�����ע������ѡ���Σ�
			Lcd_Color(0,130,319,239,BLACK);/* ��ʾ�������� */
			if(ks.F_11 ==1)sd.band_set_delay =100;/* �趨�Զ��˳��ļ�ʱʱ�� */
			else sd.band_set_delay =10;
		}
		/* ����ѡ�� */
		if(ks.F_11 ==1)
		{
			sd.spe_fil_time=SPE_FIL_TIME;//Ƶ�״�����ʾ�ظ���ʱ��־
			ks.FunctionWindow_Use =1;//�๦�ܴ�����ռ��
			//Lcd_Color(0,130,319,239,BLACK);/* ��ʾ�������� */
			ks.menu_key = MENU_OFF ;
			//ui_BandDisplay(ks.rt_rx  & 0x01,ks.RxBand_id);
			if(ks.rt_rx ==0)/* �շ��Ż�ģʽ */
			{
				//ui_BandDisplay(0,ks.RxBand_id);
				switch(ucKeyCode)
				{
					case KEY_1_UP:  
					{
						vfo[VFO_A].Band_id = COUPLING_80M;
						Band_bp =9;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_LSB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_2_UP:  
					{
						vfo[VFO_A].Band_id = COUPLING_60M;
						Band_bp =6;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_LSB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_6_UP:  
					{
						vfo[VFO_A].Band_id = COUPLING_40M;
						Band_bp =5;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_LSB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_3_UP: 
					{
						vfo[VFO_A].Band_id = COUPLING_30M;
						Band_bp =4;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_USB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_4_UP:
					{
						vfo[VFO_A].Band_id = COUPLING_20M;
						Band_bp =3;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_USB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_5_UP:  
					{
						vfo[VFO_A].Band_id = COUPLING_17M;
						Band_bp =2;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_USB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_7_UP:  
					{
						vfo[VFO_A].Band_id = COUPLING_15M;
						Band_bp =2;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_USB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_8_UP:  
					{
						vfo[VFO_A].Band_id = COUPLING_12M;
						Band_bp =1;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_USB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
					case KEY_9_UP:  
					{
						vfo[VFO_A].Band_id = COUPLING_10M;
						Band_bp =1;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_USB;
						ks.Band_key  =0;
						if(sd.band_set_delay >10)sd.band_set_delay  =10;
					}break;
					case KEY_10_UP: 
					{
						vfo[VFO_A].Band_id = COUPLING_6M;
						Band_bp =0;
						//ks.Band_key  =0;
						//vfo[VFO_A].Mode = DEMOD_USB;
						ks.Band_key  =0;
						//if(sd.band_set_delay >1)sd.band_set_delay  =0;
					}break;
				}
				//key_Mode_Control();
			}
			else
			if(ks.rt_rx  ==1)/* ���ջ�ģʽ */
			{
				//ui_BandDisplay(1,ks.RxBand_id);
				switch(ucKeyCode)
				{
					case KEY_1_UP:  
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id <= RADIO_MW)
							vfo[VFO_A].RxBand_id = RADIO_120M;
						if(vfo[VFO_A].RxBand_id > RADIO_120M)
							vfo[VFO_A].RxBand_id = RADIO_MW;
						ks.Band_key  =0;
					}break;
					case KEY_2_UP:
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id > RADIO_75M)
							vfo[VFO_A].RxBand_id = RADIO_90M;
						if(vfo[VFO_A].RxBand_id < RADIO_90M)
							vfo[VFO_A].RxBand_id = RADIO_75M;
						ks.Band_key  =0;
					}break;
					case KEY_6_UP:  
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id < RADIO_60M)
							vfo[VFO_A].RxBand_id = RADIO_49M;
						if(vfo[VFO_A].RxBand_id > RADIO_49M)
							vfo[VFO_A].RxBand_id = RADIO_60M;
						ks.Band_key  =0;
					}break;	
					case KEY_3_UP:  
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id < RADIO_41M)
							vfo[VFO_A].RxBand_id = RADIO_31M;
						if(vfo[VFO_A].RxBand_id > RADIO_31M)
							vfo[VFO_A].RxBand_id = RADIO_41M;
						ks.Band_key  =0;
					}break;
					case KEY_4_UP:
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id < RADIO_25M)
							vfo[VFO_A].RxBand_id = RADIO_22M;
						if(vfo[VFO_A].RxBand_id > RADIO_22M)
							vfo[VFO_A].RxBand_id = RADIO_25M;
						ks.Band_key  =0;
					}break;
					case KEY_5_UP:  
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id < RADIO_19M)
							vfo[VFO_A].RxBand_id = RADIO_16M;
						if(vfo[VFO_A].RxBand_id > RADIO_16M)
							vfo[VFO_A].RxBand_id = RADIO_19M;
						ks.Band_key  =0;
					}break;	
					case KEY_7_UP:  
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id < RADIO_19M)
							vfo[VFO_A].RxBand_id = RADIO_16M;
						if(vfo[VFO_A].RxBand_id > RADIO_16M)
							vfo[VFO_A].RxBand_id = RADIO_19M;
						ks.Band_key  =0;
					}break;
					case KEY_8_UP:
					{
						vfo[VFO_A].RxBand_id ++;
						if(vfo[VFO_A].RxBand_id < RADIO_13M)
							vfo[VFO_A].RxBand_id = RADIO_11M;
						if(vfo[VFO_A].RxBand_id > RADIO_11M)
							vfo[VFO_A].RxBand_id = RADIO_13M;
						ks.Band_key  =0;
					}break;
					case KEY_9_UP:  
					{
						vfo[VFO_A].RxBand_id = RADIO_10M;
						ks.Band_key  =0;
						if(sd.band_set_delay >10)sd.band_set_delay  =10;
					}break;
//					case KEY_10_UP:  
//					{
//						vfo[VFO_A].RxBand_id ++;
//						if(vfo[VFO_A].RxBand_id < RADIO_2M5)
//							vfo[VFO_A].RxBand_id = RADIO_2M;
//						if(vfo[VFO_A].RxBand_id > RADIO_2M)
//							vfo[VFO_A].RxBand_id = RADIO_2M5;
//						ks.Band_key  =0;
//					}break;
				}
			}	
		}
	}
	/*
	*	ֻ�ڿ����������б仯ʱִ�в���
	*	�Լ���CPU���������ˢ���ʣ�
	*/
	if(ks.F_11 ==1)/* ���벨��ѡ����� */
	{
		ks.FunctionWindow_Use =1;//�๦�ܴ�����ռ��
		if(clk ==0)/* ��ʾ��������һ�� */
		{
			//Lcd_Color(0,130,319,239,BLACK);/* ��ʾ�������� */	
			clk =1;
		}
		if(ks.rt_rx ==1)/* ���ջ�ģʽ */
		{
			ads.tx_delay =100;
			//vfo[VFO_A].Freq = RxBand_Info[vfo[VFO_A].RxBand_id].Default;/* ��ѡ���ε�Ĭ��Ƶ�� */
			vfo[VFO_A].Freq = AT24CXX_ReadLenByte( vfo[VFO_A].RxBand_id *4 + ADDR_RX_BAND_FREQ_M ,4 );
			/* Ƶ���޶� */
			if(vfo[VFO_A].Freq <  RxBand_Info[vfo[VFO_A].RxBand_id].start || vfo[VFO_A].Freq > RxBand_Info[vfo[VFO_A].RxBand_id].end )//
			{
				vfo[VFO_A].Freq =  RxBand_Info[vfo[VFO_A].RxBand_id].Default ;
				//vfo[VFO_A].RxBand_id =  RADIO_MW;
			}
			vfo[VFO_A].Mode = AT24CXX_ReadLenByte( vfo[VFO_A].RxBand_id  + ADDR_RX_BAND_MODE_M ,1 );
			if(vfo[VFO_A].Mode >DEMOD_FM )vfo[VFO_A].Mode = DEMOD_FM;
			if(vfo[VFO_A].Mode <DEMOD_CW_LSB )vfo[VFO_A].Mode = DEMOD_CW_LSB;
			
			AT24CXX_WriteOneByte( MODE_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],vfo[VFO_A].Mode );
			
			vfo[VFO_A].FilterSelected_id = AT24CXX_ReadLenByte( vfo[VFO_A].RxBand_id  + ADDR_RX_BAND_FIL_M ,1 );
			if(vfo[VFO_A].Mode < DEMOD_AM )
			{
				if(vfo[VFO_A].FilterSelected_id > 5 )vfo[VFO_A].FilterSelected_id = 5;
			}
			else if(vfo[VFO_A].FilterSelected_id > 1 )vfo[VFO_A].FilterSelected_id = 1;
			if(vfo[VFO_A].FilterSelected_id < 1 )vfo[VFO_A].FilterSelected_id = 0;
			
			AT24CXX_WriteOneByte( FILTERSELECTED_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ], vfo[VFO_A].FilterSelected_id );
			for(i=2;i<RADIO_MAX;i++)
			{
				if(i ==vfo[VFO_A].RxBand_id)fc =RED;/* ѡ�еĲ�������Ϊ��ɫ */
				else
					fc =GRAY0;
				
				/* ��ʾ���ջ�ģʽ�Ĳ���ѡ����� */
				LCD_ShowString(  RxBand_Info[i].x ,  RxBand_Info[i].y , fc, GRAY4 , 24, 0, RxBand_Info[i].text );
				//LCD_RectangleWindow(u8 x, u8 y, u8 w, u8 h, u16 fc);
			}
			switch(vfo[VFO_A].RxBand_id)
			{
				case RADIO_MW:		Band_bp =9;break;
				case RADIO_120M:	Band_bp =9;break;
				case RADIO_90M:		Band_bp =9;break;
				case RADIO_75M:		Band_bp =6;break;
				case RADIO_60M:		Band_bp =6;break;
				case RADIO_49M:		Band_bp =5;break;
				case RADIO_41M:		Band_bp =5;break;
				case RADIO_31M:		Band_bp =4;break;
				case RADIO_25M:		Band_bp =4;break;
				case RADIO_22M:		Band_bp =3;break;
				case RADIO_19M:		Band_bp =3;break;
				case RADIO_16M:		Band_bp =2;break;
				case RADIO_13M:		Band_bp =1;break;
				case RADIO_11M:		Band_bp =1;break;
				case RADIO_10M:		Band_bp =1;break;
			}
			//ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_RXGAIN+vfo[VFO_A].RxBand_id, 1);
			//if(ads.IF_gain >63)ads.IF_gain =32;
			//if(ads.IF_gain < 1)ads.IF_gain =32;
		}
		else
		{
			ads.tx_delay =100;
			//vfo[VFO_A].Freq = Band_Info[vfo[VFO_A].Band_id].Default;
			vfo[VFO_A].Freq = AT24CXX_ReadLenByte( vfo[VFO_A].Band_id *4 + ADDR_RT_BAND_FREQ_M ,4 );
			/* Ƶ���޶� */
			if(vfo[VFO_A].Freq <  Band_Info[vfo[VFO_A].Band_id].start || vfo[VFO_A].Freq > Band_Info[vfo[VFO_A].Band_id].end )//
			{
				vfo[VFO_A].Freq =  Band_Info[vfo[VFO_A].Band_id].Default ;
				//vfo[VFO_A].RxBand_id =  RADIO_MW;
			}
			vfo[VFO_A].Mode = AT24CXX_ReadLenByte( vfo[VFO_A].Band_id + ADDR_RT_BAND_MODE_M ,1 );
			if(vfo[VFO_A].Mode >DEMOD_FM )vfo[VFO_A].Mode = DEMOD_FM;
			if(vfo[VFO_A].Mode <DEMOD_CW_LSB )vfo[VFO_A].Mode = DEMOD_CW_LSB;
			
			AT24CXX_WriteOneByte( MODE_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],vfo[VFO_A].Mode );
			
			vfo[VFO_A].FilterSelected_id = AT24CXX_ReadLenByte( vfo[VFO_A].RxBand_id  + ADDR_RX_BAND_FIL_M ,1 );
			if(vfo[VFO_A].Mode < DEMOD_AM )
			{
				if(vfo[VFO_A].FilterSelected_id > 5 )vfo[VFO_A].FilterSelected_id = 5;
			}
			else if(vfo[VFO_A].FilterSelected_id > 1 )vfo[VFO_A].FilterSelected_id = 1;
			if(vfo[VFO_A].FilterSelected_id < 1 )vfo[VFO_A].FilterSelected_id = 0;
			
			AT24CXX_WriteOneByte( FILTERSELECTED_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ], vfo[VFO_A].FilterSelected_id );
			for(i=0;i<COUPLING_MAX;i++)/*�շ���ģʽ������ʾ */
			{
				if(i ==vfo[VFO_A].Band_id)fc =RED;/* ѡ�еĲ�������Ϊ��ɫ */
				else
					fc =GRAY0;
				
				/* ��ʾ�շ�ģʽ�Ĳ���ѡ����� */
				LCD_ShowString( Band_Info[i].x , Band_Info[i].y , fc, GRAY4 , 24, 0, Band_Info[i].text );
				//LCD_RectangleWindow(u8 x, u8 y, u8 w, u8 h, u16 fc);
			}
			switch(vfo[VFO_A].Band_id)
			{
				case COUPLING_80M:		Band_bp =9;break;
				case COUPLING_60M:		Band_bp =6;break;
				case COUPLING_40M:		Band_bp =5;break;
				case COUPLING_30M:		Band_bp =4;break;
				case COUPLING_20M:		Band_bp =3;break;
				case COUPLING_17M:		Band_bp =2;break;
				case COUPLING_15M:		Band_bp =2;break;
				case COUPLING_12M:		Band_bp =1;break;
				case COUPLING_10M:		Band_bp =1;break;
				case COUPLING_6M:		Band_bp =0;break;
			}
			//sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
			//ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+vfo[VFO_A].Band_id, 1);
			//if(ads.IF_gain >63)ads.IF_gain =32;
			//if(ads.IF_gain < 1)ads.IF_gain =32;
		}
		if(sd.band_set_delay >0)sd.band_set_delay--;
		if(sd.band_set_delay <1)
		{
			ks.F_11 =0;/* ��ʱʱ�䵹��ʱ���㣬����Ƶ����ʾ���� */
			sd.band_set_delay =0;
		}
//		if(ks.rt_rx )
//		{
//			//sd.band = vfo[VFO_A].RxBand_id; //����IF����
//			ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_RXGAIN+vfo[VFO_A].RxBand_id, 1);
//		}
//		else
//		{
//			//sd.band = vfo[VFO_A].Band_id; //����IF����
//			//ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+vfo[VFO_A].Band_id, 1);
//			if(sd.menu_page)sd.Pow =50;
//			else sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
//		}
//		if(ads.IF_gain >63)ads.IF_gain =32;
//		if(ads.IF_gain < 1)ads.IF_gain =32;
	}
	else/* �˳�������ʾ���� */
	{
		ks.FunctionWindow_Use =0;//�๦�ܴ����ѽ��ռ��
		if(clk ==1)
		{
			Lcd_Color(0,130,319,239,BLACK);/* ��ʾ�������� */	
			clk =0;
			//sd.band_set_delay  =100;/* �趨�Զ��˳��ļ�ʱʱ�� */
		}
		if(sd.band_set_delay >0)sd.band_set_delay--;
		if(sd.band_set_delay <1)
		{
			ks.F_11 =0;/* ��ʱʱ�䵹��ʱ���㣬����Ƶ����ʾ���� */
			sd.band_set_delay =0;
		}
	}
	
	if(ks.Band_key  ==0)
	{
		clk =0;
		ks.F_11 =0;
		ks.Band_key =1;
		Lcd_Color(0,130,319,239,BLACK);/* ��ʾ�������� */
		if(sd.band_set_delay >1)sd.band_set_delay--;
	}
	if(ks.F_11 ==0)
	{
		if(Band_0 != vfo[VFO_A].Band_id ||RxBand_0 != vfo[VFO_A].RxBand_id || freq_0 != vfo[VFO_A].Freq)
		{
			if(ks.rt_rx ==1)/* ���ջ�ģʽ */
			{
				/*
				*	��Ƶ����ʾ
				*/
	//			if(vfo[VFO_A].Freq <  RxBand_Info[vfo[VFO_A].RxBand_id].start || vfo[VFO_A].Freq > RxBand_Info[vfo[VFO_A].RxBand_id].end -10)//
	//			{
	//				vfo[VFO_A].Freq =  RxBand_Info[vfo[VFO_A].RxBand_id].Default ;
	//				//vfo[VFO_A].RxBand_id =  RADIO_MW;
	//			}
				/* �Ͷ�Ƶ���޶� */
				if(vfo[VFO_A].Freq <  RxBand_Info[RADIO_90M].start)//
				{
					vfo[VFO_A].Freq =  RxBand_Info[RADIO_90M].start;
					//vfo[VFO_A].RxBand_id =  RADIO_MW;
				}
				/* ��Ƶ�ʵͶ˵�г */
				if(vfo[VFO_A].Freq <  RxBand_Info[vfo[VFO_A].RxBand_id].start && vfo[VFO_A].Freq >  RxBand_Info[vfo[VFO_A].RxBand_id-1].end)//�±߽�
				{	
					vfo[VFO_A].RxBand_id --;
					//if(vfo[VFO_A].RxBand_id <RADIO_MW) vfo[VFO_A].RxBand_id = RADIO_MW; //
					vfo[VFO_A].Freq =(u32) (RxBand_Info[vfo[VFO_A].RxBand_id].end/sd.tuning_step)*sd.tuning_step;
				}
				
				/* ��Ƶ�ʸ߶˵�г */
				if(vfo[VFO_A].Freq >  RxBand_Info[vfo[VFO_A].RxBand_id].end && vfo[VFO_A].Freq < RxBand_Info[vfo[VFO_A].RxBand_id+1].start) 	//�ϱ߽�
				{
					vfo[VFO_A].RxBand_id  ++;
					//if(vfo[VFO_A].RxBand_id > RADIO_2M) vfo[VFO_A].RxBand_id = RADIO_2M;
					vfo[VFO_A].Freq = RxBand_Info[vfo[VFO_A].RxBand_id].start;
				}
				/* �߶�Ƶ���޶� */
				if(vfo[VFO_A].Freq > RxBand_Info[RADIO_10M].end -1)//
				{
					vfo[VFO_A].Freq =  RxBand_Info[RADIO_10M].Default ;
					//vfo[VFO_A].RxBand_id =  RADIO_2M;
				}
	//			/* ����İ��Ƶ�����ڵĲ��� */
	//			if(i != vfo[VFO_A].RxBand_id)
	//			{
	//				if(++i >RADIO_2M)i = RADIO_2M;/* �����޶� */
	//				if(vfo[VFO_A].Freq >= RxBand_Info[i].start && vfo[VFO_A].Freq <= RxBand_Info[i].end )/* ���β��� */
	//				vfo[VFO_A].RxBand_id = i;/* �ʺϵĲ���IDд�� */	
	//			}
				if(mode0 ==0|| id0!=vfo[VFO_A].RxBand_id)
				{
					if(vfo[VFO_A].Freq < RxBand_Info[RADIO_MW].end  )
					{
						ks.StepSelected_idx = T_STEP_9KHZ_IDX;//MW���β���Ĭ������Ϊ9kHz;
						vfo[VFO_A].Freq /=9000;
						vfo[VFO_A].Freq *=9000;
					}
					else
					//if(vfo[VFO_A].RxBand_id > RADIO_MW)
					{
						ks.StepSelected_idx = T_STEP_5KHZ_IDX;//�������β���Ĭ������Ϊ5kHz;
						//vfo[VFO_A].Freq +=3000;
						vfo[VFO_A].Freq /=5000;
						vfo[VFO_A].Freq *=5000;
					}
					mode0 = 1;
					id0 = vfo[VFO_A].RxBand_id;
				}
				/*
				*	��Ƶ����ʾ
				*/
				/* �Ͷ�Ƶ���޶� */
				if(vfo[VFO_B].Freq <  RxBand_Info[RADIO_90M].start)//
				{
					vfo[VFO_B].Freq =  RxBand_Info[RADIO_90M].start;
					vfo[VFO_B].RxBand_id =  RADIO_MW;
				}
				/* ��Ƶ�ʵͶ˵�г */
				if(vfo[VFO_B].Freq <  RxBand_Info[vfo[VFO_B].RxBand_id].start && vfo[VFO_B].Freq >  RxBand_Info[vfo[VFO_B].RxBand_id-1].end)//�±߽�
				{	
					vfo[VFO_B].RxBand_id --;
					//if(vfo[VFO_A].RxBand_id <RADIO_MW) vfo[VFO_A].RxBand_id = RADIO_MW; //
					vfo[VFO_B].Freq =(u32) (RxBand_Info[vfo[VFO_B].RxBand_id].end/sd.tuning_step)*sd.tuning_step;
				}
				
				/* ��Ƶ�ʸ߶˵�г */
				if(vfo[VFO_B].Freq >  RxBand_Info[vfo[VFO_B].RxBand_id].end && vfo[VFO_B].Freq < RxBand_Info[vfo[VFO_B].RxBand_id+1].start) 	//�ϱ߽�
				{
					vfo[VFO_B].RxBand_id  ++;
					//if(vfo[VFO_A].RxBand_id > RADIO_2M) vfo[VFO_A].RxBand_id = RADIO_2M;
					vfo[VFO_B].Freq = RxBand_Info[vfo[VFO_B].RxBand_id].start;
				}
				/* �߶�Ƶ���޶� */
				if(vfo[VFO_B].Freq > RxBand_Info[RADIO_10M].end -1)//
				{
					vfo[VFO_B].Freq =  RxBand_Info[RADIO_10M].Default ;
					//vfo[VFO_B].RxBand_id =  RADIO_2M;
				}
	//			/* ����İ��Ƶ�����ڵĲ��� */
	//			if(i != vfo[VFO_B].RxBand_id)
	//			{
	//				if(++i >RADIO_2M)i = RADIO_2M;/* �����޶� */
	//				if(vfo[VFO_B].Freq >= RxBand_Info[i].start && vfo[VFO_B].Freq <= RxBand_Info[i].end )/* ���β��� */
	//				vfo[VFO_B].RxBand_id = i;/* �ʺϵĲ���IDд�� */	
	//			}
				sd.band = vfo[VFO_A].RxBand_id; //����IF����
			
				/*
				*	���ο���
				*/
				Band_Switch = 0x0000;					//����IO���㣬�͵�λ��
				//Band_Switch = BAND_ON << ks.Band_id;	//������ѡ�Ĳ���IOΪ�ߵ�ƽ��
				switch(vfo[VFO_A].RxBand_id)
				{
					case RADIO_MW:	{ads.rx_amp_band =0;	Band_bp =9;}break;
					case RADIO_120M:{ads.rx_amp_band =0;	Band_bp =9;}break;
					case RADIO_90M:	{ads.rx_amp_band =0;	Band_bp =9;}break;
					case RADIO_75M:	{ads.rx_amp_band =1;	Band_bp =6;}break;
					case RADIO_60M:	{ads.rx_amp_band =1;	Band_bp =6;}break;
					case RADIO_49M:	{ads.rx_amp_band =2;	Band_bp =5;}break;
					case RADIO_41M:	{ads.rx_amp_band =2;	Band_bp =5;}break;
					case RADIO_31M:	{ads.rx_amp_band =3;	Band_bp =4;}break;
					case RADIO_25M:	{ads.rx_amp_band =3;	Band_bp =4;}break;
					case RADIO_22M:	{ads.rx_amp_band =4;	Band_bp =3;}break;
					case RADIO_19M:	{ads.rx_amp_band =5;	Band_bp =3;}break;
					case RADIO_16M:	{ads.rx_amp_band =6;	Band_bp =2;}break;
					case RADIO_13M:	{ads.rx_amp_band =7;	Band_bp =1;}break;
					case RADIO_11M:	{ads.rx_amp_band =8;	Band_bp =1;}break;
					case RADIO_10M:	{ads.rx_amp_band =8;	Band_bp =1;}break;
				}
				/*
				*	���ò��δ�ͨ����IO��ƽ��	
				*/		
				BAND_A0((Band_bp>>0)&0x01);
				BAND_A1((Band_bp>>1)&0x01);
				BAND_A2((Band_bp>>2)&0x01);
				BAND_A3((Band_bp>>3)&0x01);
				/*
				*	���ò��ε�ͨ����IO��ƽ��
				*/
				LPF_80M(BAND_OFF);
				LPF_60_40M(BAND_OFF);
				LPF_30_20M(BAND_OFF);
				LPF_17_15M(BAND_OFF);
				LPF_12_10M(BAND_OFF);
			}
			else/* �շ��Ż�ģʽ */
			{
				/*
				*	��Ƶ����ʾ
				*/
				
				/* �Ͷ�Ƶ���޶� */
				if(vfo[VFO_A].Freq <  Band_Info[COUPLING_80M].start)//
				{
					vfo[VFO_A].Freq =  Band_Info[COUPLING_80M].start;
					vfo[VFO_A].Band_id =  COUPLING_80M;
				}
				/* ��Ƶ�ʵͶ˵�г */
				if(vfo[VFO_A].Freq < Band_Info[vfo[VFO_A].Band_id].start && vfo[VFO_A].Freq > Band_Info[vfo[VFO_A].Band_id-1].end)	//�±߽�
				{	
					vfo[VFO_A].Band_id --;
					//if(vfo[VFO_A].Band_id <= COUPLING_80M) vfo[VFO_A].Band_id =  COUPLING_80M; //
					vfo[VFO_A].Freq = (u32)(Band_Info[vfo[VFO_A].Band_id].end/sd.tuning_step) * sd.tuning_step;
				}
				/* ��Ƶ�ʸ߶˵�г */
				if(vfo[VFO_A].Freq > Band_Info[vfo[VFO_A].Band_id].end && vfo[VFO_A].Freq < Band_Info[vfo[VFO_A].Band_id+1].start) 	//�ϱ߽�
				{
					vfo[VFO_A].Band_id  ++;
					//if(ks.Band_id >=  COUPLING_6M) vfo[VFO_A].Band_id =  COUPLING_6M;
					vfo[VFO_A].Freq = (u32)(Band_Info[vfo[VFO_A].Band_id].start /sd.tuning_step) * sd.tuning_step;
				}
				/* �߶�Ƶ���޶� */
				if(vfo[VFO_A].Freq >  Band_Info[COUPLING_6M].end  )//
				{
					vfo[VFO_A].Freq = (u32)(Band_Info[COUPLING_6M].end/sd.tuning_step) * sd.tuning_step;
					vfo[VFO_A].Band_id =  COUPLING_6M;
				}
			
				/* �����ʺ�İ��Ƶ�ʵĲ��� */
	//			if(k != vfo[VFO_A].Band_id)
	//			{
	//				vfo[VFO_A].Band_id++;if(vfo[VFO_A].Band_id > COUPLING_6M)vfo[VFO_A].Band_id = COUPLING_80M;/* �����޶� */
	//				if(vfo[VFO_A].Freq >=  Band_Info[vfo[VFO_A].Band_id].start && vfo[VFO_A].Freq <= Band_Info[vfo[VFO_A].Band_id].end )
	//				//vfo[VFO_A].Band_id = k;
	//				k = vfo[VFO_A].Band_id;
	//			}
	//			if(vfo[VFO_A].Freq >=  Band_Info[COUPLING_60M].start && vfo[VFO_A].Freq <= Band_Info[COUPLING_60M].end )
	//			{
	//				if(vfo[VFO_A].Band_id != COUPLING_60M)
	//				vfo[VFO_A].Band_id = COUPLING_60M;
	//			}
				if(mode0==1)
				{
					ks.StepSelected_idx = T_STEP_1KHZ_IDX;//���β���Ĭ������Ϊ1kHz;
					mode0=0;
				}
	//			if(vfo[VFO_A].Band_id == COUPLING_6M)
	//			{
	//				if(sd.Pow >50)sd.Pow =50;
	//			}
				
				/*
				*	��Ƶ����ʾ
				*/
			
				/* �Ͷ�Ƶ���޶� */
				if(vfo[VFO_B].Freq < Band_Info[COUPLING_80M].start)//
				{
					vfo[VFO_B].Freq = Band_Info[COUPLING_80M].start;
					//vfo[VFO_B].Band_id =  COUPLING_80M;
				}
				/* ��Ƶ�ʵͶ˵�г */
				if(vfo[VFO_B].Freq < Band_Info[vfo[VFO_B].Band_id].start && vfo[VFO_B].Freq > Band_Info[vfo[VFO_B].Band_id-1].end)	//�±߽�
				{	
					vfo[VFO_B].Band_id --;
					//if(vfo[VFO_A].Band_id <= COUPLING_80M) vfo[VFO_A].Band_id =  COUPLING_80M; //
					vfo[VFO_B].Freq = (u32)(Band_Info[vfo[VFO_B].Band_id].end/sd.tuning_step) * sd.tuning_step;
				}
				/* ��Ƶ�ʸ߶˵�г */
				if(vfo[VFO_B].Freq > Band_Info[vfo[VFO_B].Band_id].end && vfo[VFO_B].Freq < Band_Info[vfo[VFO_B].Band_id+1].start) 	//�ϱ߽�
				{
					vfo[VFO_B].Band_id  ++;
					//if(ks.Band_id >=  COUPLING_6M) vfo[VFO_A].Band_id =  COUPLING_6M;
					vfo[VFO_B].Freq = (u32)(Band_Info[vfo[VFO_B].Band_id].start /sd.tuning_step) * sd.tuning_step;
				}
				/* �߶�Ƶ���޶� */
				if(vfo[VFO_B].Freq >  Band_Info[COUPLING_6M].end  )//
				{
					vfo[VFO_B].Freq = (u32)(Band_Info[COUPLING_6M].end/sd.tuning_step) * sd.tuning_step;
					vfo[VFO_B].Band_id =  COUPLING_6M;
				}
				/* �����ʺ�İ��Ƶ�ʵĲ��� */
	//			if(i != vfo[VFO_B].Band_id)
	//			{
	//				i ++;
	//				if(i > COUPLING_10M)i = COUPLING_80M;/* �����޶� */
	//				if(vfo[VFO_B].Freq >=  Band_Info[i].start && vfo[VFO_B].Freq <= Band_Info[i].end )
	//				//if(i > COUPLING_10M)i = COUPLING_10M;/* �����޶� */	
	//				vfo[VFO_A].Band_id = i;
	//				i = vfo[VFO_A].Band_id;
	//			}
				
				//if(sd.menu_page==1&&sd.menu>7)sd.Pow =50;
				sd.band = vfo[VFO_A].Band_id; //����IF����
				/*
				*	���ι���ϵ��
				*/
				//ads.cw_alc_gain[vfo[VFO_A].Band_id] = AT24CXX_ReadLenByte(ADDR_CW_POW_GAIN(vfo[VFO_A].Band_id),2);//CW��������ϵ����ȡ
				/*
				*	���ο���
				*/
				Band_Switch = 0x0000;				//����IO���㣬�͵�λ��
				Band_Switch = 1 << vfo[VFO_A].Band_id;	//������ѡ�Ĳ���IOΪ�ߵ�ƽ��
				/*
				*	�����ͨ����
				*/
				switch(vfo[VFO_A].Band_id)
				{
					case COUPLING_80M:{ads.rx_amp_band =0;	Band_bp =9;}break;
					case COUPLING_60M:{ads.rx_amp_band =1;	Band_bp =6;}break;
					case COUPLING_40M:{ads.rx_amp_band =2;	Band_bp =5;}break;
					case COUPLING_30M:{ads.rx_amp_band =3;	Band_bp =4;}break;
					case COUPLING_20M:{ads.rx_amp_band =4;	Band_bp =3;}break;
					case COUPLING_17M:{ads.rx_amp_band =5;	Band_bp =2;}break;
					case COUPLING_15M:{ads.rx_amp_band =6;	Band_bp =2;}break;
					case COUPLING_12M:{ads.rx_amp_band =7;	Band_bp =1;}break;
					case COUPLING_10M:{ads.rx_amp_band =8;	Band_bp =1;}break;
					case COUPLING_6M: {ads.rx_amp_band =9;	Band_bp =0;}break;
				}
				
				/*
				*	RX��λ�ͷ���
				*/
				//ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
				//ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
				/*
				*	���ò��δ�ͨ����IO��ƽ��	
				*/		
				BAND_A0((Band_bp>>0)&0x01);
				BAND_A1((Band_bp>>1)&0x01);
				BAND_A2((Band_bp>>2)&0x01);
				BAND_A3((Band_bp>>3)&0x01);
				
				if(sd.txband_no ==1  && vfo[VFO_A].Band_id==1)ks.tx_no =1;
				else ks.tx_no =0;
				/*
				*	���ò��ε�ͨ����IO��ƽ��
				*/
				if(vfo[VFO_A].Band_id <= COUPLING_6M)//
				{
					LPF_80M((Band_Switch>>0) &0x0001);
					LPF_60_40M(((Band_Switch>>1) &0x0001) | ((Band_Switch>>2) &0x0001));
					LPF_30_20M(((Band_Switch>>3) &0x0001) | ((Band_Switch>>4) &0x0001));
					LPF_17_15M(((Band_Switch>>5) &0x0001) | ((Band_Switch>>6) &0x0001));
					LPF_12_10M(((Band_Switch>>7) &0x0001) | ((Band_Switch>>8) &0x0001));
					LPF_6M((Band_Switch>>9) &0x0001);
				}
			}
			/*
			*	RX��λ�ͷ���
			*/
			ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
			ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
			//
			ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+ads.rx_amp_band, 1);
			if(ads.IF_gain >63)ads.IF_gain =32;
			if(ads.IF_gain < 1)ads.IF_gain =32;
		
			//sd.band = vfo[VFO_A].Band_id; //����IF����
			//ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+vfo[VFO_A].Band_id, 1);
			//if(sd.menu_page)sd.Pow =50;
			//else sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
			//
//			if(sd.menu_page ==1||vfo[VFO_A].Band_id == COUPLING_6M)
//			{
//				sd.Pow =50;
//			}
//			else
//			{
//				sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
//				if(vfo[VFO_A].Band_id == COUPLING_6M)
//				{
//					if(sd.Pow >50)sd.Pow =50;
//				}
//				else
//				{
//					if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
//					{
//						if(sd.Pow >100)sd.Pow =100;
//					}
//					else if(sd.Pow >200)sd.Pow =200;
//				}
//			}
			//
			//pow_gain_set();
			Band_0 = vfo[VFO_A].Band_id ;
			RxBand_0 = vfo[VFO_A].RxBand_id; 
			freq_0 = vfo[VFO_A].Freq;
		}
		Pow_max_set();
		Hp_set();
	}
}
/*
***********************************************************************************
*	���ʲ���ϵ��
***********************************************************************************
*/
Band_power_compensation_coefficient 	power_coefficient[] =
{
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0},
	{100.0,	100.0,	100.0,	100.0}
	
};
void key_wm8978_set(u8 trvolue,u8 gain,u8 alc)
{
	if(trvolue ==0)
	{
		WM8978_ALC_Cfg(0,15);
		if(vfo[VFO_A].Mode < DEMOD_LSB)WM8978_AUXIN_Cfg(7);
		else WM8978_AUXIN_Cfg(0);	
		WM8978_Input_Cfg(1,1);//����ͨ��(MIC&LINE IN)
		WM8978_Output_Cfg(0,0);		//����BYPASS��� 
		WM8978_MIC_Gain(gain&0x3f);//MIC��������,�������ALC���棬��������ָ���������
		WM8978_Inpga_muter(0,0);
		WM8978_AUX_Gain(0,0);
		WM8978_LINEIN_Gain(0,0);
	}
	if(trvolue ==1)
	{
		//WM8978_MIC_Gain(15);
		//WM8978_Output_Cfg(1,1);
		//WM8978_AUXIN_Cfg(0);
	}
	if(trvolue ==2)
	{
		//WM8978_MIC_Gain(gain&0x3f);
		//WM8978_ALC_Cfg(3,alc);
		//WM8978_Output_Cfg(1,1);		//����BYPASS���
		if(ks.cat_ptt ==TX)
		{
			WM8978_Input_Cfg(2,2);
			WM8978_Inpga_muter(1,1);
			if(ks.bt_key )
			{
				//WM8978_ALC_Cfg(0,alc);
				//WM8978_MIC_Gain(gain&0x3f);
				//WM8978_Input_Cfg(1,0);//����ͨ��(BLE&R_ IN)
				WM8978_LINEIN_Gain(0,7);
			}
			else
			{	
				//WM8978_Input_Cfg(1,1);
				//WM8978_Inpga_muter(1,1);
				WM8978_AUX_Gain(7,0);
			}
		}
		//else
		//if(ks.rit_tx ==TX)WM8978_Input_Cfg(0,1);//����ͨ��(MIC&L IN)
		else 
		{
			WM8978_ALC_Cfg(3,alc);
			WM8978_MIC_Gain(gain&0x3f);
			
			WM8978_Input_Cfg(0,1);//����ͨ��(MIC&L IN)	
		}
	}
}
/*
**********************************************************************************
*	����ģʽ�ٿع���
*	Modulation mode control management
**********************************************************************************
*/
CW_WPM_management	wpm_tab[46]=
{
	{" 5",	255},//0
	{" 6",	217},//1
	{" 7",	187},//2
	{" 8",	163},//3
	{" 9",	144},//4
	{"10",	130},//5
	{"11",	118},//6
	{"12",	108},//7
	{"13",	100},//8
	{"14",	 93},//9
	{"15",	 86},//10
	{"16",	 81},//11
	{"17",	 76},//12
	{"18",	 72},//13
	{"19",	 68},//14
	{"20",	 65},//15
	{"21",	 65},//16
	{"22",	 62},//17
	{"23",	 59},//18
	{"24",	 56},//19
	{"25",	 54},//20
	{"26",	 50},//21
	{"27",	 48},//22
	{"28",	 46},//23
	{"29",	 45},//24
	{"30",	 44},//25
	{"31",	 42},//26
	{"32",	 41},//27
	{"33",	 39},//28
	{"34",	 38},//29
	{"35",	 37},//30
	{"36",	 36},//31
	{"37",	 35},//32
	{"38",	 34},//33
	{"39",	 33},//34
	{"40",	 32},//35
	{"41",	 32},//36
	{"42",	 31},//37
	{"43",	 30},//38
	{"44",	 30},//39
	{"45",	 29},//40
	{"46",	 28},//41
	{"47",	 28},//42
	{"48",	 27},//43
	{"49",	 27},//44
	{"50",	 26},//45
};

/*
*
*/
void key_Mode_Control()
{
	static u8 clk =0;
	static u8 mode_0=0;
	static u8 ssb_mode=1;
	static u8 fil_0;
	static u8 ptt;
	static u8 cw_mode0;
	static u8 wm8978_tr_set;
	static u16 dsp_IF1;
	static u16 cw_si;
	static u16 bfo;
	static int16_t rit_0;
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
	{

		/*
		*	����ģʽѡ�� 
		*/
		if( ucKeyCode==KEY_1_UP )
		{	
			ads.tx_delay =100;
//			vfo[VFO_A].Mode ++;
//			if(vfo[VFO_A].Mode > DEMOD_USB_DIG)vfo[VFO_A].Mode  =  DEMOD_LSB;
//			else
//			if(vfo[VFO_A].Mode < DEMOD_LSB)vfo[VFO_A].Mode  =  DEMOD_USB_DIG;
//			else
//			if(vfo[VFO_A].Mode > DEMOD_LSB && vfo[VFO_A].Mode < DEMOD_USB)vfo[VFO_A].Mode  =  DEMOD_LSB_DIG;
//			else
//			if(vfo[VFO_A].Mode > DEMOD_LSB_DIG && vfo[VFO_A].Mode < DEMOD_USB_DIG)vfo[VFO_A].Mode  =  DEMOD_USB;
//			else	
//			if(vfo[VFO_A].Mode > DEMOD_USB && vfo[VFO_A].Mode <= DEMOD_USB_DIG)vfo[VFO_A].Mode  =  DEMOD_USB_DIG;
			
			vfo[VFO_A].Mode +=2;
			if(vfo[VFO_A].Mode > DEMOD_USB)vfo[VFO_A].Mode  =  DEMOD_LSB;
			if(vfo[VFO_A].Mode < DEMOD_LSB)vfo[VFO_A].Mode  =  DEMOD_USB;
            if(ssb_mode==1)
            {
                if(vfo[VFO_A].Freq >9500000)vfo[VFO_A].Mode = DEMOD_USB;
                else vfo[VFO_A].Mode = DEMOD_LSB;
                ssb_mode =0;
            }
		}
		//else
		if( ucKeyCode==KEY_3_UP)
		{
			ads.tx_delay =100;
			vfo[VFO_A].Mode  = cw_mode0;
			//vfo[VFO_A].Mode ++;
			if(vfo[VFO_A].Mode  >  DEMOD_CW_USB)vfo[VFO_A].Mode  =  DEMOD_CW_LSB;
			else
			if(vfo[VFO_A].Mode  <  DEMOD_CW_LSB)vfo[VFO_A].Mode  =  DEMOD_CW_USB;
			if( ++ks.CW_keySW>1 )
			{
				ks.CW_keySW = 0;
			}
            ssb_mode =1;
		}
		//else
		if( ucKeyCode==KEY_4_UP)
		{
			ads.tx_delay =100;
			//AF_SQL(1);/* ��Ƶ���� */
			//ks.NOT_key =0;
			//ks.NR_key =0;
			vfo[VFO_A].Mode ++;
			if(vfo[VFO_A].Mode  >  DEMOD_FM)vfo[VFO_A].Mode  =  DEMOD_AM;
			else
			if(vfo[VFO_A].Mode  <  DEMOD_AM)vfo[VFO_A].Mode  =  DEMOD_FM;
			//delay_ms(200);
			//AF_SQL(0);/* ��Ƶ���� */
			ssb_mode =1;
		}
		//else 
		/* CW�Զ���/�ֶ�ת�� */
		if(ucKeyCode==KEY_3_LONG && vfo[VFO_A].Mode < DEMOD_LSB && TR_READ == CONTROL_RX )
		{
			//if( ++ks.CW_keySW>1 )
			//{
			//	ks.CW_keySW = 0;
			//}
			ks.CW_AB =~ks.CW_AB&0x01;
		}
		/* CW�Զ���/�ֶ�����ϰģʽ */
		if(ucKeyCode==KEY_1_LONG && vfo[VFO_A].Mode < DEMOD_LSB && TR_READ == CONTROL_RX)
		{
			ads.tx_delay =100;
			//if(++ks.CW_Exercise >1)
			//ks.CW_Exercise =0;
			vfo[VFO_A].Mode ++;
			if(vfo[VFO_A].Mode  >  DEMOD_CW_USB)vfo[VFO_A].Mode  =  DEMOD_CW_LSB;
			else
			if(vfo[VFO_A].Mode  <  DEMOD_CW_LSB)vfo[VFO_A].Mode  =  DEMOD_CW_USB;
			cw_mode0 = vfo[VFO_A].Mode;
		}
		/* CW�Զ���A/Bת�� */
		if( ucKeyCode==KEY_5_LONG && vfo[VFO_A].Mode < DEMOD_LSB && TR_READ == CONTROL_RX )
		{
			//ks.CW_AB =~ks.CW_AB&0x01;
			//vfo[VFO_A].Mode ++;
			//if(vfo[VFO_A].Mode  >  DEMOD_CW_USB)vfo[VFO_A].Mode  =  DEMOD_CW_LSB;
			//else
			//if(vfo[VFO_A].Mode  <  DEMOD_CW_LSB)vfo[VFO_A].Mode  =  DEMOD_CW_USB;
			//cw_mode0 = vfo[VFO_A].Mode;
			if(++ks.CW_Exercise >1)
			ks.CW_Exercise =0;
		}
	}
/*
*******************************************************************************************************
*
*********************************************************************************************************
*/
	
	if(dsp_IF1 != sd.IF_1 || bfo != Filte_Center[vfo[VFO_A].FilterSelected_id] || cw_si != sd.CW_Sidetone)
	{
		clk=0;
		dsp_IF1 = sd.IF_1;
		cw_si = sd.CW_Sidetone;
		bfo = Filte_Center[vfo[VFO_A].FilterSelected_id];
	}
	if(mode_0 != vfo[VFO_A].Mode  || fil_0 != vfo[VFO_A].FilterSelected_id || clk ==0 || rit_0!=sd.rit ||ptt != TR_READ)
	{
		//pow_gain_set();//�趨�����¸������ε�����ϵ��
		if(rit_0==sd.rit)
		{
			if(TR_READ ==CONTROL_TX)
			{
				//ads.dsp_IF = sd.IF_1;
				if(vfo[VFO_A].Mode < DEMOD_LSB)wm8978_tr_set=1;
				else wm8978_tr_set=2;
				key_wm8978_set(wm8978_tr_set,sd.alc_mic &0x3f,sd.alc_start);
			}
			else 
			{
				//ads.dsp_IF = sd.IF_1-sd.rit;
				wm8978_tr_set=0;
				key_wm8978_set(wm8978_tr_set,ads.IF_gain&0x3f,sd.alc_start);
			}
		}
		if(vfo[VFO_A].Mode < DEMOD_LSB)
		{
			sd.Dsp_Bfo =sd.CW_Sidetone;/* ����BFO��ƵƵ�� */
			
			sd.Dsp_Lo1 = sd.IF_1;
			ads.dsp_IF = sd.IF_1;
			
			ads.cw_spe = 1300/sd.CW_Speed ;
			softdds_setfreq(DDS_SI,(float32_t)sd.Dsp_Bfo, SAMPLING_RETE, 1);
			vfo[VFO_A].FilterSelected_id = sd.cw_fil_id;
		}
		//else
		if(vfo[VFO_A].Mode==DEMOD_LSB || vfo[VFO_A].Mode==DEMOD_USB)/**/
		{
		
			sd.Dsp_Bfo = Filte_Center[vfo[VFO_A].FilterSelected_id];

			if(vfo[VFO_A].Mode == DEMOD_LSB)
			{
				sd.Dsp_Lo1 = sd.IF_1 - sd.Dsp_Bfo;
				ads.dsp_IF = sd.IF_1;
			}
			else
			if(vfo[VFO_A].Mode == DEMOD_USB)
			{
				sd.Dsp_Lo1 = sd.IF_1 + sd.Dsp_Bfo;
				ads.dsp_IF = sd.IF_1; 
			}
		}
		//else
		if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode == DEMOD_USB_DIG)
		{
			
			sd.Dsp_Bfo = Filte_Center[vfo[VFO_A].FilterSelected_id];

			if(vfo[VFO_A].Mode == DEMOD_LSB_DIG)
			{
				sd.Dsp_Lo1 = sd.IF_1 - sd.Dsp_Bfo;
				ads.dsp_IF = sd.IF_1;
				ui_TopBackground(165,4,24,16);
				LCD_ShowString(165,4,GRAY1,BLUE2 ,16,0,"DIG");
			}
			else
			{
				sd.Dsp_Lo1 = sd.IF_1 + sd.Dsp_Bfo;
				ads.dsp_IF = sd.IF_1;
				ui_TopBackground(165,4,24,16);
				LCD_ShowString(165,4,GRAY1,BLUE2 ,16,0,"DIG");
			}
			//ads.spk_vol =0;
			//ads.hp_vol =60;
		}			
		//else
		if(vfo[VFO_A].Mode > DEMOD_USB_DIG)
		{
			sd.Dsp_Lo1 = sd.IF_1;
			ads.dsp_IF = sd.IF_1;
			
			if(vfo[VFO_A].Mode == DEMOD_AM)vfo[VFO_A].FilterSelected_id = sd.am_fil_id;
			else
            if(vfo[VFO_A].Mode == DEMOD_FM)vfo[VFO_A].FilterSelected_id = sd.fm_fil_id;
			//ads.spk_vol =63;
		}
		
		if(vfo[VFO_A].Mode != DEMOD_USB_DIG && vfo[VFO_A].Mode != DEMOD_LSB_DIG )
		{
			//ads.spk_vol =63;
			LCD_ShowString(165,4,GRAY0,BLUE2 ,16,1,"   ");
            ui_TopBackground(165,4,24,16);
		}
			
		if(mode_0 != vfo[VFO_A].Mode || clk ==0 )
		{
			LCD_RectangleWindow(1, 25, 52, 20, GRAY1);/* ���ڱ߿� */
			LCD_ShowString(3, 27, GRAY0, RED , 16, 0, text.Mode [vfo[VFO_A].Mode] );
			
		}
		/* ������޶� */
//		if(sd.menu_page ==1)
//		{
//			sd.Pow =50;
//		}
//		else
//		{
//			sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
//			if(vfo[VFO_A].Band_id == COUPLING_6M)
//			{
//				if(sd.Pow >50)sd.Pow =50;
//			}
//			else
//			{
//				if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
//				{
//					if(sd.Pow >100)sd.Pow =100;
//				}
//				else if(sd.Pow >200)sd.Pow =200;
//			}
//		}
//		//AT24CXX_WriteOneByte(MENU_POW, sd.Pow );
		//pow_gain_set();
		
		softdds_setfreq(DDS_TX_LO,(float32_t)(sd.Dsp_Lo1), SAMPLING_RETE, 1);
		softdds_setfreq(DDS_LO1,(float32_t)(sd.Dsp_Lo1+sd.rit), SAMPLING_RETE, 1);
		softdds_setfreq(DDS_BFO,(float32_t)sd.Dsp_Bfo, SAMPLING_RETE / 8, 1);
		mode_0 = vfo[VFO_A].Mode ;
		fil_0 = vfo[VFO_A].FilterSelected_id;
		ptt = TR_READ;
		if(clk ==0)clk =1;
		rit_0=sd.rit;
	}
}

/*
**********************************************************************************
*	�˲����ٿع���
**********************************************************************************
*/

//typedef struct
//{
//	u16 min;
//	u16 Default;
//	u16 max;
//}Filte_Center_TypeDef;
Filte_Center_TypeDef	filte_center[6]=
{
	{850,	1100,	2000},//1.5k
	{1000,	1200,	2000},//1.8k
	{1150,	1300,	2100},//2.1k
	{1300,	1400,	2100},//2.4k
	{1450,	1500,	2200},//2.7k
	{1600,	1600,	2200},//3.0k
};
uint16_t Filte_Center[6];
void key_Filter_Control(u8 idx)
{
	u8 i;
	static int8_t mode;
	static u8 init_fil =0;
	static u8 ptt;
	static u8 Filter_idx;
	static u8 idx_0;
	static u8 ks_mode0;
	//static u16 bfo;
	
	if(idx_0 != vfo[VFO_A].FilterSelected_id )// ||bfo!=Filte_Center[vfo[VFO_A].FilterSelected_id] )
	{
		init_fil=0;
		ads.tx_delay=100;
		//bfo=Filte_Center[vfo[VFO_A].FilterSelected_id];
		idx_0 = vfo[VFO_A].FilterSelected_id;
	}
	
//	if(ptt!=PTT_RT )
//	{
//		//if(vfo[VFO_A].Mode >  DEMOD_CW_USB)Adc3_Channel_Selection( PTT_RT);//����ʱת������˷�ӿ�
//		if(vfo[VFO_A].Mode==DEMOD_LSB || vfo[VFO_A].Mode==DEMOD_USB)/**/
//		{
//			if(PTT_RT == TX)vfo[VFO_A].FilterSelected_id = sd.tx_filter ;
//			else
//			{	
//				if(vfo[VFO_A].Mode==DEMOD_LSB)vfo[VFO_A].FilterSelected_id = sd.lsb_fil_id;
//				if(vfo[VFO_A].Mode==DEMOD_USB)vfo[VFO_A].FilterSelected_id = sd.usb_fil_id;
//			}
//		}
//		//ptt=PTT_RT;
//	}
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
	{
		//Filter �˲���ѡ��
		if( ucKeyCode==KEY_10_UP)
		{
			ads.tx_delay =100;
			vfo[VFO_A].FilterSelected_id ++;
			if(vfo[VFO_A].Mode < DEMOD_AM)
			{
				if(vfo[VFO_A].FilterSelected_id > 5 ) vfo[VFO_A].FilterSelected_id =0;
			}
			else
			{
				if(vfo[VFO_A].FilterSelected_id > 1 ) vfo[VFO_A].FilterSelected_id =0;
			}
			//
			if(vfo[VFO_A].Mode < DEMOD_LSB)
			{
				switch(vfo[VFO_A].FilterSelected_id )
				{
					case 0:  Filter_idx = CW_50HZ ; break;
					case 1:  Filter_idx = CW_100HZ; break;
					case 2:  Filter_idx = CW_200HZ ;break;
					case 3:  Filter_idx = CW_300HZ ;break;
					case 4:  Filter_idx = CW_500HZ ;break;
					case 5:  Filter_idx = CW_800HZ ;break;
				}
				//if(vfo[VFO_A].FilterSelected_id > 5 ) vfo[VFO_A].FilterSelected_id =0;
				//sd.Dsp_Bfo = sd.CW_Sidetone;/* ����BFO��ƵƵ�� */
				sd.cw_fil_id = vfo[VFO_A].FilterSelected_id;
				AT24CXX_WriteOneByte(ADDR_FIL_CW, sd.cw_fil_id );
				 
			}
			//else
			if(vfo[VFO_A].Mode ==DEMOD_LSB||vfo[VFO_A].Mode ==DEMOD_USB)
			{
				if(vfo[VFO_A].Mode ==DEMOD_LSB)
				{
					
					sd.lsb_fil_id = vfo[VFO_A].FilterSelected_id;
					AT24CXX_WriteOneByte(ADDR_FIL_LSB, sd.lsb_fil_id );
				}
				//else 
				if(vfo[VFO_A].Mode ==DEMOD_USB)
				{
					sd.usb_fil_id = vfo[VFO_A].FilterSelected_id;
					AT24CXX_WriteOneByte(ADDR_FIL_USB, sd.usb_fil_id );
				}
				switch(vfo[VFO_A].FilterSelected_id)
				{
					case 0:  {Filter_idx = SSB_1K5 ;/*	sd.Dsp_Bfo = SSB_BFO_1100;*/}break;
					case 1:  {Filter_idx = SSB_1K8 ;/*	sd.Dsp_Bfo = SSB_BFO_1200;*/}break;
					case 2:  {Filter_idx = SSB_2K1 ;/*	sd.Dsp_Bfo = SSB_BFO_1300;*/}break;
					case 3:  {Filter_idx = SSB_2K4 ;/*	sd.Dsp_Bfo = SSB_BFO_1400;*/}break;
					case 4:  {Filter_idx = SSB_2K7 ;/*	sd.Dsp_Bfo = SSB_BFO_1500;*/}break;
					case 5:  {Filter_idx = SSB_3K  ;/*	sd.Dsp_Bfo = SSB_BFO_1600;*/}break;
				}
				if(vfo[VFO_A].FilterSelected_id > 5 ) vfo[VFO_A].FilterSelected_id =0;
				//sd.Dsp_Bfo = Filte_Center[Filter_idx];
			}
			//else
			if(vfo[VFO_A].Mode ==DEMOD_USB_DIG || vfo[VFO_A].Mode ==DEMOD_LSB_DIG)
			{
				Filter_idx = SSB_3K;
				if(vfo[VFO_A].FilterSelected_id != 5 ) vfo[VFO_A].FilterSelected_id =5;
			}			
			//else
			if(vfo[VFO_A].Mode ==DEMOD_AM)
			{
				switch(vfo[VFO_A].FilterSelected_id)
				{
					case 0:  Filter_idx = AM_6K   ;break;
					case 1:  Filter_idx = AM_9K   ;break;
					//case 2:  Filter_idx = AM_8K   ;break;
					//case 3:  Filter_idx = AM_9K   ;break;
				}
				if(vfo[VFO_A].FilterSelected_id > 1 ) vfo[VFO_A].FilterSelected_id =0;
				sd.am_fil_id =  vfo[VFO_A].FilterSelected_id;
				AT24CXX_WriteOneByte(ADDR_FIL_AM, sd.am_fil_id ); 
				
			}
			//else
			if(vfo[VFO_A].Mode ==DEMOD_FM)
			{
				switch(vfo[VFO_A].FilterSelected_id)
				{
					case 0:  { Filter_idx = FM_5K;  sd.fm_offset = FM_OFFSET_2K5; }break;
					case 1:  { Filter_idx = FM_10K; sd.fm_offset = FM_OFFSET_5K; }break;
					//case 2:  Filter_idx = FM_5K  ;break;
					//case 3:  Filter_idx = FM_10K   ;break;
				}
				if(vfo[VFO_A].FilterSelected_id > 1 ) vfo[VFO_A].FilterSelected_id =0;
				sd.fm_fil_id = vfo[VFO_A].FilterSelected_id;
				AT24CXX_WriteOneByte(ADDR_FIL_FM, sd.fm_fil_id  );
				
			}
		}
	}
	//
	if( ks_mode0 !=vfo[VFO_A].Mode || init_fil ==0 ||ptt != TR_READ)
	{
		if(vfo[VFO_A].Mode <DEMOD_LSB)
		{
			switch(vfo[VFO_A].FilterSelected_id )
			{
				case 0:  Filter_idx = CW_50HZ ; break;
				case 1:  Filter_idx = CW_100HZ; break;
				case 2:  Filter_idx = CW_200HZ ;break;
				case 3:  Filter_idx = CW_300HZ ;break;
				case 4:  Filter_idx = CW_500HZ ;break;
				case 5:  Filter_idx = CW_800HZ ;break;
			}
			//sd.Dsp_Bfo = sd.CW_Sidetone;/* ����BFO��ƵƵ�� */
			if(vfo[VFO_A].FilterSelected_id > 5 ) vfo[VFO_A].FilterSelected_id =0;
		}
		//else
		if(vfo[VFO_A].Mode ==DEMOD_LSB||vfo[VFO_A].Mode ==DEMOD_USB)
		{
			if(vfo[VFO_A].Mode ==DEMOD_LSB)
            {
                if(TR_READ == CONTROL_RX)vfo[VFO_A].FilterSelected_id = sd.lsb_fil_id;
				else	vfo[VFO_A].FilterSelected_id = sd.tx_filter ;
            }
			//else 
            if(vfo[VFO_A].Mode ==DEMOD_USB)
            {
                if(TR_READ == CONTROL_RX)vfo[VFO_A].FilterSelected_id = sd.usb_fil_id;
				else	vfo[VFO_A].FilterSelected_id = sd.tx_filter ;
            }
			switch(vfo[VFO_A].FilterSelected_id)
			{
//				case 0:  {Filter_idx = SSB_1K5 ;sd.Dsp_Bfo = Filte_Center[0];}break;
//				case 1:  {Filter_idx = SSB_1K8 ;sd.Dsp_Bfo = Filte_Center[1];}break;
//				case 2:  {Filter_idx = SSB_2K1 ;sd.Dsp_Bfo = Filte_Center[2];}break;
//				case 3:  {Filter_idx = SSB_2K4 ;sd.Dsp_Bfo = Filte_Center[3];}break;
//				case 4:  {Filter_idx = SSB_2K7 ;sd.Dsp_Bfo = Filte_Center[4];}break;
//				case 5:  {Filter_idx = SSB_3K  ;sd.Dsp_Bfo = Filte_Center[5];}break;
				
				case 0:  Filter_idx = SSB_1K5 ;/*	sd.Dsp_Bfo = SSB_BFO_1100;*/break;
				case 1:  Filter_idx = SSB_1K8 ;/*	sd.Dsp_Bfo = SSB_BFO_1200;*/break;
				case 2:  Filter_idx = SSB_2K1 ;/*	sd.Dsp_Bfo = SSB_BFO_1300;*/break;
				case 3:  Filter_idx = SSB_2K4 ;/*	sd.Dsp_Bfo = SSB_BFO_1400;*/break;
				case 4:  Filter_idx = SSB_2K7 ;/*	sd.Dsp_Bfo = SSB_BFO_1500;*/break;
				case 5:  Filter_idx = SSB_3K  ;/*	sd.Dsp_Bfo = SSB_BFO_1600;*/break;
			}
			if(vfo[VFO_A].FilterSelected_id > 5 ) vfo[VFO_A].FilterSelected_id =0;
			
//			if(Filte_Center[vfo[VFO_A].FilterSelected_id] < filte_center[vfo[VFO_A].FilterSelected_id].min )
//				Filte_Center[vfo[VFO_A].FilterSelected_id] = filte_center[vfo[VFO_A].FilterSelected_id].Default	;
//			if(Filte_Center[vfo[VFO_A].FilterSelected_id] > filte_center[vfo[VFO_A].FilterSelected_id].max )
//				Filte_Center[vfo[VFO_A].FilterSelected_id] = filte_center[vfo[VFO_A].FilterSelected_id].Default ;
			//sd.Dsp_Bfo = Filte_Center[vfo[VFO_A].FilterSelected_id];
		}
		//else
		if(vfo[VFO_A].Mode ==DEMOD_USB_DIG || vfo[VFO_A].Mode ==DEMOD_LSB_DIG)
		{
			Filter_idx = SSB_3K;
			if(vfo[VFO_A].FilterSelected_id != 5 ) vfo[VFO_A].FilterSelected_id =5;
		}			
		//else
		if(vfo[VFO_A].Mode ==DEMOD_AM)
		{
			switch(vfo[VFO_A].FilterSelected_id)
			{
				case 0:  Filter_idx = AM_6K   ;break;
				case 1:  Filter_idx = AM_9K   ;break;
				//case 2:  Filter_idx = AM_8K   ;break;
				//case 3:  Filter_idx = AM_9K   ;break;
			}
			if(vfo[VFO_A].FilterSelected_id > 1 ) vfo[VFO_A].FilterSelected_id =0;
			sd.am_fil_id =  vfo[VFO_A].FilterSelected_id;
           // AT24CXX_WriteOneByte(ADDR_FIL_AM, sd.am_fil_id ); 
            
		}
		//else
		if(vfo[VFO_A].Mode ==DEMOD_FM)
		{
			if(vfo[VFO_A].FilterSelected_id > 1 ) vfo[VFO_A].FilterSelected_id =0;
			sd.fm_fil_id =   vfo[VFO_A].FilterSelected_id;
			switch(vfo[VFO_A].FilterSelected_id)
			{
				case 0:  { Filter_idx = FM_5K;  sd.fm_offset = FM_OFFSET_2K5; ads.fm_gain =FM_GAIN_2K5;}break;
				case 1:  { Filter_idx = FM_10K; sd.fm_offset = FM_OFFSET_5K; ads.fm_gain =FM_GAIN_5K;}break;
				//case 2:  Filter_idx = FM_5K  ;break;
				//case 3:  Filter_idx = FM_10K   ;break;
			}
		}
		
		sd.spe_fil_id = Filter_idx;
		/* AF��Ƶ��ͨ�˲� */
		for(i=0;i<IIR_NUM_TAPS;i++)
		{
			AF_State[i]=0;
			Riss_State[i] =0;
		}
		IIR_AF.pState=(float32_t *)& AF_State;
		IIR_AF.numStages = FilterPathInfo[ Filter_idx ].AF_instance->numStages;
		IIR_AF.pCoeffs = FilterPathInfo[ Filter_idx ].AF_instance->pCoeffs;
		//AF_Values = FilterPathInfo[ Filter_idx ]. AF_ScaleValues;
		//if(vfo[VFO_A].Mode <DEMOD_AM)
		//{
			RISS.pState=(float32_t *)& Riss_State;
			RISS.numStages = RISS_BPF.numStages;
			RISS.pCoeffs = RISS_BPF.pCoeffs;
//		}
//		else
//		{
//			RISS.pState=(float32_t *)& Riss_State;
//			RISS.numStages =IIR_dfI_100hz_LPF.numStages;
//			RISS.pCoeffs = IIR_dfI_100hz_LPF.pCoeffs;
//		}
		/* ת�������˲��� */
		for(i=0;i<IIR_NUM_TAPS;i++)
		{
			IIRStateF32_I[i]=0;
			IIRStateF32_Q[i]=0;
		}
		IIR_LPF_I.pState=(float32_t *)&IIRStateF32_I;
		IIR_LPF_Q.pState=(float32_t *)&IIRStateF32_Q;
		IIR_LPF_I.numStages = FilterPathInfo[ Filter_idx ].LP_instance->numStages;
		IIR_LPF_Q.numStages = FilterPathInfo[ Filter_idx ].LP_instance->numStages;
	
		IIR_LPF_I.pCoeffs = FilterPathInfo[ Filter_idx ].LP_instance->pCoeffs;
		IIR_LPF_Q.pCoeffs = FilterPathInfo[ Filter_idx ].LP_instance->pCoeffs;
		
		IIR_LP_Values = FilterPathInfo[ Filter_idx ].Lpf_ScaleValues;
		/*
		*	ת����ȡ���ڲ��˲���
		*/
		ads.Rate = FilterPathInfo[ Filter_idx ].sample_rate;
		
		DECIMATE_I.numTaps = FilterPathInfo[ Filter_idx ].Decimate_LP->numTaps;
		DECIMATE_Q.numTaps = FilterPathInfo[Filter_idx].Decimate_LP->numTaps;
		INTERPOLATE_I.phaseLength = FilterPathInfo[Filter_idx].Interpolate_LP->phaseLength/ads.Rate;
		INTERPOLATE_Q.phaseLength = FilterPathInfo[Filter_idx].Interpolate_LP->phaseLength/ads.Rate;
		
		DECIMATE_I.pCoeffs = FilterPathInfo[Filter_idx].Decimate_LP->pCoeffs;
		DECIMATE_Q.pCoeffs = FilterPathInfo[Filter_idx].Decimate_LP->pCoeffs;
		INTERPOLATE_I.pCoeffs = FilterPathInfo[Filter_idx].Interpolate_LP->pCoeffs;
		INTERPOLATE_Q.pCoeffs = FilterPathInfo[Filter_idx].Interpolate_LP->pCoeffs;
		
		DECIMATE_I.M = ads.Rate;
		DECIMATE_Q.M = ads.Rate;
		INTERPOLATE_I.L = ads.Rate;
		INTERPOLATE_Q.L = ads.Rate;
		
		DECIMATE_I.pState = (float32_t *)&DecimState_I[0];
		DECIMATE_Q.pState = (float32_t *)&DecimState_Q[0];
		INTERPOLATE_I.pState = (float32_t *)&InterpState_I[0];
		INTERPOLATE_Q.pState = (float32_t *)&InterpState_Q[0];
		
		for(i=0;i<FIR_NUM_TAPS+FIR_BLOCK_SIZE-1;i++)
		{
			DecimState_I[i]=0;
			DecimState_Q[i]=0;
			InterpState_I[i]=0;
			InterpState_Q[i]=0;
		}
		//AT24CXX_WriteOneByte(7,k_num.FIL_K7);
		//Lcd_Color(0,0,219,18,BLUE);
		if(vfo[VFO_A].Mode  ==  DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_LSB_DIG)mode = 1;
		else
		if(vfo[VFO_A].Mode  ==  DEMOD_USB || vfo[VFO_A].Mode == DEMOD_USB_DIG)mode =-1;	
		else
			mode = 0;
		sd.spe_fil_mode = mode;
		//ui_Spe_Filter_Display(Filter_idx,mode,clk,ks.F_11);
		/* �˲�����ʾ */
		ui_Filter_Display (284,25,RED,GRAY5,Filter_idx,mode);
		
		//idx_0 = vfo[VFO_A].FilterSelected_id;
		ks_mode0 =vfo[VFO_A].Mode;
		ptt = TR_READ;
		if(init_fil ==0)init_fil =1;
	}
}
/*
**********************************************************************************
*	Ƶ�ʵ�г�����ٿ�
**********************************************************************************
*/
// Tuning steps
const u32 tune_steps[T_STEP_MAX_STEPS] =
{
    T_STEP_1HZ,
    T_STEP_10HZ,
    T_STEP_100HZ,
    T_STEP_1KHZ,
	T_STEP_10KHZ,
    T_STEP_100KHZ,
    T_STEP_1MHZ,
	T_STEP_5KHZ,
	T_STEP_9KHZ
};
void key_Step_Control(u8 step_idx)
{
	static u8 clk =0;
	static u8 step_idx0;
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0&& PIN_K13 ==1)//�а����������������
	{
		/*
		*	��г����ѡ��
		*/
		if( ucKeyCode==KEY_13_DOWN && PIN_K13 ==1)//Ƶ����ť�̰�����г����ѡ��
		{
			ks.StepSelected_idx ++;
//			if(ks.menu_key==0 && ks.rt_rx ==1 && ks.RxBand_id == RADIO_MW)/* ���ջ�ģʽ��MW���β���Ϊ9KHz */
//			{
//				ks.StepSelected_idx = T_STEP_9KHZ_IDX;
//			}
//			else
			if(ks.menu_key==1)/* �˵������²���1->1MHZ */
			{
				if(ks.StepSelected_idx > T_STEP_1MHZ_IDX) 
					ks.StepSelected_idx=0;					
			}
			else/* ��������ģʽ�²���10->100KHZ*/
			{
				if(ks.StepSelected_idx > T_STEP_100KHZ_IDX) 
					ks.StepSelected_idx=0;
			}	
		}
	}
	//
	if((step_idx0 != step_idx || clk ==0)&&ks.menu_key==0)
	{
		sd.tuning_step = tune_steps[step_idx];
		if(ks.StepSelected_idx <= T_STEP_1KHZ_IDX)
		{			
			vfo[VFO_A].Freq /= sd.tuning_step;
			vfo[VFO_A].Freq *= sd.tuning_step;
		}
		ui_Step_Display(68, 22, step_idx);
		step_idx0 = step_idx;
		clk =1;
	}
	else
	if((step_idx0 != step_idx || clk ==0)&&ks.menu_key==1)
	{
		sd.tuning_step = tune_steps[step_idx];
		//ui_Step_Display(68, 22, step_idx);
		step_idx0 = step_idx;
		clk =1;
	}	
}
/*
**********************************************************************************
*	AGC ����
**********************************************************************************
*/
AGC_Recovery_Time AGC_Time[]=
{
	{"S ",	45},
	{"M ",	15},
	{"F ",	5},
};
void Key_AGC_RecoveryTime_set()//AGC �ָ�ʱ�����
{
	static u8 clk=0;
	static u8 agc_ma0;
	if(agc_ma0 != ks.AGC_Constant)
	{
		clk=0;
		agc_ma0 = ks.AGC_Constant;
	}
	if(clk ==0 || (ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0 && ks.FunctionWindow_Use ==0))//�а����������������
	{
		//*ATT����
		if( sd.ritatt_ptt !=2 )
		{
			
			if(ucKeyCode==KEY_7_LONG)//[ATT]���̰�
			{
				ks.agc_m_adjust =~ks.agc_m_adjust&0x01;
				if(ks.agc_m_adjust ==0)sd.Enc2_delay =0;
			}
		}
//			{
//				ks.agc_m_adjust =0;
//				if(ucKeyCode==KEY_7_UP)ks.AGC_Constant++;//[ATT]���̰�
//				
//				if(ks.AGC_Constant>2)ks.AGC_Constant =0;
//			}
		ads.agc_constant = AGC_Time[ks.AGC_Constant].Default ;
		ui_TopBackground(200,4,16,16);
		LCD_ShowString(200, 4, GRAY0, RED , 16, 1, AGC_Time[ks.AGC_Constant].text  );
		clk =1;
	
	}
}
/*
**********************************************************************************
*	�߷�AMP
**********************************************************************************
*/
void key_ampset()
{
	if(TR_READ ==CONTROL_RX)
	{
		if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
		{
			/*	�߷�AMP */
			if(ks.menu_key == MENU_OFF && ucKeyCode==KEY_9_UP)
			{
				sd.amp_set =~sd.amp_set&0x01;
			}
			if(sd.amp_set ==1)AMP_ON;
			else AMP_OFF;
		}
	}		
}
/*
**********************************************************************************
*	ATT ����˥������
**********************************************************************************
*/
void key_ATT_Control()
{
	static u8 k50;
	static u8 ptt;
	static u8 clk=0;
	static u8 ATT_Key0;
	//static u8 k5,k7;
	if( sd.ritatt_ptt !=2 )
	{
		//PTT_IN();
		if(ks.key_lock == KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
		{
			if(ucKeyCode==KEY_7_UP)
			{
				ks.ATT_key ++;		//ATT���أ�0��������ATT, 1������ATT.
				if(ks.ATT_key>2)ks.ATT_key=0;
			}
		}
	}
	if(ucKeyCode != KEY_NONE ||k50!=PIN_K5)
	{
		if(sd.ritatt_ptt ==1)
		{
			if( PIN_K5==1)
			{
				//ks.cat_ptt =RX;//RX
				ks.rit_tx =1;
			}
			else
			if(PIN_K5==0 )
			{
				//ks.cat_ptt =TX;//TX
				ks.rit_tx =0;
			}
			k50 =PIN_K5;			
		}
		//if(sd.ritatt_ptt ==2)
		//{
		//	if( PIN_K7==1)ks.cat_ptt =RX;//RX
		//	else
		//	if(PIN_K7==0 )ks.cat_ptt =TX;//TX			
		//}
	}
	if(clk ==0 || ATT_Key0 != ks.ATT_key||ptt != TR_READ)
	{
		if(TR_READ ==CONTROL_RX)
		{
			if(ks.ATT_key ==0)
			{
				AMP_ON;
				ATT_OFF;
			}
			else 
			if(ks.ATT_key ==1)
			{	
				AMP_OFF;
				ATT_OFF;
			}
			else 
			if(ks.ATT_key ==2)
			{	
				AMP_OFF;
				ATT_ON;
			}
			ui_ATT_Display( ks.ATT_key);
			//clk =1;
			//ATT_Key0 = ks.ATT_key;
			//ptt = TR_READ;
		}
		else
		if(TR_READ ==CONTROL_TX)
		{
			ATT_OFF;
			AMP_OFF;
		}
		clk =1;
		ATT_Key0 = ks.ATT_key;
		ptt = TR_READ;
	}
}
/*
**********************************************************************************
*	��������
**********************************************************************************
*/
void bluetooth_icon(u16 fc)//����ͼ��
{
	//u8 y;
	Gui_DrawLine(95,6,95,18,fc);
	Gui_DrawLine(95,6,98,9,fc);
	Gui_DrawLine(91,8,98,15,fc);
	Gui_DrawLine(95,18,98,15,fc);
	Gui_DrawLine(91,16,98,9,fc);
}
void key_bluetooth_control()
{
	//static u8 bt_on,bt_dat;
	static u8 key,key1;
	
	//bt_on = BLUETOOTH_ON;
	//bt_dat =  BLUETOOTH_DAT;
	if(key1 != BLUETOOTH_DAT ||key != ks.bt_key )
	{
		if(ks.bt_key )
		{
			BLUETOOTH_POWER(1);//��������
			if(BLUETOOTH_DAT ==0)// && BLUETOOTH_ON ==0 )
			{
				USART3->CR1&=~(1<<0);  	//����ʹ
				ui_TopBackground(85,4,24,16);
				//LCD_ShowString(85, 4, GRAY3,  BLACK , 16, 1, "BLE" );
				bluetooth_icon(RED);
			}
			//else
//			if(BLUETOOTH_ON ==1)
//			{
//				USART3->CR1|=1<<0;
//				ui_TopBackground(82,4,24,16);
//				//LCD_ShowString(85, 4,  GRAY1,  BLACK , 16, 0, "BLE" );
//				bluetooth_icon(YELLOW);
//			}
			
			if(BLUETOOTH_DAT ==1)	
			{
				USART3->CR1|=1<<0;
				ui_TopBackground(82,4,24,16);
				//LCD_ShowString(85, 4, GREEN, BLACK , 16, 0, "BLE" );
				bluetooth_icon(GRAY0);
			}
//			if(BLUETOOTH_DAT ==1)	
//			{
//				USART3->CR1|=1<<0;
//				ui_TopBackground(82,4,24,16);
//				//LCD_ShowString(85, 4, YELLOW, BLACK , 16, 0, "BLE" );
//				bluetooth_icon(GREEN);
//			}
			//key0 = BLUETOOTH_ON;
			//key  = BLUETOOTH_DAT;
		}
		else
		{
			BLUETOOTH_POWER(0);
			//USART1->CR1|=1<<0;
			USART3->CR1&=~(1<<0);
			
			//LCD_ShowString(85, 4, BLUE2, BLUE2 , 16, 1, "   " );
			ui_TopBackground(85,4,24,16);
			bluetooth_icon(GRAY3);
		}
		//key0 = BLUETOOTH_ON;
		key1  = BLUETOOTH_DAT;
		key = ks.bt_key;
	}		
}
/*
**********************************************************************************
*	����������
**********************************************************************************
*/
void Hp_set()
{
	//u8 hp1,hp2;
	static u8 hp1_value;
	static u8 hp2_value;
	//hp1=HP_DETEC_1;
	//hp2=HP_DETEC_2;
	if(hp1_value!=HP_DETEC_1||hp2_value!=HP_DETEC_2)
	{
		if(HP_DETEC_1==1&&HP_DETEC_2==1)ks.Spk_key =0;
		else
		if(HP_DETEC_1==0||HP_DETEC_2==0)ks.Spk_key =1;
		//ks.Spk_key = (~hp1==1&~hp2==1)&0x01;
		ui_Mute_icon(220,4,RED,ks.Spk_key);
		hp1_value = HP_DETEC_1;
		hp2_value = HP_DETEC_2;
	}
}
/*
**********************************************************************************
*	��ѹ�����ʾ
**********************************************************************************
*/
void key_voltage_Display()
{
	static u8 clk =0;
	//static u16 power_v;
	static u16 power_v0;
	//power_v = Get_Adc1(ADC_CH5)*0.003825f;/* 16λADC */
	//power_v = Get_Adc1(ADC_CH5)*0.0594f; /* 12λADC */
	if(power_v0 != ads.power_v || clk ==0)
	{
		if(clk ==0)
		{
			ui_TopBackground(270,0,30,12);
			clk =1;
		}
		ui_Decimal_Dispaly(270,4,GRAY0,BLUE ,16,ads.power_v);
		LCD_ShowChar(302, 4, GRAY0, BLUE,16,0,'v');
		power_v0 = ads.power_v;
		clk =1;
	}
	else
	if(power_v0 == ads.power_v && clk ==1)
	{
		//ui_TopBackground(270,0,30,12);
		clk =0;
	}		
}
/*
*********************************************************************************
*	
*********************************************************************************
*/
void Pow_max_set(void)
{
	static u8 clk_0;
	static u8 pow_0;
	static u8 mode_0;
	static u8 pig_0;
	static u8 band_0;
	static u8 ptt_0;
	//static u8 pow_gain0;

	if(pow_0 != sd.Pow||mode_0!=vfo[VFO_A].Mode ||pig_0!=sd.menu_page||band_0!=vfo[VFO_A].Band_id||clk_0 ==0)
	{
		if(sd.menu_page>0)
		{
			sd.Pow =50;
		}
		else
		{
			sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
			//sd.Pow = AT24CXX_ReadLenByte( MENU_POW,1 );
			if(vfo[VFO_A].Band_id == COUPLING_6M)
			{
				if(sd.Pow >50)sd.Pow =50;
			}
			else
			{
				if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
				{
					if(sd.Pow >100)sd.Pow =100;
				}
				else if(sd.Pow >200)sd.Pow =200;
			}
		}
		//if(sd.Pow !=50)
			//ads.pow_gain [vfo[VFO_A].Band_id] = AT24CXX_ReadLenByte(vfo[VFO_A].Band_id*2 + ADDRPOWER_GAIN ,2);
		//if(sd.Pow < 1)sd.Pow =1;
		pow_gain_set();
		pow_0 =sd.Pow;
		pig_0 =sd.menu_page;
		mode_0 =vfo[VFO_A].Mode;
		band_0 =vfo[VFO_A].Band_id;
		clk_0 =1;
	}
}

/*
**********************************************************************************
*	TX/RX�ۺϲٿع���
**********************************************************************************
*/

//const u16 Addr_OFFSET[2]=
//{
//	ADDR_RT_OFFSET,
//	ADDR_RX_OFFSET
//};
///**/
//const u16 CH_ADDR[8] =
//{
//	ADDR_CH_NUM(VFO_A) ,
//	ADDR_CH_NUM(VFO_B) ,
//	ADDR_CH_NUM(CHA_NUM),
//	ADDR_CH_NUM(CHB_NUM),
//	ADDR_CH_NUM(RXVFO_A),
//	ADDR_CH_NUM(RXVFO_B) , 
//	ADDR_CH_NUM(RXCHA_NUM) ,
//	ADDR_CH_NUM(RXCHB_NUM) 
//};
/*
*	����ģʽ�ٿ�
*	mode_reg:
*				λ0��VFO A/Bѡ��0��VFO_A,1: VFO_B;
*				λ1���ŵ�CH/VFOѡ��0��VFO��1��CH;
*				λ2����̨�Ĺ���ģʽ��0���շ�ģʽ��1��ֻ����ģʽ��
*				λ3-λ5��������
*				λ6��ͬƵ/��Ƶ�շ�ѡ��0���շ�ͬƵ��1���շ���Ƶ��
*				λ7���շ�ת����0������RX��1������TX��
*/
void RXandTX_Control_Management()
{
	
	static u8 clk =0;
	 u8 i;
	//static u8 num_b;
	static u8 addr_a;
	static u8 addr_b;
	static u32 freq_a;
	static u32 freq_b;
	static u32 freq_si;
	static u8 mode_a;
	static u8 band_a;
	static u8 band_b;
	static u8 IF_gain;
	static u8 filter_a;
	static u8 tr_read;
	static u8 rt_rx;
	//static u8 tr_red;
	
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
	{
		/*
		*	����ģʽ�ٿ�
		*/
		
		/* ��̨����ģʽ�л� */
		if( ucKeyCode==KEY_4_LONG )	// [AM.FM]������
		{
			//ads.tx_delay=50;
			ks.rt_rx = ~ks.rt_rx&0x01;
			 ks.vfo_read =0;
			ads.tx_delay=100;
		}
		//* ͬƵ/��Ƶת��
		if( ucKeyCode==KEY_6_LONG )	//[A.B]������
		{
			ads.tx_delay=100;
			//ks.A_B_vfo = ~ks.A_B_vfo &0x01;
			ks.sim_dif = ~ks.sim_dif &0x01;
			 ks.vfo_read =0;
		}
		//*VFO/�ŵ�ģʽת��
		if( ucKeyCode==KEY_2_UP )	//[M.V]���̰�
		{
			ads.tx_delay=100;
			ks.ch_vfo = ~ks.ch_vfo &0x01;	
			 ks.vfo_read =0;
		}
		//* A/B ת��
		if( ucKeyCode==KEY_6_UP)	//[A.B]���̰�
		{
			ads.tx_delay=100;
			ks.A_B = ~ks.A_B &0x01;
			 ks.vfo_read =0;
		}
	
	}
	/**/
	if(PIN_K13 ==0 || ks.vfo_read ==0||clk==0 || tr_read !=TR_READ)
	{
		/*
		*	��Ƶ����ʱ����Ƶ��Ϊ����Ƶ�ʣ�����Ƶ����ʾλ�ý�����
		*/
		if(clk)
		{
			if(ks.sim_dif ==1 && TR_READ ==CONTROL_TX)
			{
				addr_a = ((ks.rt_rx  &0x01)<<2) | ((ks.ch_vfo &0x01)<<1) | (~ks.A_B & 0x01);									
				addr_b = ((ks.rt_rx  &0x01)<<2) | ((ks.ch_vfo &0x01)<<1) | ( ks.A_B & 0x01);
			}
			else/* A/Bת�� */	
			{		
				addr_a = ((ks.rt_rx  &0x01)<<2) | ((ks.ch_vfo &0x01)<<1) | ( ks.A_B & 0x01);
				addr_b = ((ks.rt_rx  &0x01)<<2) | ((ks.ch_vfo &0x01)<<1) | (~ks.A_B & 0x01);
			}
		}
		else
		{
			addr_a = AT24CXX_ReadOneByte(ADDR_A );// ��ȡ���ŵ����
			if(addr_a<1)addr_a =0;
			if(addr_a>7)addr_a =0;
			addr_b = AT24CXX_ReadOneByte(ADDR_B );// ��ȡ���ŵ����
			if(addr_b<1)addr_b =0;
			if(addr_b>7)addr_b =1;
			
			ks.rt_rx = (addr_a >>2)&0x01;
			ks.ch_vfo = (addr_a >>1)&0x01;
			ks.A_B = (addr_a >>0)&0x01;
			sd.num_b = AT24CXX_ReadOneByte(CH_ADDR[addr_b]);// ��ȡ���ŵ����
//			if(ks.A_B_vfo)
//			{
//				sd.num_a = AT24CXX_ReadOneByte(CH_ADDR[addr_b]);// ��ȡ���ŵ����
//			}
//			else
			sd.num_a = AT24CXX_ReadOneByte(CH_ADDR[addr_a]);// ��ȡ���ŵ����
			
			if(sd.num_a> 99) sd.num_a = 99;	// �綨����ŵ���ֵ
			if(sd.num_a < 0) sd.num_a = 0;	// �綨��С�ŵ���ֵ
			if(sd.num_b> 99) sd.num_b = 99;	// �綨����ŵ���ֵ
			if(sd.num_b < 0) sd.num_b = 0;	// �綨��С�ŵ���ֵ
		}
		if( PIN_K13 ==1)	
		{
			if(ks.ch_vfo ==0)/* VFOģʽ A/B */
			{
				sd.num_b = addr_b;// ��ȡ���ŵ�
				//if(ks.A_B_vfo)sd.num_a = addr_b;// ��ȡ���ŵ�
				sd.num_a = addr_a;// ��ȡ���ŵ�
				AT24CXX_WriteOneByte(ADDR_A, addr_a);
				AT24CXX_WriteOneByte(ADDR_B, addr_b);
				AT24CXX_WriteOneByte(CH_ADDR[addr_a], sd.num_a);
				AT24CXX_WriteOneByte(CH_ADDR[addr_b], sd.num_b);
			}
			else/* CH�ŵ�ģʽ CHa/CHb */
			{
				sd.num_b = AT24CXX_ReadOneByte(CH_ADDR[addr_b]);// ��ȡ���ŵ����
				//if(ks.A_B_vfo)sd.num_a = AT24CXX_ReadOneByte(CH_ADDR[addr_b]);// ��ȡ���ŵ����
				sd.num_a = AT24CXX_ReadOneByte(CH_ADDR[addr_a]);// ��ȡ���ŵ����
			}
			if(sd.num_a> 99) sd.num_a = 99;	// �綨����ŵ���ֵ
			if(sd.num_a < 0) sd.num_a = 0;	// �綨��С�ŵ���ֵ
			if(sd.num_b> 99) sd.num_b = 99;	// �綨����ŵ���ֵ
			if(sd.num_b < 0) sd.num_b = 0;	// �綨��С�ŵ���ֵ
			
		}
		else/* ֻ��Ƶ�ʱ��������س���ʱ�����ŵ���� */
		if( PIN_K13 ==0&& ks.ch_vfo ==1)	
		{
			AT24CXX_WriteOneByte(CH_ADDR[addr_a], sd.num_a);
			if(sd.num_a> 99) sd.num_a = 99;	// �綨����ŵ���ֵ
			if(sd.num_a < 2) sd.num_a = 2;	// �綨��С�ŵ���ֵ
		}
		/*
		* 	��ȡ����ʾ����Ϣ
		*/
		if(ks.F_11==0) 
		{
			/* ��Ƶ�ʶ�ȡ */
			vfo[VFO_A].Freq = AT24CXX_ReadLenByte( FREQ_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],4 );// ��ȡ���ŵ�
			
			if(ks.rt_rx )/* ���ջ�ģʽ */
			{
				if(vfo[VFO_A].Freq >  RxBand_Info[RADIO_10M].end )//* �޶�Ƶ�����ֵ */
				{
					vfo[VFO_A].Freq =  RxBand_Info[RADIO_10M].end ;
				}	
				if(vfo[VFO_A].Freq <  RxBand_Info[RADIO_90M].start)/* �޶�Ƶ����Сֵ */
				{
					vfo[VFO_A].Freq =  RxBand_Info[RADIO_90M].start;
				}
				/* ��Ƶ�ʲ��ζ�ȡ */
				vfo[VFO_A].RxBand_id = AT24CXX_ReadLenByte( BAND_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],1 );// ��ȡ���ŵ� 
				if(vfo[VFO_A].RxBand_id > RADIO_10M )vfo[VFO_A].RxBand_id = RADIO_10M;
				if(vfo[VFO_A].RxBand_id < RADIO_90M )vfo[VFO_A].RxBand_id = RADIO_90M;
				if(vfo[VFO_A].Freq > RxBand_Info[vfo[VFO_A].RxBand_id].end || vfo[VFO_A].Freq < RxBand_Info[vfo[VFO_A].RxBand_id].start )
				{
					i+=1;
					if(i > RADIO_10M)i = RADIO_10M;
					if(vfo[VFO_A].Freq < RxBand_Info[i].end && vfo[VFO_A].Freq > RxBand_Info[i].start )
					{
						vfo[VFO_A].RxBand_id =i;
						
					}
				}
				//sd.band =vfo[VFO_A].RxBand_id ;
				//vfo[VFO_A].Mode = AT24CXX_ReadLenByte( vfo[VFO_A].RxBand_id + ADDR_RX_BAND_MODE_M ,1 );
				//if(vfo[VFO_A].Mode >DEMOD_FM )vfo[VFO_A].Mode = DEMOD_FM;
				//if(vfo[VFO_A].Mode <DEMOD_CW_LSB )vfo[VFO_A].Mode = DEMOD_CW_LSB;
			}
			else/* �շ��Ż�ģʽ */
			{
				if(vfo[VFO_A].Freq >  Band_Info[COUPLING_6M].end )//* �޶�Ƶ�����ֵ */
				{
					vfo[VFO_A].Freq = Band_Info[COUPLING_6M].end ;
				}	
				if(vfo[VFO_A].Freq <  Band_Info[COUPLING_80M].start)/* �޶�Ƶ����Сֵ */
				{
					vfo[VFO_A].Freq = Band_Info[COUPLING_80M].start;
				}
				/* ��Ƶ�ʲ��ζ�ȡ */
				vfo[VFO_A].Band_id = AT24CXX_ReadOneByte( BAND_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ] );// ��ȡ���ŵ� 
				if(vfo[VFO_A].Band_id > COUPLING_6M)
				{
					vfo[VFO_A].Band_id = COUPLING_6M ;
					vfo[VFO_A].Freq = Band_Info[COUPLING_6M].end ;
				}
				if(vfo[VFO_A].Band_id < COUPLING_80M)
				{
					vfo[VFO_A].Band_id = COUPLING_80M ;
					vfo[VFO_A].Freq = Band_Info[COUPLING_80M].start;
				}
				
				if(vfo[VFO_A].Freq > Band_Info[vfo[VFO_A].Band_id].end || vfo[VFO_A].Freq < Band_Info[vfo[VFO_A].Band_id].start )
				{
					i+=1;
					if(i > COUPLING_6M)i = COUPLING_80M ;
					if(vfo[VFO_A].Freq < Band_Info[i].end && vfo[VFO_A].Freq > Band_Info[i].start )
					{
						vfo[VFO_A].Band_id =i;
						
					}
				}
				//if(sd.menu_page>0)sd.Pow =50;
				//else sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
			}			
			/* ��Ƶ��ģʽ��ȡ */
			//vfo[VFO_A].Mode = AT24CXX_ReadLenByte( vfo[VFO_A].Band_id + ADDR_RT_BAND_MODE_M ,1 );
			vfo[VFO_A].Mode  = AT24CXX_ReadOneByte( MODE_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ] );// ��ȡ���ŵ� 
			if(vfo[VFO_A].Mode >DEMOD_FM )vfo[VFO_A].Mode = DEMOD_FM;
			if(vfo[VFO_A].Mode <DEMOD_CW_LSB )vfo[VFO_A].Mode = DEMOD_CW_LSB;
			
			/* ��Ƶ���˲�����ȡ */
			if(TR_READ == CONTROL_RX)
			{
				vfo[VFO_A].FilterSelected_id = AT24CXX_ReadOneByte( FILTERSELECTED_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ] );// ��ȡ���ŵ� 
				if(vfo[VFO_A].Mode < DEMOD_AM )
				{
					if(vfo[VFO_A].FilterSelected_id > 5 )vfo[VFO_A].FilterSelected_id = 5;
				}
				else if(vfo[VFO_A].FilterSelected_id > 1 )vfo[VFO_A].FilterSelected_id = 1;
				if(vfo[VFO_A].FilterSelected_id < 1 )vfo[VFO_A].FilterSelected_id = 0;
			}
			else	vfo[VFO_A].FilterSelected_id = sd.tx_filter ;
//			if(vfo[VFO_A].Mode <DEMOD_LSB )sd.cw_fil_id = vfo[VFO_A].FilterSelected_id;
//			else if(vfo[VFO_A].Mode == DEMOD_LSB )sd.lsb_fil_id = vfo[VFO_A].FilterSelected_id;
//			else if(vfo[VFO_A].Mode == DEMOD_USB )sd.usb_fil_id = vfo[VFO_A].FilterSelected_id;
//            else if(vfo[VFO_A].Mode == DEMOD_AM )sd.am_fil_id = vfo[VFO_A].FilterSelected_id;
//			else if(vfo[VFO_A].Mode == DEMOD_FM )sd.fm_fil_id = vfo[VFO_A].FilterSelected_id;
			/*
			* 	��ȡ����ʾ����Ϣ
			*/
			/* ��Ƶ�ʶ�ȡ */
			vfo[VFO_B].Freq = AT24CXX_ReadLenByte( FREQ_CH(sd.num_b) + Addr_OFFSET[ks.rt_rx ],4 );// ��ȡ���ŵ�
			if(ks.rt_rx )
			{
				if(vfo[VFO_B].Freq >  RxBand_Info[RADIO_10M].end )//* �޶�Ƶ�����ֵ */
				{
					vfo[VFO_B].Freq =  RxBand_Info[RADIO_10M].end ;
				}	
				if(vfo[VFO_B].Freq <  RxBand_Info[RADIO_90M].start)/* �޶�Ƶ����Сֵ */
				{
					vfo[VFO_B].Freq =  RxBand_Info[RADIO_90M].start;
				}
				
				vfo[VFO_B].RxBand_id = AT24CXX_ReadLenByte( BAND_CH(sd.num_b) + Addr_OFFSET[ks.rt_rx ],1 );// ��ȡ���ŵ� 
				if(vfo[VFO_B].RxBand_id > RADIO_10M )vfo[VFO_B].RxBand_id = RADIO_10M;
				if(vfo[VFO_B].RxBand_id < RADIO_MW )vfo[VFO_B].RxBand_id = RADIO_MW;
			}
			else
			{
				if(vfo[VFO_B].Freq >  Band_Info[COUPLING_6M].end )//* �޶�Ƶ�����ֵ */
				{
					vfo[VFO_B].Freq = Band_Info[COUPLING_6M].end ;
				}	
				if(vfo[VFO_B].Freq <  Band_Info[COUPLING_80M].start)/* �޶�Ƶ����Сֵ */
				{
					vfo[VFO_B].Freq = Band_Info[COUPLING_80M].start;
				}
				
				vfo[VFO_B].Band_id = AT24CXX_ReadOneByte( BAND_CH(sd.num_b) + Addr_OFFSET[ks.rt_rx ] );// ��ȡ���ŵ� 
				if(vfo[VFO_B].Band_id > COUPLING_6M)vfo[VFO_B].Band_id = COUPLING_6M ;
				if(vfo[VFO_B].Band_id < COUPLING_80M)vfo[VFO_B].Band_id = COUPLING_80M ;
				
			}					
			
			/* ��Ƶ��ģʽ��ȡ */
			vfo[VFO_B].Mode  = AT24CXX_ReadLenByte( MODE_CH(sd.num_b) + Addr_OFFSET[ks.rt_rx ],1 );// ��ȡ���ŵ� 
			if(vfo[VFO_B].Mode >DEMOD_FM )vfo[VFO_B].Mode = DEMOD_FM;
			if(vfo[VFO_B].Mode <DEMOD_CW_LSB )vfo[VFO_B].Mode = DEMOD_CW_LSB;
			
			/* ��Ƶ���˲�����ȡ */
			vfo[VFO_B].FilterSelected_id = AT24CXX_ReadLenByte( FILTERSELECTED_CH(sd.num_b) + Addr_OFFSET[ks.rt_rx ],1 );// ��ȡ���ŵ� 
			if(vfo[VFO_B].Mode < DEMOD_AM )
            {
                if(vfo[VFO_B].FilterSelected_id > 5 )vfo[VFO_B].FilterSelected_id = 5;
            }
            else if(vfo[VFO_B].FilterSelected_id > 1 )vfo[VFO_B].FilterSelected_id = 1;
            if(vfo[VFO_B].FilterSelected_id < 1 )vfo[VFO_B].FilterSelected_id = 0;
			
			ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
			ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
			//ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+ads.rx_amp_band, 1);
			//if(ads.IF_gain >63)ads.IF_gain =32;
			//if(ads.IF_gain < 1)ads.IF_gain =32;
//			if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
//			{
//				if(sd.Pow >100)sd.Pow =100;
//			}
//			else if(sd.Pow >200)sd.Pow =200;
		}
		if(clk ==0|| rt_rx!=ks.rt_rx||band_a != ads.rx_amp_band)
		{
			
			ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+ads.rx_amp_band, 1);
			if(ads.IF_gain >63)ads.IF_gain =32;
			if(ads.IF_gain < 1)ads.IF_gain =32;
			//sd.band = vfo[VFO_A].Band_id; //����IF����
			
			//if(sd.menu_page==1)sd.Pow =50;
			//else sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
			
//			if(sd.menu_page>0)
//			{
//				sd.Pow =50;
//			}
//			else
//			{
//				sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
//				if(vfo[VFO_A].Band_id == COUPLING_6M)
//				{
//					if(sd.Pow >50)sd.Pow =50;
//				}
//				else
//				{
//					if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
//					{
//						if(sd.Pow >100)sd.Pow =100;
//					}
//					else if(sd.Pow >200)sd.Pow =200;
//				}
//			}
			//if(sd.Pow < 1)sd.Pow =1;
			//pow_gain_set();
			rt_rx =ks.rt_rx;
			sd.Riss = ads.IF_gain;
			//band_b = vfo[VFO_A].RxBand_id;
			band_a = ads.rx_amp_band;
			WM8978_MIC_Gain(ads.IF_gain&0x3f);//MIC��������
			//IF_gain = ads.IF_gain[sd.band];
		}
		clk =1;
		ks.vfo_read = 1;
		tr_read =TR_READ;
	}
	key_Step_Control(ks.StepSelected_idx );
	key_Mode_Control();	
	key_Filter_Control(vfo[VFO_A].FilterSelected_id );
	Key_Band_Control();
	ui_Vfo_Display(sd.num_a);
	
	//Key_Band_Control();
	//key_Mode_Control();
	/* ��Ƶ�ʶ�̬���� */
	if(freq_a != vfo[VFO_A].Freq )
	{
		LCD_Frequency_Display(64, 26, GRAY0, BLACK, 32, vfo[VFO_A].Freq);/* ��Ƶ����ʾ */
		AT24CXX_WriteLenByte( FREQ_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],vfo[VFO_A].Freq, 4);/* Ƶ�ʱ��� */
		if(ks.rt_rx )/* ���ջ�ģʽ */
		{
			AT24CXX_WriteLenByte( vfo[VFO_A].RxBand_id*4 + ADDR_RX_BAND_FREQ_M,vfo[VFO_A].Freq, 4);/* RXBANDƵ�ʱ��� */
		}
		else
		{
			AT24CXX_WriteLenByte( vfo[VFO_A].Band_id*4 + ADDR_RT_BAND_FREQ_M,vfo[VFO_A].Freq, 4);/* RTBAND Ƶ�ʱ��� */
		}
		freq_a = vfo[VFO_A].Freq;	
	}
	/* ��Ƶ�ʶ�̬���� */
	if(freq_b != vfo[VFO_B].Freq)
	{
		LCD_Frequency_Display(104,60, GRAY2, GRAY5, 24, vfo[VFO_B].Freq);/* ��Ƶ����ʾ */
		freq_b = vfo[VFO_B].Freq;	
	}
	/* ģʽ��̬���� */
	if(mode_a != vfo[VFO_A].Mode)
	{
		AT24CXX_WriteOneByte( MODE_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],vfo[VFO_A].Mode );
		if(ks.rt_rx )/* ���ջ�ģʽ */
		{
			AT24CXX_WriteLenByte( vfo[VFO_A].RxBand_id + ADDR_RX_BAND_MODE_M,vfo[VFO_A].Mode , 1);/* RXBANDƵ�ʱ��� */
		}
		else
		{
			AT24CXX_WriteLenByte( vfo[VFO_A].Band_id + ADDR_RT_BAND_MODE_M,vfo[VFO_A].Mode , 1);/* RTBAND Ƶ�ʱ��� */
		}
		//sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
//		if(sd.menu_page>0)
//		{
//			sd.Pow =50;
//		}
//		else
//		{
//			sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
//			if(vfo[VFO_A].Band_id == COUPLING_6M)
//			{
//				if(sd.Pow >50)sd.Pow =50;
//			}
//			else
//			{
//				if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
//				{
//					if(sd.Pow >100)sd.Pow =100;
//				}
//				else if(sd.Pow >200)sd.Pow =200;
//			}
//		}
		//pow_gain_set();
		mode_a = vfo[VFO_A].Mode;
	}
	/* ���ζ�̬���� */
	if(band_a != vfo[VFO_A].Band_id || band_b != vfo[VFO_A].RxBand_id)
	{
		if(ks.rt_rx)
		{
			AT24CXX_WriteOneByte( BAND_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],vfo[VFO_A].RxBand_id );
			band_b = vfo[VFO_A].RxBand_id;
		}
		else
		{
			AT24CXX_WriteOneByte( BAND_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],vfo[VFO_A].Band_id );
			pow_gain_set();
			band_a = vfo[VFO_A].Band_id;
		}
		sd.Riss = ads.IF_gain;
		WM8978_MIC_Gain(ads.IF_gain&0x3f);//MIC��������
	}
	/* �˲�����̬���� */
	if(TR_READ == CONTROL_RX)
	{
		
		if(filter_a != vfo[VFO_A].FilterSelected_id)
		{
			AT24CXX_WriteOneByte( FILTERSELECTED_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ], vfo[VFO_A].FilterSelected_id );
			filter_a = vfo[VFO_A].FilterSelected_id;
		}
		if(ks.rt_rx )/* ���ջ�ģʽ */
		{
			AT24CXX_WriteLenByte( vfo[VFO_A].RxBand_id + ADDR_RX_BAND_FIL_M,vfo[VFO_A].FilterSelected_id, 1);/* RXBANDƵ�ʱ��� */
		}
		else
		{
			AT24CXX_WriteLenByte( vfo[VFO_A].Band_id + ADDR_RT_BAND_FIL_M,vfo[VFO_A].FilterSelected_id , 1);/* RTBAND Ƶ�ʱ��� */
		}
	}
	else	vfo[VFO_A].FilterSelected_id = sd.tx_filter ;
	/* һ����̬д�� */
	if(freq_si != vfo[VFO_A].Freq + sd.Tcxo + sd.IF_1)//&&vfo[VFO_A].Mode >=DEMOD_LSB)
	{
		si5351aSetFrequency_LoA(sd.Tcxo, (vfo[VFO_A].Freq - sd.IF_1)*2);
		
		freq_si = vfo[VFO_A].Freq + sd.Tcxo +sd.IF_1;
	}
	if(IF_gain != ads.IF_gain)
	{
//		if(ks.rt_rx )AT24CXX_WriteOneByte(ADDR_IF_RXGAIN+sd.band, ads.IF_gain);
//		else 
//		{
			AT24CXX_WriteOneByte(ADDR_IF_GAIN+ads.rx_amp_band, ads.IF_gain);
//			//AT24CXX_WriteOneByte(ADDRPOW_MM+vfo[VFO_A].Band_id, sd.Pow);
//		}
		//if(ads.IF_gain[sd.band] >63)ads.IF_gain[sd.band] =32;
		//if(ads.IF_gain[sd.band] < 1)ads.IF_gain[sd.band] =32;
		sd.Riss = ads.IF_gain;
		WM8978_MIC_Gain(ads.IF_gain&0x3f);//MIC��������
		IF_gain = ads.IF_gain;
	}
}
/*
**********************************************************************************
*	������λ�ͷ�����ʾ
**********************************************************************************
*/
void Rx_amp_phase_adjuse()
{
	static u8 menu0;
	static u8 clk0;
	static u16 amp_0;
	//static u16 amp_m;
	static u8 phase_0;
	static u8 key;
	if(ks.rx_adjuset_key ==1)
	{	
		if(key!=sd.amp_phase){clk0=0;key=sd.amp_phase;}
		if(menu0!=sd.menu||clk0==0||amp_0!=Rx_amp_user[ads.rx_amp_band]||phase_0!=Rx_phase_user[ads.rx_amp_band])
		{
			if(clk0==0)Lcd_Color(0, 182,319,239,BLACK) ;
			if(sd.Rx_amp_ma)sd.menu=1;
//			if(sd.menu==2)
//			{
//				LCD_ShowString(30,182+36,YELLOW,BLUE1, 16, 0,"  AMP_S   " );
//				LCD_ShowNum(220,182+36,GRAY1,BLUE1, 9,16,Rx_amp_adjust[ads.amp_adjust_num]);
//				LCD_ShowString(30,182+18,YELLOW,BLACK, 16, 0,"  PHASE   " );
//				LCD_ShowNum(220,182+18,GRAY1,BLACK, 9,16,Rx_phase_user[ads.rx_amp_band]);
//				LCD_ShowString(30,182,YELLOW,BLACK, 16, 0,"  AMP     " );
//	 			LCD_ShowNum(220,182,GRAY1,BLACK, 9,16,Rx_amp_user[ads.rx_amp_band]);
//			}
			if(sd.menu==1)
			{
				//LCD_ShowString(30,182+36,YELLOW,BLACK, 16, 0,"  AMP_S   " );
				//LCD_ShowNum(220,182+36,GRAY1,BLACK, 9,16,Rx_amp_adjust[ads.amp_adjust_num]);
				LCD_ShowString(30,182+18,YELLOW,BLUE1, 16, 0,"  PHASE   " );
				LCD_ShowNum(220,182+18,GRAY1,BLUE1, 9,16,Rx_phase_user[ads.rx_amp_band]);
				LCD_ShowString(30,182,YELLOW,BLACK, 16, 0,"  AMP     " );
				LCD_ShowNum(220,182,GRAY1,BLACK, 9,16,Rx_amp_user[ads.rx_amp_band]);
			}
			if(sd.menu==0)
			{
				//LCD_ShowString(30,182+36,YELLOW,BLACK, 16, 0,"  AMP_S   " );
				//LCD_ShowNum(220,182+36,GRAY1,BLACK, 9,16,Rx_amp_adjust[ads.amp_adjust_num]);
				LCD_ShowString(30,182+18,YELLOW,BLACK, 16, 0,"  PHASE   " );
				LCD_ShowNum(220,182+18,GRAY1,BLACK, 9,16,Rx_phase_user[ads.rx_amp_band]);
				LCD_ShowString(30,182,YELLOW,BLUE1, 16, 0,"  AMP     " );
				LCD_ShowNum(220,182,GRAY1,BLUE1, 9,16,Rx_amp_user[ads.rx_amp_band]);
			}
			ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
			ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
			//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
			clk0=1;
			amp_0=Rx_amp_user[ads.rx_amp_band];
			phase_0 =Rx_phase_user[ads.rx_amp_band];
			menu0 =sd.menu;
		}
	}
}
/*
**********************************************************************************
*	MENU�˵����ܲٿع���
**********************************************************************************
*/

/*
* 	���ڲ˵���Ŀ��ʾ��·����Ϣ������
*/
typedef struct
{
	 //u8  num;
	char* name;
	int32_t start;
	int32_t Default_fr;
	int32_t end;
	 //u8  Len;
	u16 addr;
	u8  Len;
	//u8  stp_idx;
}Menu_GenInfo;
//typedef struct
//{
//	Menu_GenInfo	name[12];
//}MNAU_NNAME;
//MNAU_NNAME Menu[2];
//
int32_t  Menu_user[2][28];
//
Menu_GenInfo User[] = //***���������Ϊ4�ı���
{

	{ "0 MENU         ",	       5,         30,         100,	MENU,  				1}, //0
	{ "1 IF 1         ",	   20000,	   24000,	    28000,	MENU_IF_1,			4}, //1	
	{ "2 CW_DELAY     ",		   5,   	  15,          99,  MENU_CW_DELAY, 		1}, //3
	{ "3 AGC_SPEED    ",	   	   0,	       1,           2,  MENU_AGC_M_A,  		1},	//4
	
	{ "4 BLUETOOTH    ", 	       0,	       1,           1,  MENU_BLUETOOTH, 	1}, //5
	{ "5 RIT_PTT      ", 	       0,	       0,           1,  MENU_RITATT_PTT, 	1}, //6
	{ "6 ENCODE_FREQ  ", 	       0,	       1,           5,  MENU_ENC_FREQ, 		1}, //7
	{ "7 TX_FILTER    ", 	       0,	       4,           5, 	MENU_TX_FILTER,     1}, //9
	
	{ "8 TX_AF_COMP   ", 	       1,	      10,          99, 	MENU_TX_AF_COMP,     1}, //9
	{ "END            ",	0,	0,	0,	0,	0,},
	{ "END            ",	0,	0,	0,	0,	0,},
	{ "END            ",	0,	0,	0,	0,	0,},
	
//	{ "END            ",	0,	0,	0,	0,	0,},
//	{ "END            ",	0,	0,	0,	0,	0,},
//	{ "END            ",	0,	0,	0,	0,	0,},
};

	/* �����ǿ�������Ŀ*/
Menu_GenInfo Deve[] = //***���������Ϊ4�ı���
{	
	{ "0 POW_CORRE     ",	      50,         70,         100,	MENU_POW_CORRE,  	1}, //0
	{ "1 AGC_STARE     ",		   2,		 127,         200,  MENU_AGC_START, 	1}, //10
	{ "2 S_CORRECT     ", 	       0,	      5,          63, 	MENU_S_CORRECT,     1}, //11
	{ "3 TCXO          ",    24900000,	25000000,    27100000,	MENU_TCXO,			4}, //12 
	
	{ "4 SPE_DISPALY   ", 	       0,	       0,          1,   MENU_SPE_DISPALY, 	1},
	{ "5 RX_AMP_MA     ", 	       0,	       0,          1,   MENU_RX_PHASE, 	    1}, 	//19
	{ "6 ALC_START     ", 	       0,	      15,          15,  MENU_ALC_START, 	1}, //15
	{ "7 ALC_MIC       ", 	       0,	      32,          63,  MENU_ALC_MIC, 		1}, 	//16
	
	{ "8 TX_AMP_80M    ", 	    500,	   1000,       2000,  MENU_TX_AMP_80M, 	2}, //13
	{ "9 TX_PHASE_80M  ", 	     50,	     99,        200,  MENU_TX_PHASE_80M, 	2}, //14
	{ "10 TX_AMP_60M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_60M, 	2}, //13
	{ "11 TX_PHASE_60M ", 	     50,	     99,        200,  MENU_TX_PHASE_60M, 	2}, //14
	
	{ "12 TX_AMP_40M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_40M, 	2}, //13
	{ "13 TX_PHASE_40M ", 	     50,	     99,        200,  MENU_TX_PHASE_40M, 	2}, //14
	{ "14 TX_AMP_30M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_30M, 	2}, //13
	{ "15 TX_PHASE_30M ", 	     50,	     99,        200,  MENU_TX_PHASE_30M, 	2}, //14
	
	{ "16 TX_AMP_20M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_20M, 	2}, //13
	{ "17 TX_PHASE_20M ", 	     50,	     99,        200,  MENU_TX_PHASE_20M, 	2}, //14data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYBAMAAAASWSDLAAAAAXNSR0IArs4c6QAAABtQTFRFAAAA////////////////////////////////600+wgAAAAl0Uk5TAAMKSKqrxMrMLwzd/QAAAFNJREFUGNNjYCAWCEEoRRDBGGYAophTBIAkU0YxiGPepgCm2oFSzBVgIQgNFoFKwSTAUnAJkBRcAiSFkEDlICtDMQDZaBRLkZ2D4lAUL6B4jigAABZQGm2ENmfZAAAAAElFTkSuQmCC
	{ "18 TX_AMP_17M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_17M, 	2}, //13
	{ "19 TX_PHASE_17M ", 	     50,	     99,        200,  MENU_TX_PHASE_17M, 	2}, //14
	
	{ "20 TX_AMP_15M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_15M, 	2}, //13
	{ "21 TX_PHASE_15M ", 	     50,	     99,        200,  MENU_TX_PHASE_15M, 	2}, //14
	{ "22 TX_AMP_12M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_12M, 	2}, //13
	{ "23 TX_PHASE_12M ", 	     50,	     99,        200,  MENU_TX_PHASE_12M, 	2}, //14
	
	{ "24 TX_AMP_10M   ", 	    500,	   1000,       2000,  MENU_TX_AMP_10M, 	2}, //13
	{ "25 TX_PHASE_10M ", 	     50,	     99,        200,  MENU_TX_PHASE_10M, 	2}, //14
	{ "26 TX_AMP_6M    ", 	    500,	   1000,       2000,  MENU_TX_AMP_6M, 	2}, //13
	{ "27 TX_PHASE_6M  ", 	     50,	     99,        200,  MENU_TX_PHASE_6M, 	2}, //14
	
	
};
//Menu_GenInfo Rx_Phase[] = //***���������Ϊ4�ı���
//{	
//	{ "0 RX_AMP_80M    ", 	    500,	   1115,       2000,  MENU_RX_AMP_80M, 	2}, //13
//	{ "1 RX_PHASE_80M  ", 	     50,	     92,        200,  MENU_RX_PHASE_80M, 	1}, //14
//	{ "2 RX_AMP_60M   ", 	    500,	   1112,       2000,  MENU_RX_AMP_60M, 	2}, //13
//	{ "3 RX_PHASE_60M ", 	     50,	     99,        200,  MENU_RX_PHASE_60M, 	1}, //14
//	
//	{ "4 RX_AMP_40M   ", 	    500,	   1110,       2000,  MENU_RX_AMP_40M, 	2}, //13
//	{ "5 RX_PHASE_40M ", 	     50,	     99,        200,  MENU_RX_PHASE_40M, 	1}, //14
//	{ "6 RX_AMP_30M   ", 	    500,	   1107,       2000,  MENU_RX_AMP_30M, 	2}, //13
//	{ "7 RX_PHASE_30M ", 	     50,	     99,        200,  MENU_RX_PHASE_30M, 	1}, //14
//	
//	{ "8 RX_AMP_20M   ", 	    500,	   1103,       2000,  MENU_RX_AMP_20M, 	2}, //13
//	{ "9 RX_PHASE_20M ", 	     50,	     99,        200,  MENU_RX_PHASE_20M, 	1}, //14
//	{ "10 RX_AMP_17M   ", 	    500,	   1100,       2000,  MENU_RX_AMP_17M, 	2}, //13
//	{ "11 RX_PHASE_17M ", 	     50,	    100,        200,  MENU_RX_PHASE_17M, 	1}, //14
//	
//	{ "12 RX_AMP_15M   ", 	    500,	   1100,       2000,  MENU_RX_AMP_15M, 	2}, //13
//	{ "13 RX_PHASE_15M ", 	     50,	     93,        200,  MENU_RX_PHASE_15M, 	1}, //14
//	{ "14 RX_AMP_12M   ", 	    500,	   1091,       2000,  MENU_RX_AMP_12M, 	2}, //13
//	{ "15 RX_PHASE_12M ", 	     50,	     99,        200,  MENU_RX_PHASE_12M, 	1}, //14
//	
//	{ "16 RX_AMP_10M   ", 	    500,	   1092,       2000,  MENU_RX_AMP_10M, 	2}, //13
//	{ "17 RX_PHASE_10M ", 	     50,	     99,        200,  MENU_RX_PHASE_10M, 	1}, //14
//	{ "18 RX_AMP_6M    ", 	    500,	   1057,       2000,  MENU_RX_AMP_6M, 	2}, //13
//	{ "19 RX_PHASE_6M  ", 	     50,	     73,        200,  MENU_RX_PHASE_6M, 	1}, //14
//};
//void menu_init()
//{
//	u8 i;
//	for(i=0; i<16;i++)
//	{
//		Menu[0].name[i].name 		=User[i].name ;
//		Menu[0].name[i].start		=User[i].start ;
//		Menu[0].name[i].Default_fr	=User[i].Default_fr ;
//		Menu[0].name[i].end  		=User[i].end  ;
//		Menu[0].name[i].addr 		=User[i].addr ;
//		Menu[0].name[i].Len  		=User[i].Len ;
//		
//		Menu[1].name[i].name 		=Developers[i].name ;
//		Menu[1].name[i].start 		=Developers[i].start ;
//		Menu[1].name[i].Default_fr 	=Developers[i].Default_fr;
//		Menu[1].name[i].end 		=Developers[i].end ;
//		Menu[1].name[i].addr 		=Developers[i].addr ;
//		Menu[1].name[i].Len			=Developers[i].Len ;
//	}
//}
void Menus_Set(void)
{
	 u8 i;
	 u8 page;
	 u8 line;
	u16 bc;
	static u8 ptt;
	static u8 ptt_time;
	static u8 step_idx0;
	static u8 lock=0;
	static int32_t  Values[MENU_END + MENU_HIDD_END ];
	  
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
	{
		/*	�߷�AMP */
	//*MENU 1���˵����ܣ�0��Ƶ����ʾ��
		if( ucKeyCode==KEY_9_LONG)
		{
			if(ks.rx_adjuset_key)
			{
				sd.amp_phase = ~sd.amp_phase&0x01;
				ks.menu_key=0;
			}
			else
			{
				ks.menu_key= ~ks.menu_key &0x01;
				Lcd_Color(0, 130,319,239,BLACK);// ˢ��ҳ��
				ks.StepSelected_idx = T_STEP_1HZ_IDX;//���β���Ĭ������Ϊ1kHz;
				ads.tx_delay =0;
				sd.spe_fil_time=SPE_FIL_TIME;//Ƶ�״�����ʾ�ظ���ʱ��־
			}
		}
		//
		if( ucKeyCode==KEY_9_UP && (ks.menu_key ==1||sd.amp_phase==1)) 
		{
			if(ks.rx_adjuset_key ==1)
			{
				sd.menu += 1;
				if( sd.menu > 1 )sd.menu = 0;
				if( sd.menu <0 )sd.menu = 1;
				
				ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
				ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
			}
			else
			{
				sd.menu += 1;
				if( sd.menu > sd.menu_end -1 )sd.menu = 0;
				if( sd.menu < MENU )sd.menu = sd.menu_end -1;
				if(sd.menu_page>0)
				{
					sd.Pow = 50;
					pow_gain_set();
					if( sd.menu >= 8)
					{
						ks.rt_rx =0;
						vfo[VFO_A].Band_id = (sd.menu -8)/2;
						vfo[VFO_A].Freq = AT24CXX_ReadLenByte( vfo[VFO_A].Band_id *4 + ADDR_RT_BAND_FREQ_M ,4 );
						ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+vfo[VFO_A].Band_id, 1);
						//sd.Pow = 50;//AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
						//pow_gain_set();
						if(ads.IF_gain >63)ads.IF_gain =32;
						if(ads.IF_gain < 1)ads.IF_gain =32;
					}
				}
				sd.menu_read =0;
			}
			//sd.menu_read =0;
		}
		if( sd.band_set_delay<1)
		{			
			if( ucKeyCode==KEY_9_UP && ks.menu_key ==0 && sd.amp_phase ==0) 
			{
				ks.Spk_key = ~ks.Spk_key &0x01;
				ui_Mute_icon(220,4,RED,ks.Spk_key);
				//SOUND_CARD_POW(ks.Spe_key);
			}
		}
		//SOUND_CARD_POW(1);
	}
	//
	if(ks.rx_adjuset_key ==0)
	{
		if(ks.menu_key == MENU_ON && ks.F_11 ==0 &&sd.amp_phase ==0)	// ����˵�
		{
			if(ptt!=TR_READ )
			{
				if(TR_READ==CONTROL_TX)ptt_time =5;
				ptt=TR_READ;
			}
			if(TR_READ==CONTROL_RX)	
			{
				if(ptt_time>0)
				{
					ptt_time--;
					lock =0;
				}
				if(ptt_time<1)
				{
					ptt_time=0;
					//lock =1;
				}
			}
			ks.FunctionWindow_Use =1;//�๦�ܴ�����ռ��
			//ks.Spk_key =0;
			if(lock ==0)
			{
				//Lcd_Color(0, 130,319,239,BLACK) ;		// ˢ��ҳ��
				ui_Meun_Window(20, 130,80, GRAY0, BLACK);	// ���Ʋ˵�����
				spe.clk =0;
			}
			//
			page = sd.menu/4;		// ҳ�����
			line = sd.menu%4;		// ��Ŀ����
			
			/* �˵���ʾ�͵��� */
			if(sd.Enc0_Value !=0 || sd.menu_read ==0 || lock==0||Values[sd.menu]!=Menu_user[sd.menu_page ][sd.menu] || step_idx0 != ks.StepSelected_idx)/* ֻ�ڽ���˵����߲˵���Ŀ����ʱ���� */
			{
				
				if(sd.menu_read ==0)/* ��ȡ�˵��洢���� */
				{
	//				for(i=0; i<4;i++)
	//				{
	//					Menu_Values[page*4 + i] = AT24CXX_ReadLenByte(Menu[page*4 + i].addr, Menu[page*4 + i].Len );
	//					if(Menu_Values[page*4 + i] > Menu[page*4 + i].end || Menu_Values[page*4 + i] < Menu[page*4 + i].start )
	//					{
	//						Menu_Values[page*4 + i] = Menu[page*4 + i].Default_fr ;
	//					}						
	//				}	
					sd.menu_read =1;
				}
				
				//else
				//{
					for(i=0; i<MENU_HIDD_END;i++)
					{
						Menu_user[1][i] = AT24CXX_ReadLenByte(Deve[i].addr, Deve[i].Len );
						if(Menu_user[1][i]<Deve[i].start)Menu_user[1][i] = Deve[i].Default_fr;
						if(Menu_user[1][i]>Deve[i].end )Menu_user[1][i] = Deve[i].Default_fr;				
					}
				
					for(i=0; i<MENU_END;i++)
					{
						Menu_user[0][i] = AT24CXX_ReadLenByte(User[i].addr, User[i].Len );
						if(Menu_user[0][i]<User[i].start)Menu_user[0][i] = User[i].Default_fr;
						if(Menu_user[0][i]>User[i].end )Menu_user[0][i] = User[i].Default_fr;				
					}
				//}
				/* �˵����ݴ洢 */
				//AT24CXX_WriteLenByte( Menu[sd.menu].addr  ,Menu_Values[sd.menu]   ,Menu[sd.menu].Len);
			
				//�˵�ѡ����ʾ
				//if(ks.StepSelected_idx > (u8)(log10(Menu_user[sd.menu_page][sd.menu]+1)))ks.StepSelected_idx= T_STEP_1HZ_IDX;
				
				for(i=0; i<4;i++)
				{
					if(i == line)bc = BLUE1;	//ѡ�е�ѡ���Ϊ��ɫ
					else
						bc = BLACK;		// ����Ϊ��ɫ
					/* �˵�������ʾ */
					if(sd.menu_page)LCD_ShowString(30,132+i*20,YELLOW,bc, 16, 0, Deve [page*4 + i].name );
					else LCD_ShowString(30,132+i*20,YELLOW,bc, 16, 0, User[page*4 + i].name );
					/* �˵�������ʾ */	
					if((page*4 + i)<sd.menu_end)/* ���Ʒ�Χ */	
					{
						if(i == line)LCD_ShowNum_Step(220,132+i*20,GRAY1,bc, 9,16, Menu_user[sd.menu_page][page*4 + i],ks.StepSelected_idx);
						else LCD_ShowNum(220,132+i*20,GRAY1,bc, 9,16, Menu_user[sd.menu_page][page*4 + i]);
					}
					else/* �˵���Ŀ�������� */
					{
						LCD_ShowString(220,132+i*20,BLACK,BLACK, 16, 0, "         " );
						LCD_ShowString(30,132+i*20,BLACK,BLACK, 16,  0, "               " );
					}
				}
				/* �˵����ݴ��뻺�� */
				
				//{
					sd.menu_time =Menu_user[0][0]*60;
					sd.IF_1 = Menu_user[0][1]; 		/* һ��Ƶ���� */
					sd.CW_Delay = Menu_user[0][2]*10;	/* CW�Զ������� */
					ks.AGC_Constant = Menu_user[0][3];		/* AGC��������� */
					ks.bt_key = Menu_user[0][4];		/* �������� */
					sd.ritatt_ptt = Menu_user[0][5];
					sd.enc_freq = Menu_user[0][6];
					//sd.enc_exti = Menu_user[0][7];
					sd.tx_filter = Menu_user[0][7];
					sd.tx_af_comp =  Menu_user[0][8];
				//}
				/*
				*	�����ǿ�������Ŀ
				*/
				//else
				//{
					ads.pow_corre  =Menu_user[1][0]*0.00001f;
					sd.agc_Start = Menu_user[1][1]*65536;	/* AGC��ص���� */
					sd.S_correct = Menu_user[1][2];
					sd.Tcxo  = Menu_user[1][3];		/* �²�����Ƶ��У׼ */
					sd.TX_spe = Menu_user[1][4];
					sd.Rx_amp_ma = Menu_user[1][5];
					sd.alc_start = Menu_user[1][6];
					sd.alc_mic = Menu_user[1][7];
				
					ads.tx_amp[0] = Menu_user[1][8]*0.001f;
					ads.tx_phase[0] = (Menu_user[1][9]-100) *0.001f;
					ads.tx_amp[1] = Menu_user[1][10]*0.001f;
					ads.tx_phase[1] = (Menu_user[1][11]-100) *0.001f;
					ads.tx_amp[2] = Menu_user[1][12]*0.001f;
					ads.tx_phase[2] = (Menu_user[1][13]-100) *0.001f;
					ads.tx_amp[3] = Menu_user[1][14]*0.001f;
					ads.tx_phase[3] = (Menu_user[1][15]-100) *0.001f;
					ads.tx_amp[4] = Menu_user[1][16]*0.001f;
					ads.tx_phase[4] = (Menu_user[1][17]-100) *0.001f;
					ads.tx_amp[5] = Menu_user[1][18]*0.001f;
					ads.tx_phase[5] = (Menu_user[1][19]-100) *0.001f;
					ads.tx_amp[6] = Menu_user[1][20]*0.001f;
					ads.tx_phase[6] = (Menu_user[1][21]-100) *0.001f;
					ads.tx_amp[7] = Menu_user[1][22]*0.001f;
					ads.tx_phase[7] = (Menu_user[1][23]-100) *0.001f;
					ads.tx_amp[8] = Menu_user[1][24]*0.001f;
					ads.tx_phase[8] = (Menu_user[1][25]-100) *0.001f;
					ads.tx_amp[9] = Menu_user[1][26]*0.001f;
					ads.tx_phase[9] = (Menu_user[1][27]-100) *0.001f;
				//
				lock = 1;
				Values[sd.menu]=Menu_user[sd.menu_page][sd.menu];
				step_idx0 = ks.StepSelected_idx;
			}
			if(sd.menu_page==0&& ks.rx_adjuset_key ==0)
			{
				sd.menu_time--;/* �˵��˳�����ʱ */
				LCD_ShowNum(150,154,GRAY0,BLACK,3,24, sd.menu_time/60);/* ����ʱʱ����ʾ */
			}
			if(sd.menu_time<1)ks.menu_key = MENU_OFF ;/* ����ʱʱ�䵽��˳��˵� */
			
		}
		else
		if(ks.menu_key == MENU_OFF && ks.F_11 ==0 )// �˳��˵�
		{
			if(lock ==1)
			{
				if(vfo[VFO_A].RxBand_id  == RADIO_MW && ks.rt_rx ==1)
				{
					ks.StepSelected_idx = T_STEP_9KHZ_IDX;
				}
				//else  
				if(vfo[VFO_A].RxBand_id != RADIO_MW && ks.rt_rx ==1)
				{
					ks.StepSelected_idx = T_STEP_5KHZ_IDX;
				}				
				if(ks.rt_rx ==0) ks.StepSelected_idx = T_STEP_1KHZ_IDX;
				ks.FunctionWindow_Use =0;//�๦�ܴ����ѽ��ռ��
				Lcd_Color(0, 128,319,239,BLACK) ;		// ˢ��ҳ��
				//Meun_Window(0, 124,80, GRAY0, BLACK);	// ���Ʋ˵�����
				sd.menu_time =AT24CXX_ReadLenByte(User[MENU ].addr, User[MENU].Len )*60;/* �˵���ʾʱ�� */
				sd.menu_time = Menu_user[0][0]*60;/* ����ʱʱ���ȡ��Ϊ��һ����׼�� */
				sd.IF_1 = Menu_user[0][1];/* ��ȡһ��Ƶ */
				sd.menu =0;
				//ks.Spe_key =0;
				lock = 0;
				//sd.menu_read =1;
			}
		}
	}
	Rx_amp_phase_adjuse();
}
/*
**************************************************************************************
*	Ƶ����ʾ�ٿ�
**************************************************************************************
*/
void key_Spectrum_Control()
{
	//u16 i;
	//static u8 clk =0;
	
	if(ks.menu_key == MENU_OFF && ks.F_11 ==0 )/* */
	{
		
//		if(clk ==0)
//		{
//			Lcd_Color(0, 130,319,239,BLACK) ;		// ˢ��ҳ��
//			//Meun_Window(0, 124,80, GRAY0, BLACK);	// ���Ʋ˵�����
//			
//			sd.menu_time = Menu_Values[0];/* ����ʱʱ���ȡ��Ϊ��һ����׼�� */
//			sd.IF_1 = Menu_Values[1];/* ��ȡһ��Ƶ */
//			sd.menu =0;
//			ks.Spe_key =0;
//			clk = 1;
//		}
		
		LCD_ShowString(257,131,YELLOW ,BLACK , 16, 0, "-");
		LCD_ShowString(290,131,YELLOW ,BLACK , 16, 0, "dbm");
		Spectrum_Dispaly(2,239, 319,57);
		//ui_Spe_Filter_Display(sd.spe_fil_id,sd.spe_fil_mode,clk0,ks.F_11);
		ui_SideFunction_Display();
	}
}
/*
**************************************************************************************
*	���ݶ�ȡ��ʼ��
**************************************************************************************
*/

void Data_init()
{
	u8 i;
	//int8_t addr_a,addr_b;
	sd.IF_1 = AT24CXX_ReadLenByte( MENU_IF_1,4 );/* һ��Ƶ��ȡ */
	if(sd.IF_1 >24000)sd.IF_1 = 24000;
	if(sd.IF_1 <1000)sd.IF_1=24000;
	
	for(i=0; i<10;i++)
	{
		
	}
//	ks.rt_rx = (AT24CXX_ReadOneByte(ADDR_A )>>2)&0X01;
//	sd.num_a = AT24CXX_ReadOneByte(CH_ADDR[AT24CXX_ReadOneByte(ADDR_A )]);// ��ȡ���ŵ����
//	if(ks.rt_rx )
//	{
//		vfo[VFO_A].RxBand_id = AT24CXX_ReadLenByte( BAND_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ],1 );// ��ȡ���ŵ� 
//		if(vfo[VFO_A].RxBand_id > RADIO_10M )vfo[VFO_A].RxBand_id = RADIO_10M;
//		if(vfo[VFO_A].RxBand_id < RADIO_MW )vfo[VFO_A].RxBand_id = RADIO_MW;
//		sd.band = vfo[VFO_A].RxBand_id;
//	}		
//	else 
//	{
//		vfo[VFO_A].Band_id = AT24CXX_ReadOneByte( BAND_CH(sd.num_a) + Addr_OFFSET[ks.rt_rx ] );// ��ȡ���ŵ� 
//		if(vfo[VFO_A].Band_id > COUPLING_6M)
//		{
//			vfo[VFO_A].Band_id = COUPLING_6M ;
//			vfo[VFO_A].Freq = Band_Info[COUPLING_6M].end ;
//		}
//		if(vfo[VFO_A].Band_id < COUPLING_80M)
//		{
//			vfo[VFO_A].Band_id = COUPLING_80M ;
//			vfo[VFO_A].Freq = Band_Info[COUPLING_80M].start;
//		}
//		sd.band = vfo[VFO_A].Band_id;
//	}
//	if(ks.rt_rx )ads.IF_gain[sd.band] =AT24CXX_ReadLenByte(ADDR_IF_RXGAIN+sd.band, 1);
//	else ads.IF_gain[sd.band] =AT24CXX_ReadLenByte(ADDR_IF_RXGAIN+sd.band, 1);
	
//	if(ads.IF_gain[sd.band] >63)ads.IF_gain[sd.band] =48;
//	if(ads.IF_gain[sd.band] < 1)ads.IF_gain[sd.band] =48;
//	sd.Riss = ads.IF_gain[sd.band];
//	WM8978_MIC_Gain(ads.IF_gain[sd.band]&0x3f);//MIC��������
	
	sd.Tcxo = AT24CXX_ReadLenByte( MENU_TCXO,4 );/**/
	if(sd.Tcxo >28000000)sd.Tcxo =28000000;
	if(sd.Tcxo <24000000)sd.Tcxo =24000000;
	
//	sd.Sql  = AT24CXX_ReadLenByte( MENU_SQL,1 );
//	if(sd.Sql >99)sd.Sql =99;
//	sd.CW_vol = AT24CXX_ReadLenByte( MENU_CW_VOL,1 );
//	if(sd.CW_vol >99)sd.CW_vol =99;
//	sd.CW_Sidetone = AT24CXX_ReadLenByte( MENU_CW_SI,2 );
//	if(sd.CW_Sidetone >900)sd.CW_Sidetone =900;
//	if(sd.CW_Sidetone <500)sd.CW_Sidetone =500;
//	sd.CW_Speed = AT24CXX_ReadLenByte( MENU_CW_SP,1 );

	//sd.Pow = AT24CXX_ReadLenByte( MENU_POW,1 );
	
	sd.agc_Start = AT24CXX_ReadLenByte( MENU_AGC_START,1 )*65536;
	sd.agc_ma = AT24CXX_ReadLenByte( MENU_AGC_M_A,1 );
	/**/
	//ks.mode_Reg = AT24CXX_ReadLenByte( ADDR_MODE_REG,1 );
	sd.Vol = AT24CXX_ReadLenByte( ADDR_VOL,1 );
	ks.StepSelected_idx = T_STEP_1KHZ_IDX;
	
    sd.cw_fil_id = AT24CXX_ReadLenByte(ADDR_FIL_CW ,1);
    if(sd.cw_fil_id >5)sd.cw_fil_id = 4;
	if(sd.cw_fil_id <1)sd.cw_fil_id = 4;
    //AT24CXX_WriteOneByte(ADDR_FIL_CW, sd.cw_fil_id ); 
    sd.lsb_fil_id = AT24CXX_ReadLenByte(ADDR_FIL_LSB ,1);
    if(sd.lsb_fil_id >5)sd.lsb_fil_id = 2;
	if(sd.lsb_fil_id <1)sd.lsb_fil_id = 2;
    //AT24CXX_WriteOneByte(ADDR_FIL_LSB, sd.lsb_fil_id ); 
    sd.usb_fil_id = AT24CXX_ReadLenByte(ADDR_FIL_USB ,1);
    if(sd.usb_fil_id >5)sd.usb_fil_id = 2;
	if(sd.usb_fil_id <1)sd.usb_fil_id = 2;
    //AT24CXX_WriteOneByte(ADDR_FIL_USB, sd.usb_fil_id );
    sd.am_fil_id = AT24CXX_ReadLenByte(ADDR_FIL_AM ,1);
    if(sd.am_fil_id > 1)sd.am_fil_id = 1;
	
    //AT24CXX_WriteOneByte(ADDR_FIL_AM, sd.am_fil_id );
    sd.fm_fil_id = AT24CXX_ReadLenByte(ADDR_FIL_FM ,1);
    if(sd.fm_fil_id > 1)sd.fm_fil_id = 1;
	
    //AT24CXX_WriteOneByte(ADDR_FIL_FM, sd.fm_fil_id );
	
	ks.AGC_Constant =1;
//	sd.txband_no = AT24CXX_ReadLenByte( ADDR_TX_BAND_NO,2 )&0x01;
//	addr_a = AT24CXX_ReadOneByte(ADDR_A );// ��ȡ���ŵ����
//	if(addr_a<1)addr_a =0;
//	if(addr_a>7)addr_a =7;
//	addr_b = AT24CXX_ReadOneByte(ADDR_B );// ��ȡ���ŵ����
//	if(addr_b<1)addr_b =0;
//	if(addr_b>7)addr_b =7;
//	
//	sd.num_a = AT24CXX_ReadOneByte(CH_ADDR[addr_a]);// ��ȡ���ŵ����
//	sd.num_b = AT24CXX_ReadOneByte(CH_ADDR[addr_b]);// ��ȡ���ŵ����
//	if(sd.num_a> 99) sd.num_a = 99;	// �綨����ŵ���ֵ
//	if(sd.num_a < 0) sd.num_a = 0;	// �綨��С�ŵ���ֵ
//	if(sd.num_b> 99) sd.num_b = 99;	// �綨����ŵ���ֵ
//	if(sd.num_b < 0) sd.num_b = 0;	// �綨��С�ŵ���ֵ
	
	//ks.rt_rx = (addr_a >>2)&0x01;
	//if(ks.rt_rx==0)ks.StepSelected_idx = T_STEP_1KHZ_IDX;
	
	//
	for(i=0; i<10;i++)
	{
		ads.pow_gain [i] = AT24CXX_ReadLenByte(i*2 + ADDRPOWER_GAIN ,2);
		if(ads.pow_gain [i]<1000)ads.pow_gain[i] = 10000;
		if(ads.pow_gain [i]>POW_GAIN_MAX)ads.pow_gain[i] = 10000;
		AT24CXX_WriteLenByte(i*2 + ADDRPOWER_GAIN , ads.pow_gain [i],2);
//		if(ads.cw_gain[i]>10000)ads.cw_gain[i] =10000;
//		if(ads.cw_gain[i]<1)ads.cw_gain[i] =1;
	}
	/* ��ȡAT24C16�е����� */
	for(i=0; i< MENU_END ;i++)
	{
		Menu_user[0][i] = AT24CXX_ReadLenByte(User[i].addr, User[i].Len );
		if(Menu_user[0][i] < User[i].start )Menu_user[0][i] = User[i].Default_fr ;
		if(Menu_user[0][i] >User[i].end)Menu_user[0][i] = User[i].Default_fr ;	
		AT24CXX_WriteLenByte( User[i].addr, Menu_user[0][i], User[i].Len);
	}	
	/* �˵����ݴ��뻺�� */
	sd.menu_time =Menu_user[0][0]*60;
	sd.IF_1 = Menu_user[0][1]; 		/* һ��Ƶ���� */
	sd.CW_Delay = Menu_user[0][2]*10;	/* CW�Զ������� */
	ks.AGC_Constant = Menu_user[0][3];		/* AGC��������� */
	ks.bt_key = Menu_user[0][4];		/* �������� */
	sd.ritatt_ptt = Menu_user[0][5];
	sd.enc_freq = Menu_user[0][6];
	//sd.enc_exti = Menu_user[0][7];
	sd.tx_filter = Menu_user[0][7];
	sd.tx_af_comp =  Menu_user[0][8];
	//ks.sim_dif =  Menu_user[0][8];
	//AT24CXX_WriteLenByte( MENU_ENC_EXTI  ,Menu_user[0][8] ,1);
	/*
	*	�����ǿ�������Ŀ
	*/
	for(i=0; i<  MENU_HIDD_END;i++)
	{
		Menu_user[1][i] = AT24CXX_ReadLenByte(Deve[i].addr, Deve[i].Len );
		if(Menu_user[1][i] > Deve[i].end )Menu_user[1][i] =  Deve[i].Default_fr ;
		if( Menu_user[1][i] <  Deve[i].start )Menu_user[1][i] =  Deve[i].Default_fr ;
		AT24CXX_WriteLenByte(  Deve[i].addr,  Menu_user[1][i],  Deve[i].Len);					
	}	
	ads.pow_corre  =Menu_user[1][0]*0.00001f;
	sd.agc_Start = Menu_user[1][1]*65536;	/* AGC��ص���� */
	sd.S_correct = Menu_user[1][2];
	sd.Tcxo  = Menu_user[1][3];		/* �²�����Ƶ��У׼ */
	sd.TX_spe = Menu_user[1][4];
	 Menu_user[1][5] =0;
	sd.Rx_amp_ma = Menu_user[1][5];
	AT24CXX_WriteLenByte( MENU_RX_PHASE ,Menu_user[1][5], 1);
	sd.alc_start = Menu_user[1][6];
	//Menu_user[1][7] =60;
	sd.alc_mic = Menu_user[1][7];
	//AT24CXX_WriteLenByte( MENU_ALC_MIC ,Menu_user[1][7] ,1);
	
	ads.tx_amp[0] = Menu_user[1][8]*0.001f;
	ads.tx_phase[0] = (Menu_user[1][9]-100) *0.001f;
	ads.tx_amp[1] = Menu_user[1][10]*0.001f;
	ads.tx_phase[1] = (Menu_user[1][11]-100) *0.001f;
	ads.tx_amp[2] = Menu_user[1][12]*0.001f;
	ads.tx_phase[2] = (Menu_user[1][13]-100) *0.001f;
	ads.tx_amp[3] = Menu_user[1][14]*0.001f;
	ads.tx_phase[3] = (Menu_user[1][15]-100) *0.001f;
	ads.tx_amp[4] = Menu_user[1][16]*0.001f;
	ads.tx_phase[4] = (Menu_user[1][17]-100) *0.001f;
	ads.tx_amp[5] = Menu_user[1][18]*0.001f;
	ads.tx_phase[5] = (Menu_user[1][19]-100) *0.001f;
	ads.tx_amp[6] = Menu_user[1][20]*0.001f;
	ads.tx_phase[6] = (Menu_user[1][21]-100) *0.001f;
	ads.tx_amp[7] = Menu_user[1][22]*0.001f;
	ads.tx_phase[7] = (Menu_user[1][23]-100) *0.001f;
	ads.tx_amp[8] = Menu_user[1][24]*0.001f;
	ads.tx_phase[8] = (Menu_user[1][25]-100) *0.001f;
	ads.tx_amp[9] = Menu_user[1][26]*0.001f;
	ads.tx_phase[9] = (Menu_user[1][27]-100) *0.001f;
	//
	for(i=0; i< 10;i++)
	{
		Rx_amp_user[i] = AT24CXX_ReadLenByte(ADDR_RX_BAND_PHASE_M+i*3, 2);
		Rx_phase_user[i] = AT24CXX_ReadLenByte(ADDR_RX_BAND_PHASE_M+i*3+2, 1);
		if(Rx_phase_user[i]<20)Rx_phase_user[i]=100;
		if(Rx_phase_user[i]>250)Rx_phase_user[i]=100;
		if(Rx_amp_user[i]<200)Rx_amp_user[i]=1000;
		if(Rx_amp_user[i]>2000)Rx_amp_user[i]=1000;
		ads.rx_amp[i] = Rx_amp_user[i]*0.001f;
		ads.rx_phase[i] = (Rx_phase_user[i]-100)*0.001f;
	
		if(sd.menu_page >0)
		{
			sd.Pow =50;
			//AT24CXX_WriteOneByte(ADDRPOW_MM+i, sd.Pow);
		}
		else
		{
			if(i<9)
			{
				sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+i, 1);
				//sd.Pow = AT24CXX_ReadLenByte( MENU_POW,1 );
				if(sd.Pow>200)sd.Pow =200;
			}
			else
			{
				sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+9, 1);
				if(sd.Pow>50)sd.Pow =50;
				//AT24CXX_WriteOneByte(ADDRPOW_MM+i, sd.Pow);
			}
		}
		//ads.pow_amp[i]=0;
		//ads.pow_amp[i] = AT24CXX_ReadLenByte( MENU_TX_AMP_MM+i*4,4);
	}
	//
//	for(i=0; i< 12;i++)
//	{
//		Rx_amp_adjust[ads.amp_adjust_num] = AT24CXX_ReadLenByte( MENU_RX_AMP_JUSET+i*2, 2);
//		if(Rx_amp_adjust[ads.amp_adjust_num]<200)Rx_amp_adjust[ads.amp_adjust_num]=1000;
//		if(Rx_amp_adjust[ads.amp_adjust_num]>2000)Rx_amp_adjust[ads.amp_adjust_num]=1000;
//		AT24CXX_WriteLenByte( MENU_RX_AMP_JUSET+i*2, Rx_amp_adjust[ads.amp_adjust_num],2);
//		//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
//	}
	//
	sd.Sql = AT24CXX_ReadLenByte( MENU_SQL, 1 );
	if(sd.Sql > 99)sd.Sql =99;
	if(sd.Sql < 1)sd.Sql =0;
	
	sd.CW_vol = AT24CXX_ReadLenByte( MENU_CW_VOL, 1 );
	if(sd.CW_vol >32)sd.CW_vol =5;
	if(sd.CW_vol<1)sd.CW_vol =5;
	
	sd.CW_Sidetone = AT24CXX_ReadLenByte( MENU_CW_SI, 2 );
	if(sd.CW_Sidetone < 500)sd.CW_Sidetone =650;
	if(sd.CW_Sidetone >990)sd.CW_Sidetone =650;
	
	sd.CW_Speed = AT24CXX_ReadLenByte( MENU_CW_SP, 1 );
	if(sd.CW_Speed >50)sd.CW_Speed =20;
	if(sd.CW_Speed < 5)sd.CW_Speed =20;
	ks.CW_keySW = 0;
//	sd.Pow = AT24CXX_ReadLenByte(MENU_POW, 1 );
//	if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
//	{
//		if(sd.Pow >100)sd.Pow =100;
//	}
//	else if(sd.Pow >200)sd.Pow =200;
//	if(sd.Pow < 1)sd.Pow =1;
				
	sd.mic_gain = AT24CXX_ReadLenByte( MENU_MIC_GAIN, 1 );
	if(sd.mic_gain >99)sd.mic_gain =20;
	if(sd.mic_gain < 1)sd.mic_gain =20;
	
	nr.dsp_nr_strength = AT24CXX_ReadLenByte( MENU_NR_STRONG, 1 );
	if(nr.dsp_nr_strength >55)nr.dsp_nr_strength =15;
	if(nr.dsp_nr_strength < 1)nr.dsp_nr_strength =15;
				
	nr.dsp_nr_delaybuf_len = AT24CXX_ReadLenByte( MENU_NR_BUFLEN, 2 );
	if(nr.dsp_nr_delaybuf_len >512)nr.dsp_nr_delaybuf_len =60;
	if(nr.dsp_nr_delaybuf_len < 32)nr.dsp_nr_delaybuf_len =60;
	
	nr.dsp_nr_numtaps = 32;//AT24CXX_ReadLenByte( MENU_NR_NUMTAPS, 1 );
	
	for(i=0; i<10;i++)
	{
		Filte_Center[i] = filte_center[i].Default;
	}
	sd.Dsp_Bfo = filte_center[vfo[VFO_A].FilterSelected_id].Default;
	
	//nr.dsp_inhibit_mute = AT24CXX_ReadLenByte( MENU_NOT_STRONG, 1 );
	
	//nr.dsp_notch_delaybuf_len = AT24CXX_ReadLenByte( MENU_NOT_BUFLEN, 2 );
	
	//nr.dsp_notch_numtaps = AT24CXX_ReadLenByte( MENU_NOT_NUMTAPS, 1 );
	//sd.tx_amp= Menu_Values[13];
	//sd.tx_amp = AT24CXX_ReadLenByte(MENU_TX_AMP, 2 );
	//if(sd.tx_amp <5000 || sd.tx_amp>20000)sd.tx_amp =10000;
	//ads.tx_amp = sd.tx_amp *0.0001f;
	pow_gain_set();//�趨�����¸������ε�����ϵ��
	ks.cat_ptt =RX;
	ks.ATT_key =0;
	ks.rit_tx =RX;
	ui_Mute_icon(220,4,RED,0);
	//sd.amp_set =1;
}
/*
**************************************************************************************
*	Enc�������ٿع���
**************************************************************************************
*/
/*
*	Ƶ�ʵ��ڱ�����Enc0
*/
void Enc0_UseManagement(void)
{
	 int16_t enc0,enc1;
	//static u8 step=1;
	static int16_t enc_0,enc_1;
	static u8 clk0=0;
//	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
//	{
//		if( ucKeyCode==KEY_12_LONG )//������ť��������
//		{
//			ks.EC1_12_LONG = ~ks.EC1_12_LONG &0x01;
//			//if(ks.EC1_12_LONG >1)ks.EC1_12_LONG =0;
//		}
//		if( ucKeyCode==KEY_12_UP )//������ť�����̰���
//		{
//			ks.EC1_12 +=1;
//			if(ks.EC1_12 >3)ks.EC1_12 =0;
//			sd.Enc2_delay = 100;
//		}
//	}
	//
	if(ks.key_lock==0)
	{
//		enc0 = EncoderRead( ENCFREQ ,1);
//		enc1 = EncoderRead( ENC1,1 );
//		if(enc1!=enc_1 )
//		{
//			sd.Enc0_Value = enc1;
//			enc_1=enc1;
//		}
//		else
//		if(enc0!=enc_0 )
//		{
//			sd.Enc0_Value = enc0;
//			enc_0=enc0;
//		}			
		/* 
		*	������0ʹ��Ȩ����
		*/
		if(ks.F_11 ==0 && ks.menu_key ==0 && PIN_K13 ==1&&sd.amp_phase ==0)
		{
			if(ks.RIT_key  ==0)
			{
				ks.Enc0_Use =0;
				//sd.rit =0;
				//vfo[VFO_A].Freq += sd.Enc0_Value * sd.tuning_step ;/* ��Ƶ�ʵ��� */
			}
			else 
			{
				if( sd.band_set_delay<1)ks.Enc0_Use =1;
				
//				sd.rit += sd.Enc0_Value ;/* RITƵ��΢�� */
//				if(sd.rit > 1000)sd.rit =1000;
//				if(sd.rit < -1000)sd.rit =-1000;
			}
		}
		else
		if(ks.F_11 ==0 && ks.menu_key ==0 && PIN_K13 ==0)	
		{
			ks.Enc0_Use =2;
//			if(ks.ch_vfo ==1 )
//			{
//				sd.num_a += sd.Enc0_Value ; 	/* �ŵ����ѡ�� */
//				if(sd.num_a> 99) sd.num_a = 99;	// �޶�����ŵ���ֵ
//				if(sd.num_a <0) sd.num_a = 0;	// �޶�����ŵ���ֵ
//				//AT24CXX_WriteOneByte(CH_ADDR[addr_a], sd.num_a);
//			}
//			else
//			{
//				ks.StepSelected_idx  -= sd.Enc0_Value ;/* ����ѡ�� */
//				if(ks.StepSelected_idx > T_STEP_100KHZ_IDX)
//					ks.StepSelected_idx = T_STEP_100KHZ_IDX;
//				if(ks.StepSelected_idx < T_STEP_10HZ_IDX)
//					ks.StepSelected_idx = T_STEP_10HZ_IDX;
//			}
			//AT24CXX_WriteOneByte(CH_ADDR[addr_a], sd.num_a);
		}
		else
		if(ks.F_11 ==0 && (ks.menu_key ==1||sd.amp_phase==1 ) && PIN_K13 ==1)	
		{
			ks.Enc0_Use =3;
//			Menu_Values[sd.menu] +=sd.Enc0_Value * sd.tuning_step;/* �˵�ѡ��������� */
//			/* �˵����ݴ洢 */
//			AT24CXX_WriteLenByte( Menu[sd.menu].addr  ,Menu_Values[sd.menu]   ,Menu[sd.menu].Len);
		}
		else
		if(ks.F_11 ==0 && ks.menu_key ==1 && PIN_K13 ==0)	
		{
			ks.Enc0_Use =4;
//			ks.StepSelected_idx  -= sd.Enc0_Value ;	/* ����ѡ�� */
//			if(ks.StepSelected_idx > T_STEP_1MHZ_IDX)
//				ks.StepSelected_idx = T_STEP_1MHZ_IDX;
//			if(ks.StepSelected_idx < T_STEP_1HZ_IDX)
//				ks.StepSelected_idx = T_STEP_1HZ_IDX;
		}
		else
		if(ks.F_11 ==1 && ks.menu_key ==0 && PIN_K13 ==1)	
		{
			ks.Enc0_Use =5;
			//ks.Band_id += sd.Enc0_Value; /* �շ�ģʽ����ѡ�������δʵ�֣�*/
		}
		else
		if(ks.F_11 ==1 && ks.menu_key ==0 && PIN_K13 ==0)	
		{
			ks.Enc0_Use =6;
			//ks.RxBand_id += sd.Enc0_Value;/* ���Ż�ģʽ����ѡ�������δʵ�֣�*/
		}
		/* ��������ȡ */
		if(ks.Enc0_Use  ==0)//Ƶ�ʵ���ʱ���⴦�� ����ʱX1,��� X100;
		{
			enc0 = EncoderRead( ENCFREQ ,sd.enc_freq );
			enc1 = EncoderRead( ENC1,sd.enc_exti );
		}
		else
		{
			enc0 = EncoderRead( ENCFREQ ,0);
			enc1 = EncoderRead( ENC1,0 );
		}
		if(enc1!=enc_1 )
		{
			sd.Enc0_Value = enc1;
			enc_1=enc1;
		}
		else
		if(enc0!=enc_0 )
		{
			sd.Enc0_Value = enc0;
			enc_0=enc0;
		}	
		/*
		*	���ݵ���
		*/
		if(sd.Enc0_Value != 0)
		{
			
			if(ks.Enc0_Use  ==0&&clk0==1)
			{
				//sd.rit =0;
				vfo[VFO_A].Freq += sd.Enc0_Value * sd.tuning_step;/* ��Ƶ�ʵ��� */
			}
			//else
			if(ks.Enc0_Use  ==1&&clk0==1)	
			{
				sd.rit += sd.Enc0_Value *10;//* 10;/* RITƵ��΢�� */
				if(sd.rit > 1200)sd.rit =1200;
				if(sd.rit < -1200)sd.rit =-1200;
				sd.rit_delay =VOL_WINDOW_DELAY*2;
			}
			//else	
			if(ks.Enc0_Use  ==2)
			{
				if(ks.ch_vfo ==1 )
				{
					sd.num_a += sd.Enc0_Value ; 	/* �ŵ����ѡ�� */
					if(sd.num_a> 99) sd.num_a = 99;	// �޶�����ŵ���ֵ
					if(sd.num_a <1) sd.num_a = 0;	// �޶�����ŵ���ֵ
					//AT24CXX_WriteOneByte(CH_ADDR[addr_a], sd.num_a);
				}
				else
				{
					ks.StepSelected_idx  -= sd.Enc0_Value ;/* ����ѡ�� */
					if(ks.StepSelected_idx > T_STEP_100KHZ_IDX)
						ks.StepSelected_idx = T_STEP_100KHZ_IDX;
					if(ks.StepSelected_idx < T_STEP_10HZ_IDX)
						ks.StepSelected_idx = T_STEP_10HZ_IDX;
				}
				//AT24CXX_WriteOneByte(CH_ADDR[addr_a], sd.num_a);
			}
			//else
			if(ks.Enc0_Use  ==3)	
			{
				if(sd.amp_phase )
				{
					//if(sd.menu==2)Rx_amp_adjust[ads.amp_adjust_num] +=sd.Enc0_Value;
					//else
					if( sd.menu==1) Rx_phase_user[ads.rx_amp_band] +=sd.Enc0_Value ;/* �˵�ѡ��������� */
					if( sd.menu==0)  Rx_amp_user[ads.rx_amp_band] +=sd.Enc0_Value ;/* �˵�ѡ��������� */
					//if(Rx_amp_adjust[ads.amp_adjust_num]<200)Rx_amp_adjust[ads.amp_adjust_num]=1000;
					//if(Rx_amp_adjust[ads.amp_adjust_num]>2000)Rx_amp_adjust[ads.amp_adjust_num]=1000;
					if(Rx_phase_user[ads.rx_amp_band]<20)Rx_phase_user[ads.rx_amp_band]=100;
					if(Rx_phase_user[ads.rx_amp_band]>250)Rx_phase_user[ads.rx_amp_band]=100;
					if(Rx_amp_user[ads.rx_amp_band]<200)Rx_amp_user[ads.rx_amp_band]=1000;
					if(Rx_amp_user[ads.rx_amp_band]>2000)Rx_amp_user[ads.rx_amp_band]=1000;
					
					//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
					ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
					ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
					/* �˵����ݴ洢 */
					AT24CXX_WriteLenByte( ads.rx_amp_band*3+ADDR_RX_BAND_PHASE_M  , Rx_amp_user[ads.rx_amp_band],2);
					AT24CXX_WriteLenByte( ads.rx_amp_band*3+2+ADDR_RX_BAND_PHASE_M  , Rx_phase_user[ads.rx_amp_band],1);
					//AT24CXX_WriteLenByte(  MENU_RX_AMP_JUSET + ads.amp_adjust_num*2, Rx_amp_adjust[ads.amp_adjust_num],2);
				}
				else 
				{
					Menu_user[sd.menu_page][sd.menu] +=sd.Enc0_Value * sd.tuning_step;/* �˵�ѡ��������� */
					if(sd.menu_page==0)
					{
						if(Menu_user[0][sd.menu]<User[sd.menu].start )Menu_user[0][sd.menu] = User[sd.menu].Default_fr;
						//else
						if(Menu_user[0][sd.menu]>User [sd.menu].end )Menu_user[0][sd.menu] = User[sd.menu].Default_fr;
						/* �˵����ݴ洢 */
						AT24CXX_WriteLenByte( User[sd.menu].addr  ,Menu_user[0][sd.menu] ,User[sd.menu].Len);
					}
					else
					{
						//if(Menu_user[0][sd.menu]<User[sd.menu].start )Menu_user[0][sd.menu] = User[sd.menu].start;
						if(Menu_user[1][sd.menu]<Deve[sd.menu].start )Menu_user[1][sd.menu] = Deve[sd.menu].Default_fr;
						//else
						//if(Menu_user[0][sd.menu]>User [sd.menu].end )Menu_user[0][sd.menu] = User[sd.menu].Default_fr;
						if(Menu_user[1][sd.menu]>Deve [sd.menu].end )Menu_user[1][sd.menu] = Deve[sd.menu].Default_fr;
						/* �˵����ݴ洢 */
						//AT24CXX_WriteLenByte( User[sd.menu].addr  ,Menu_user[0][sd.menu] ,User[sd.menu].Len);
						AT24CXX_WriteLenByte( Deve[sd.menu].addr  ,Menu_user[1][sd.menu] ,Deve[sd.menu].Len);
					}
					//sd.CW_Speed = Menu_Values[7];
					//ads.cw_spe = 1300  / sd.CW_Speed;
					
					/* �˵����ݴ��뻺�� */
					if(sd.menu_page==0)
					{
						sd.menu_time =Menu_user[0][0]*60;
						sd.IF_1 = Menu_user[0][1]; 		/* һ��Ƶ���� */
						sd.CW_Delay = Menu_user[0][2]*10;	/* CW�Զ������� */
						ks.AGC_Constant = Menu_user[0][3];		/* AGC��������� */
						ks.bt_key = Menu_user[0][4];		/* �������� */
						sd.ritatt_ptt = Menu_user[0][5];
						sd.enc_freq = Menu_user[0][6];
						//sd.enc_exti = Menu_user[0][7];
						sd.tx_filter = Menu_user[0][7];
						//ks.sim_dif =  Menu_user[0][8];
					}
					/*
					*	�����ǿ�������Ŀ
					*/
					else
					{
						ads.pow_corre =Menu_user[1][0]*0.00001f;//����У׼
						sd.agc_Start = Menu_user[1][1]*65536;	/* AGC��ص���� */
						sd.S_correct = Menu_user[1][2];			//S��У׼
						sd.Tcxo  = Menu_user[1][3];		/* �²�����Ƶ��У׼ */
						sd.TX_spe = Menu_user[1][4];	//����Ƶ�׿���
						sd.Rx_amp_ma = Menu_user[1][5];	//����ƽ���ֶ�/�Զ�ģʽת��
						sd.alc_start = Menu_user[1][6];
						sd.alc_mic = Menu_user[1][7];
						sd.tx_af_comp =  Menu_user[0][8];
					
						ads.tx_amp[0] = Menu_user[1][8]*0.001f;
						ads.tx_phase[0] = (Menu_user[1][9]-100) *0.001f;
						ads.tx_amp[1] = Menu_user[1][10]*0.001f;
						ads.tx_phase[1] = (Menu_user[1][11]-100) *0.001f;
						ads.tx_amp[2] = Menu_user[1][12]*0.001f;
						ads.tx_phase[2] = (Menu_user[1][13]-100) *0.001f;
						ads.tx_amp[3] = Menu_user[1][14]*0.001f;
						ads.tx_phase[3] = (Menu_user[1][15]-100) *0.001f;
						ads.tx_amp[4] = Menu_user[1][16]*0.001f;
						ads.tx_phase[4] = (Menu_user[1][17]-100) *0.001f;
						ads.tx_amp[5] = Menu_user[1][18]*0.001f;
						ads.tx_phase[5] = (Menu_user[1][19]-100) *0.001f;
						ads.tx_amp[6] = Menu_user[1][20]*0.001f;
						ads.tx_phase[6] = (Menu_user[1][21]-100) *0.001f;
						ads.tx_amp[7] = Menu_user[1][22]*0.001f;
						ads.tx_phase[7] = (Menu_user[1][23]-100) *0.001f;
						ads.tx_amp[8] = Menu_user[1][24]*0.001f;
						ads.tx_phase[8] = (Menu_user[1][25]-100) *0.001f;
						ads.tx_amp[9] = Menu_user[1][26]*0.001f;
						ads.tx_phase[9] = (Menu_user[1][27]-100) *0.001f;
					}
				}
			}
			//else
			if(ks.Enc0_Use  ==4)	
			{
				ks.StepSelected_idx  -= sd.Enc0_Value ;	/* ����ѡ�� */
				if(ks.StepSelected_idx > T_STEP_1MHZ_IDX)
					ks.StepSelected_idx = T_STEP_1MHZ_IDX;
				if(ks.StepSelected_idx < T_STEP_1HZ_IDX)
					ks.StepSelected_idx = T_STEP_1HZ_IDX;
			}
			//else
			if(ks.Enc0_Use  ==5)	
			{
				sd.band_set_delay =100;
				if(ks.rt_rx ==0)
				{
					vfo[VFO_A].Band_id += sd.Enc0_Value; /* �շ�ģʽ����ѡ�������δʵ�֣�*/
					if(vfo[VFO_A].Band_id >  COUPLING_6M)vfo[VFO_A].Band_id =  COUPLING_80M;
					if(vfo[VFO_A].Band_id <  COUPLING_80M)vfo[VFO_A].Band_id =  COUPLING_6M;

                    if(vfo[VFO_A].Band_id <  COUPLING_30M)vfo[VFO_A].Mode =  DEMOD_LSB;
                    else vfo[VFO_A].Mode =  DEMOD_USB;
					
				}
				else 
				{
					vfo[VFO_A].RxBand_id += sd.Enc0_Value; /* �շ�ģʽ����ѡ�������δʵ�֣�*/
					if(vfo[VFO_A].RxBand_id > RADIO_10M)vfo[VFO_A].RxBand_id = RADIO_90M;
					if(vfo[VFO_A].RxBand_id < RADIO_90M)vfo[VFO_A].RxBand_id = RADIO_10M;
				}
			}
			//else
			if(ks.Enc0_Use  ==6)	
			{
				//ks.Enc0_Use =6;
				//ks.RxBand_id += sd.Enc0_Value;/* ���Ż�ģʽ����ѡ�������δʵ�֣�*/
			}
		}
		if(clk0==0)clk0=1;
	}
}
/*
*	�������ڱ����� Enc2
*/
void pow_gain_set()
{
	float32_t calc;
	/*
	*	P=V*V/R,V=P*R��ƽ��
	*	���ݹ��ʹ�ʽ��sd.PowӦ���ǳ���50��
	*	�˵��궨�Ĺ��ʷŴ���ʮ����������sd.Pow*5.0f	
	*/
	
	//ads.cw_gain[3] =calc; 
	/*	�����趨�����¸�Ƶ������ϵ��	*/
	arm_sqrt_f32 (sd.Pow, (float32_t *)&ads.cw_gain[0]);//����궨�����µĵ�ѹֵ
	//ads.cw_gain[1] =ads.pow_gain [vfo[VFO_A].Band_id ] *ads.cw_gain[0]*50.0f;
	if(sd.Pow <=50)ads.cw_gain[1] =((1500-sd.Pow*30)+ads.pow_gain [vfo[VFO_A].Band_id ]) *ads.cw_gain[0]*10.0f;
	else 
	if(sd.Pow>50)ads.cw_gain[1] =((sd.Pow -50)*30+ads.pow_gain [vfo[VFO_A].Band_id ]) *ads.cw_gain[0]*10.0f;
	
	//ads.cw_gain[0] =calc;
	//if(sd.Pow <=50)ads.cw_gain[4] =(100-sd.Pow*2) *calc*50.0f;
	//else 
	//if(sd.Pow>50)ads.cw_gain[4] =(sd.Pow -50)*1.2f *calc*50.0f;
	
	//ads.cw_gain[3] =ads.pow_gain [vfo[VFO_A].Band_id ] *50.0f;
	
	/*	�����趨��������С���ʵ������ֵ	*/
	arm_sqrt_f32 (sd.Pow, (float32_t *)&calc);
	arm_sqrt_f32 (calc, (float32_t *)&calc);
	arm_sqrt_f32 (calc, (float32_t *)&calc);
	arm_sqrt_f32 (calc, (float32_t *)&calc);
	arm_sqrt_f32 (calc , (float32_t *)&ads.cw_gain[2]);
	//ads.cw_gain[0] = ads.pow_gain [vfo[VFO_A].Band_id ];
	//arm_sqrt_f32 (sd.Pow/50, (float32_t *)&ads.cw_gain[3]);
	//ads.pow_amp[vfo[VFO_A].Band_id] = AT24CXX_ReadLenByte( MENU_TX_AMP_MM+vfo[VFO_A].Band_id*4,4);
}
//
Power_addr_GenInfo	Power_max[]=
{
	{5,	 50,	1000},//80M
	{5,	 50,	1000},//60M
	{5,	 50,	1000},//40M
	{5,	 50,	1000},//30M
	{5,	 70,	1000},//20M
	{5,	 70,	1000},//17M
	{5,	 80,	1000},//15M
	{5,	 90,	1000},//12M
	{5,	100,	1000},//10M
	{5,	120,	1000} //6M
};
void Enc2_UseManagement(void)
{
	//static u16 Timing;
	static u8  clk0=0;
	//static u8  temp;
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
	{
		if( ucKeyCode==KEY_12_LONG )//������ť��������
		{
			ks.EC1_12_LONG = ~ks.EC1_12_LONG &0x01;
			sd.Enc2_delay = VOL_WINDOW_DELAY;
			if(ks.EC1_12_LONG==0)sd.Enc2_delay =0;
		}
		
		if( ucKeyCode==KEY_12_UP )//������ť�����̰���
		{
			ks.EC1_12 +=1;
			if(ks.EC1_12_LONG ==1 || (ks.NR_key==0 && vfo[VFO_A].Mode >= DEMOD_LSB))
			{
				if(ks.EC1_12 >1)ks.EC1_12 =0;
			}
			else
			if(ks.EC1_12_LONG ==0 )
			{
				if(ks.EC1_12 >3)ks.EC1_12 =0;
			}
			sd.Enc2_delay = VOL_WINDOW_DELAY;
			if(ks.EC1_12 ==0)sd.Enc2_delay =0;
		}
		if( ucKeyCode==KEY_10_LONG && ks.rx_adjuset_key ==0)//�˲�����������
		{
			ks.IF_shift =~ks.IF_shift&0x01;
			if(ks.IF_shift ==0)sd.Enc2_delay =0;
		}
	}
	if(ks.key_lock==0)
	{
		//sd.Enc2_Value = EncoderRead( ENC2 );/* ��ȡ���������� */
			/* ������ʹ��Ȩ���� */
		if(ks.F_11 ==0 && ks.EC1_12_LONG ==1 )
		{
			ks.Enc2_Use =1;
		}
		//else
		if(ks.F_11 ==0 && ks.EC1_12_LONG ==0 )
		{
			if(ks.EC1_12 ==0  )
			{
				if(ks.PWR_LONG ==0 && ks.PWR_key ==0 && ks.RIT_key ==0 )
				{
					//sd.Enc2_delay =100;/* �Զ��˳���ʱ�趨 */
					ks.Enc2_Use =0;/* �������� */
				}
				if( ks.menu_key ==0 && TR_READ ==0 && ks.agc_m_adjust ==0&&sd.amp_phase ==0)
				{
					//sd.Enc2_delay =100;/* �Զ��˳���ʱ�趨 */
					ks.Enc2_Use =0;/* �������� */
				}
				//else
				if(ks.PWR_key ==1)// && TR_READ ==CONTROL_RX)
				{
					//sd.Enc2_delay = 300;
					ks.Enc2_Use =2;/* ���ʵ��� */
					//ks.RIT_key =0;
					ks.agc_m_adjust =0;
					ks.menu_key =0;
					ks.PWR_LONG =0;
					
					if(sd.Enc2_delay ==0)ks.PWR_key =0;
				}
				//else
				if(ks.menu_key ==1||sd.amp_phase ==1)
				{
					ks.Enc2_Use =3;/* �˵�ѡ��ѡ�� */
					//ks.RIT_key =0;
					ks.PWR_key =0;
					ks.PWR_LONG =0;
					ks.agc_m_adjust =0;
					//ks.ATT_LONG =0;
//						if(Timing ==0)
//						{
//							ks.RIT_key =0;
//						}
				}
				//else
				if(ks.PWR_key ==0 && TR_READ ==CONTROL_TX&&ks.menu_key ==0&&sd.amp_phase ==0)
				{
					ks.Enc2_Use =4;/* �˵�ѡ��ѡ�� */
					//ks.RIT_key =0;
					ks.PWR_key =0;
					ks.menu_key =0;
					ks.agc_m_adjust =0;
				}
				if(ks.agc_m_adjust ==1 && TR_READ ==CONTROL_RX)
				{
					if(clk0 )
					{
						sd.Enc2_delay =VOL_WINDOW_DELAY;
						clk0=0;
					}
					ks.Enc2_Use =5;/* �ֶ�AGC���� */
					//ks.RIT_key =0;
					ks.PWR_key =0;
					ks.menu_key =0;
					ks.PWR_LONG =0;
				}
				//if(sd.Enc2_delay <1)clk0 =1;
				if(ks.IF_shift ==1 && TR_READ ==CONTROL_RX)
				{
					if(clk0 )
					{
						//sd.Enc2_delay =VOL_WINDOW_DELAY;
						clk0=0;
					}
					ks.Enc2_Use =6;/* SSB�˲������ĵ��� */
					//ks.RIT_key =0;
					ks.PWR_key =0;
					ks.menu_key =0;
					ks.PWR_LONG =0;
					ks.agc_m_adjust =0;
				}
//				if(ks.menu_key ==1&& TR_READ ==CONTROL_TX)
//				{
//					ks.Enc2_Use =7;/*����I/Qƽ����� */
//					ks.RIT_key =0;
//					ks.PWR_key =0;
//					//ks.menu_key =0;
//					ks.PWR_LONG =0;
//					ks.agc_m_adjust =0;
//					ks.IF_shift =0;
//				}
				if(sd.Enc2_delay <1)clk0 =1;
			}
			//else 
			if(ks.F_11 ==0 && ks.EC1_12 >0 && vfo[VFO_A].Mode >= DEMOD_LSB )
			{
//				if(ks.NR_key<1 )
//				{
//					if(ks.EC1_12 >1)ks.EC1_12 =0;
//				}
//				else
//				{
//					if(ks.EC1_12 >3)ks.EC1_12 =0;
//				}
				//sd.Enc2_delay =300;
				if(ks.EC1_12 ==1  )
				{
					//sd.Enc2_delay =300;
					ks.Enc2_Use =10;
//					sd.mic_gain   += sd.Enc2_Value;/* ��˷�������� */
//					if(sd.mic_gain >99)sd.mic_gain =99;
//					if(sd.mic_gain < 1)sd.mic_gain =1;
//					Menu_Values[13] = sd.mic_gain;
//					AT24CXX_WriteLenByte(MENU_MIC_GAIN, sd.mic_gain,1);
				}
				////else 
				if(ks.EC1_12 ==2 && ks.NR_key )
				{
					ks.Enc2_Use =11;
//					nr.dsp_nr_strength += sd.Enc2_Value;/* NR����ǿ�ȵ��� */
//					if(nr.dsp_nr_strength >99)nr.dsp_nr_strength =99;
//					if(nr.dsp_nr_strength <10)nr.dsp_nr_strength =10;
//					Menu_Values[14] = nr.dsp_nr_strength ;
//					AT24CXX_WriteLenByte(MENU_NR_STRONG , nr.dsp_nr_strength,1);
				}
				//else 
				if(ks.EC1_12 ==3 && ks.NR_key )
				{
					ks.Enc2_Use =12;
//					nr.dsp_nr_delaybuf_len += sd.Enc2_Value;/* NR�����ӳٳ��ȵ��� */
//					if(nr.dsp_nr_delaybuf_len >99)nr.dsp_nr_delaybuf_len =99;
//					if(nr.dsp_nr_delaybuf_len <10)nr.dsp_nr_delaybuf_len =10;
//					Menu_Values[15] = nr.dsp_nr_delaybuf_len;
//					AT24CXX_WriteLenByte(MENU_NR_BUFLEN  , nr.dsp_nr_delaybuf_len,2);
				}
			}
			//else 
			if(ks.F_11 ==0 && ks.EC1_12 >0 && vfo[VFO_A].Mode < DEMOD_LSB )
			{
				//sd.Enc2_delay =300;
				if(ks.EC1_12 ==1  )
				{
					//sd.Enc2_delay =300;
					ks.Enc2_Use =15;
//					sd.CW_vol += sd.Enc2_Value;/* ������������ */
//					if(sd.CW_vol >99)sd.CW_vol =99;
//					if(sd.CW_vol < 0)sd.CW_vol =0;
//					Menu_Values[5] = sd.CW_Sidetone;
//					AT24CXX_WriteLenByte(MENU_CW_VOL , sd.CW_vol,1);
				}
				//else 
				if(ks.EC1_12 ==2  )
				{
					ks.Enc2_Use =14;
//					sd.CW_Sidetone += sd.Enc2_Value;/* ����Ƶ�ʵ��� */
//					if(sd.CW_Sidetone >1000)sd.CW_Sidetone =1000;
//					if(sd.CW_Sidetone < 500)sd.CW_Sidetone =500;
//					Menu_Values[6] = sd.CW_Sidetone;
//					AT24CXX_WriteLenByte(MENU_CW_SI , sd.CW_Sidetone,2);
				}
				//else 
				if(ks.EC1_12 ==3  )
				{
					ks.Enc2_Use =13;
//					sd.CW_Speed += sd.Enc2_Value;/* �Զ����ٶȵ��� */
//					if(sd.CW_Speed >100)sd.CW_Speed =100;
//					if(sd.CW_Speed < 1)sd.CW_Speed =1;
//					Menu_Values[7] = sd.CW_Speed;
//					AT24CXX_WriteLenByte(MENU_CW_SP , sd.CW_Speed,1);
				}
			}
		}
		
		//
		sd.Enc2_Value = EncoderRead( ENC2 ,0);/* ��ȡ���������� */	
		if(sd.Enc2_Value !=0 &&  PIN_K12 ==1&& ks.F_11 ==0)
		{
			if(sd.amp_phase ==0&&ks.IF_shift ==0)sd.Enc2_delay =VOL_WINDOW_DELAY;
			//ks.agc_m_adjust =0;
			if(ks.Enc2_Use ==0 && ks.menu_key  ==0)
			{
				//ads.vol_timer =VOL_WINDOW_DELAY;
				sd.Vol += sd.Enc2_Value;/* �������� */
				if(sd.Vol >99)sd.Vol =99;
				if(sd.Vol < 1)sd.Vol =0;
				AT24CXX_WriteOneByte(ADDR_VOL, sd.Vol);
			}
			//else
			if(ks.Enc2_Use ==2)
			{
				if(sd.menu_page<1)
				{
					if(sd.Pow>10) 
					{
						sd.Pow += sd.Enc2_Value*10;/* ���ʵ��� */
						sd.Pow/=10;
						sd.Pow*=10;
					}
					else sd.Pow += sd.Enc2_Value;/* ���ʵ��� */
					if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode >= DEMOD_USB_DIG)
					{
						if(sd.Pow >100)sd.Pow =100;
					}
					else //if(sd.Pow >200)sd.Pow =200;
					{
						if(vfo[VFO_A].Band_id == COUPLING_6M)
						{
							if(sd.Pow >50)sd.Pow =50;
						}
						else if(sd.Pow >200)sd.Pow =200;
					}
					if(sd.Pow < 1)sd.Pow =1;
					
					//Menu_Values[9] = sd.Pow ;		/* ������޶� */
					//if(sd.menu_page<1)
					AT24CXX_WriteOneByte(ADDRPOW_MM+vfo[VFO_A].Band_id, sd.Pow);
					//AT24CXX_WriteOneByte(MENU_POW, sd.Pow );
				}
				else sd.Pow =50;
				pow_gain_set();
				//AT24CXX_WriteOneByte(MENU_POW, sd.Pow );
			}
			//else
			if(ks.Enc2_Use ==3 && (ks.menu_key  ==1||sd.amp_phase ==1))
			{
				sd.menu += sd.Enc2_Value ;/* �˵�ѡȡ */
				if(sd.amp_phase==1)
				{
					//sd.menu += 1;
					if( sd.menu > 1 )sd.menu = 0;
					if( sd.menu <0 )sd.menu = 1;
					ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
					ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
				}
				else
				{
					if( sd.menu > sd.menu_end -1 )sd.menu = 0;
					if( sd.menu < 0 )sd.menu =  sd.menu_end -1;
				}
				if(sd.menu_page==1)
				{
					if( sd.menu >=8)
					{
						ks.rt_rx =0;
						vfo[VFO_A].Band_id = (sd.menu -8)/2;
						vfo[VFO_A].Freq = AT24CXX_ReadLenByte( vfo[VFO_A].Band_id *4 + ADDR_RT_BAND_FREQ_M ,4 );
						ads.IF_gain =AT24CXX_ReadLenByte(ADDR_IF_GAIN+vfo[VFO_A].Band_id, 1);
						sd.Pow = 50;//AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
						pow_gain_set();
						if(ads.IF_gain >63)ads.IF_gain =32;
						if(ads.IF_gain < 1)ads.IF_gain =32;
					}
				}
				sd.menu_read =0;
			}
			//else
			if(ks.Enc2_Use ==4&& TR_READ ==CONTROL_TX)
			{
				//ads.pow_gain[vfo[VFO_A].Band_id ] = AT24CXX_ReadLenByte( Power_max[vfo[VFO_A].Band_id].addr,1 );
				if(ads.pow <= sd.Pow )ads.pow_gain[vfo[VFO_A].Band_id ] += sd.Enc2_Value*10;
				if(ads.pow > sd.Pow )ads.pow_gain[vfo[VFO_A].Band_id ] -=1;
				if(ads.pow_gain[vfo[VFO_A].Band_id ]<2 )	
					ads.pow_gain[vfo[VFO_A].Band_id ] = 2;
				if(ads.pow_gain[vfo[VFO_A].Band_id ]>POW_GAIN_MAX)
					ads.pow_gain[vfo[VFO_A].Band_id ] = 2;
				
				//AT24CXX_WriteLenByte(vfo[VFO_A].Band_id*2 + ADDRPOWER_GAIN , ads.pow_gain[vfo[VFO_A].Band_id],2);
			}
			//pow_gain_set();//�趨�����¸������ε�����ϵ��
			//else
			if(ks.Enc2_Use ==1)
			{
				sd.Sql += sd.Enc2_Value;/* ������� */
				if(sd.Sql > 99)sd.Sql =99;
				if(sd.Sql < 1)sd.Sql =0;
				AT24CXX_WriteOneByte(MENU_SQL, sd.Sql);
			}
			//else
			if(ks.Enc2_Use ==5)
			{
				ads.IF_gain += sd.Enc2_Value;/* �ֶ�AGC���� */
				if(ads.IF_gain >63)ads.IF_gain =32;
				if(ads.IF_gain < 1)ads.IF_gain =32;
				sd.Riss = ads.IF_gain;
				WM8978_MIC_Gain(ads.IF_gain&0x3f);//MIC��������
				//if(ks.rt_rx )AT24CXX_WriteOneByte(ADDR_IF_RXGAIN+sd.band, ads.IF_gain );
				AT24CXX_WriteOneByte(ADDR_IF_GAIN+ads.rx_amp_band, ads.IF_gain);
			}
			if(ks.Enc2_Use ==6)
			{
//				Filte_Center[vfo[VFO_A].FilterSelected_id] +=sd.Enc2_Value*10;
//				
//				if(Filte_Center[vfo[VFO_A].FilterSelected_id] < filte_center[vfo[VFO_A].FilterSelected_id].min )
//					Filte_Center[vfo[VFO_A].FilterSelected_id] = filte_center[vfo[VFO_A].FilterSelected_id].Default	;
//				if(Filte_Center[vfo[VFO_A].FilterSelected_id] > filte_center[vfo[VFO_A].FilterSelected_id].max )
//					Filte_Center[vfo[VFO_A].FilterSelected_id] = filte_center[vfo[VFO_A].FilterSelected_id].Default ;
				//sd.Dsp_Bfo = Filte_Center[vfo[VFO_A].FilterSelected_id];
				Rx_amp_user[ads.rx_amp_band] +=sd.Enc2_Value ;/* �˵�ѡ��������� */
				if(Rx_amp_user[ads.rx_amp_band]<200)Rx_amp_user[ads.rx_amp_band]=1000;
				if(Rx_amp_user[ads.rx_amp_band]>2000)Rx_amp_user[ads.rx_amp_band]=1000;
				
				//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
				ads.rx_amp[ads.rx_amp_band] = Rx_amp_user[ads.rx_amp_band]*0.001f;
				//ads.rx_phase[ads.rx_amp_band] = (Rx_phase_user[ads.rx_amp_band]-100)*0.001f;
				/* �˵����ݴ洢 */
				AT24CXX_WriteLenByte( ads.rx_amp_band*3+ADDR_RX_BAND_PHASE_M  , Rx_amp_user[ads.rx_amp_band],2);
			}
//			if(ks.Enc2_Use ==7)
//			{
//				sd.tx_amp += sd.Enc2_Value;
//				if(sd.tx_amp <50 || sd.tx_amp>200)sd.tx_amp =100;
//				ads.tx_amp = sd.tx_amp *0.01f;
//				
//				AT24CXX_WriteLenByte(MENU_TX_AMP, sd.tx_amp,2);
//			}
			//else
			if(ks.Enc2_Use ==10)
			{
				//ks.Enc2_Use =10;
				sd.mic_gain += sd.Enc2_Value;/* ��˷�������� */
				if(sd.mic_gain >99)sd.mic_gain =99;
				if(sd.mic_gain < 1)sd.mic_gain =0;
				//sd.mic_gain = Menu_Values[13];
				AT24CXX_WriteLenByte(MENU_MIC_GAIN, sd.mic_gain,1);
			}
			//else 
			if(ks.Enc2_Use ==11&& ks.NR_key)
			{
				//ks.Enc2_Use =11;
				nr.dsp_nr_strength += sd.Enc2_Value;/* NRǿ�ȵ��� */
				if(nr.dsp_nr_strength >55)nr.dsp_nr_strength =55;
				if(nr.dsp_nr_strength < 1)nr.dsp_nr_strength =1;
				 //nr.dsp_nr_strength = Menu_Values[14];
				AT24CXX_WriteLenByte(MENU_NR_STRONG , nr.dsp_nr_strength,1);
			}
			//else 
			if(ks.Enc2_Use ==12&& ks.NR_key)
			{
				//ks.Enc2_Use =12;
				nr.dsp_nr_delaybuf_len += sd.Enc2_Value;/* NR�ӳ����鳤�ȵ��� */
				if(nr.dsp_nr_delaybuf_len >512)nr.dsp_nr_delaybuf_len =512;
				if(nr.dsp_nr_delaybuf_len < 32)nr.dsp_nr_delaybuf_len =32;
				//nr.dsp_nr_delaybuf_len = Menu_Values[15];
				
				AT24CXX_WriteLenByte(MENU_NR_BUFLEN  , nr.dsp_nr_delaybuf_len,2);
			}
			//else
			if(ks.Enc2_Use ==13)
			{
				//ks.Enc2_Use =13;
				sd.CW_vol += sd.Enc2_Value;/* ������������ */
				if(sd.CW_vol >32)sd.CW_vol =32;
				if(sd.CW_vol < 1)sd.CW_vol =0;
				//sd.CW_vol = Menu_Values[5];
				ads.spk_vol = sd.CW_vol ;//
				//WM8978_HPvol_Set(ads.spk_vol,ads.spk_vol);
				//WM8978_SPKvol_Set(ads.spk_vol);
				AT24CXX_WriteLenByte(MENU_CW_VOL , sd.CW_vol,1);
			}
			//else 
			if(ks.Enc2_Use ==14)
			{
				//ks.Enc2_Use =14;
				sd.CW_Sidetone += sd.Enc2_Value*10;/* CW�������� */
				if(sd.CW_Sidetone >990)sd.CW_Sidetone =990;
				if(sd.CW_Sidetone < 500)sd.CW_Sidetone =500;
				//sd.CW_Sidetone = Menu_Values[6];
				//TIM1->ARR = 1000000/sd.CW_Sidetone-1;
				sd.Dsp_Bfo = sd.CW_Sidetone;/* ����BFO��ƵƵ�� */
				AT24CXX_WriteLenByte(MENU_CW_SI , sd.CW_Sidetone,2);
			}
			//else 
			if(ks.Enc2_Use ==15)
			{
				//ks.Enc2_Use =15;
				sd.CW_Speed += sd.Enc2_Value;/* CW�Զ����ٶȵ��� */
				if(sd.CW_Speed >50)sd.CW_Speed =50;
				if(sd.CW_Speed < 5)sd.CW_Speed =5;
				//sd.CW_Speed = Menu_Values[7];
				
				ads.cw_spe = 1300 / sd.CW_Speed;//wpm_tab[sd.CW_Speed-5].cw_ms ;//1300 / sd.CW_Speed;
				AT24CXX_WriteLenByte(MENU_CW_SP , sd.CW_Speed,1);
			}
		}
		
	}
	
	//else
//	if(sd.Enc2_delay >0)
//	{
//		sd.Enc2_delay -=1;/* �Զ��˳�����ʱ��ʼ */
//		if(sd.Enc2_delay <2)
//		{
//			ks.Enc2_Use =0;
//			ks.EC1_12 =0;
//			ks.EC1_12_LONG =0;
//			sd.Enc2_delay =0;
//		}
//		
//	}			
}

/*
*************************************************************************************
*	NR DSP�������
*************************************************************************************
*/
 DSP_NR_parameter Dsp_nr[3]=
{
	{"STR",		10,		15,		 45},
	{"LEN",		48,		192,	512},
	{"NTS",		32,		96,		128},
};
/*
*	NR DSP���������
*/
void key_DSP_NoiseProcessing()
{
	u16 i;
	//static u8 Rt_key;
	static float mu_calc;
	static uint32_t	calc_taps;
	static uint8_t band;
	static uint16_t nr_strength0;
	static uint16_t nr_delaybuf0;
	//static uint16_t nr_numtaps0;
	//
	if(ks.key_lock== KEYLOCK_ON && ucKeyCode != KEY_NONE && ks.F_11 ==0)//�а����������������
	{
		if( ucKeyCode==KEY_1_LONG && vfo[VFO_A].Mode >= DEMOD_LSB )
		{
			ks.NR_key =~ks.NR_key &0x01;
			nr.reset_dsp_nr =1;
			//sd.Enc2_delay = 300;
		}
		if( ucKeyCode==KEY_2_LONG  )
		{
			ks.NOT_key =~ks.NOT_key &0x01;
			//sd.Enc2_delay = 200;
		}
	}
	if(TR_READ ==CONTROL_TX && ads.nr_key ==ks.NR_key)nr.reset_dsp_nr =1;
	if(band != vfo[VFO_A].Band_id && ads.nr_key ==ks.NR_key)
	{
		nr.reset_dsp_nr =1;
		band = vfo[VFO_A].Band_id;
	}

//	if(nr.reset_dsp_nr)
//	{
//		for(i = 0; i < DSP_NR_NUMTAPS_MAX + BUFF_LEN; i++)	 		
//		{
//			
//			lms1NormCoeff_f32[i] = 0;
//		}
//	}
    //
	if(nr_strength0 !=nr.dsp_nr_strength || nr_delaybuf0 != nr.dsp_nr_delaybuf_len || ads.nr_key!=ks.NR_key )
	{
		//nr.dsp_nr_strength = Menu_Values[14];
		//nr.dsp_nr_delaybuf_len = Menu_Values[15];
		nr.dsp_nr_numtaps = 32;//Menu_Values[16];
		if((nr.dsp_nr_numtaps < DSP_NR_NUMTAPS_MIN) || (nr.dsp_nr_numtaps > DSP_NR_NUMTAPS_MAX))
			calc_taps = DSP_NR_NUMTAPS_DEFAULT;
		else
			calc_taps = (uint16_t)nr.dsp_nr_numtaps;
		
		lms1Norm_instance.numTaps = calc_taps;
		lms1Norm_instance.pCoeffs = lms1NormCoeff_f32;
		lms1Norm_instance.pState = lms1StateF32;
		//
		mu_calc = (float)nr.dsp_nr_strength;		// get user setting
		
		//
		// New DSP NR "mu"
		//
		mu_calc /= 2;	// 
		mu_calc += 2;	// 
		mu_calc /= 10;	// 
		mu_calc = powf(10,mu_calc);		
		mu_calc = 1/mu_calc;			
		lms1Norm_instance.mu = mu_calc;

	
		for(i = 0; i < LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN; i++)	 		
		{
			lms1_nr_delay[i] = 0;
		}
		//
		for(i = 0; i < DSP_NR_NUMTAPS_MAX + BUFF_LEN; i++)	 		
		{
			lms1StateF32[i] = 0;			
			//if(nr.reset_dsp_nr)	 			
			//{
				lms1NormCoeff_f32[i] = 0;
			//}
		}
		//
		arm_lms_norm_init_f32(&lms1Norm_instance, calc_taps, &lms1NormCoeff_f32[0], &lms1StateF32[0], (float32_t)mu_calc, 32);
		//
		//
		if((nr.dsp_nr_delaybuf_len > DSP_NR_BUFLEN_MAX) || (nr.dsp_nr_delaybuf_len < DSP_NR_BUFLEN_MIN))
			nr.dsp_nr_delaybuf_len = DSP_NR_BUFLEN_DEFAULT;
		//
		nr_strength0 = nr.dsp_nr_strength;
		nr_delaybuf0 = nr.dsp_nr_delaybuf_len;
		band =vfo[VFO_A].Band_id;
		//nr_numtaps0 = Menu_Values[16];
		ads.nr_key =ks.NR_key;
	}
}





/************************************************************************************/
/*------END----------------------END---------------------------------END------------*/
/************************************************************************************/
