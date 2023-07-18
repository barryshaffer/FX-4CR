#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
/*
*******************************************************************************	 
* 	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
*	 FX-4C 
*	��ʱ�� ��������	   
*	BG2IXD
*	QQ:1413894726
*	��������:2020/9/9
*	�汾��V1.1
*	��Ȩ���У�����ؾ���									  
********************************************************************************
*/	 

/* TIM6ʱ�� */
#define TIM6_CLOCK		200000000

/* ͨ���ı�TIM1->CCR4��ֵ�ı�ռ�ձȣ��Ӷ��ı�CW�������� */
#define CW_SIDETONE_VOL	 TIM1->CCR4

//ͨ���ı�TIM3->CCR4��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define LED0_PWM_VAL TIM3->CCR4    

//TIM15 CH2��ΪPWM DAC�����ͨ�� 
#define PWM_DAC_VAL TIM15->CCR2 
/*
*	�ڴ˶������ɸ������ʱ��ȫ�ֱ���
*	ע�⣬��������__IO �� volatile����Ϊ����������жϺ���������ͬʱ�����ʣ��п�����ɱ����������Ż���
*/
#define TMR_COUNT	4		/* �����ʱ���ĸ��� ����ʱ��ID��Χ 0 - 3) */

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* һ�ι���ģʽ */
	TMR_AUTO_MODE = 1		/* �Զ���ʱ����ģʽ */
}TMR_MODE_E;

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint8_t Mode;		/* ������ģʽ��1���� */
	volatile uint8_t Flag;		/* ��ʱ�����־  */
	volatile uint32_t Count;	/* ������ */
	volatile uint32_t PreLoad;	/* ������Ԥװֵ */
}SOFT_TMR;

void SysTick_init(u32 SYSCLK,u16 Xms);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
void TIM6_Int_Init(u16 arr,u16 psc);/* TIM6����ADC/DAC */
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc); 
void TIM3_PWM_Init(u32 arr,u32 psc);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void TIM1_CH4_PWM_Init(u32 arr,u32 psc);
#endif























