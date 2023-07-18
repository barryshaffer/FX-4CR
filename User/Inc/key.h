 
/*
*********************************************************************************************************
*
*	ģ������ : ��������ģ��
*	�ļ����� : bsp_key.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/
#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
/*
**************************************************************************************************************
*
*	
*	IXD_202 �������߷��䣺
*		K1 ��       : PA2     SSB
*		K2 ��       : PB1     M.V
*		K3 ��       : PA3     CW
*		K4 ��   	: PE11    AM.FM
*		K5 �� 		: PA6    RIT
*		K6 �� 		: PE12    A/B
*		K7 ��		: PA7    IF.ATT
*		K8 ��   	: PE13    PWR
*		K9 ��		: PC4	 MENU
*		K10��		: PE14	 FILTER
*		K11��		: PC5	 F
*		K12��		: PE15
*		K13��		: PB0
**************************************************************************************************************
*/
#define PIN_K1		GPIO_Pin_Get(GPIOA,PIN2)//PDin(0)
#define PIN_K2	    GPIO_Pin_Get(GPIOB,PIN1)//PDin(1)
#define PIN_K3	    GPIO_Pin_Get(GPIOA,PIN3)//PDin(2)
#define PIN_K4	    GPIO_Pin_Get(GPIOE,PIN11)//PDin(3)
#define PIN_K5	    GPIO_Pin_Get(GPIOA,PIN6)//PDin(4)
#define PIN_K6	    GPIO_Pin_Get(GPIOE,PIN12)//PAin(7)
#define PIN_K7	    GPIO_Pin_Get(GPIOA,PIN7)//PCin(4)
#define PIN_K8	    GPIO_Pin_Get(GPIOE,PIN13)//PDin(7)
#define PIN_K9	  	GPIO_Pin_Get(GPIOC,PIN4)//PBin(9)
#define PIN_K10		GPIO_Pin_Get(GPIOE,PIN14)//PEin(12)
#define PIN_K11	    GPIO_Pin_Get(GPIOC,PIN5)//PEin(13)
#define PIN_K12	  	GPIO_Pin_Get(GPIOE,PIN15)//PEin(4)
#define PIN_K13		GPIO_Pin_Get(GPIOB,PIN0)//PEin(3)
/* ��������,10��������*/
#define KEY_COUNT    13

/* ����Ӧ�ó���Ĺ��������������� */
#define DOWN_K1		KEY_1_DOWN
#define UP_K1		KEY_1_UP
#define LONG_K1		KEY_1_LONG

#define DOWN_K2		KEY_2_DOWN
#define UP_K2		KEY_2_UP
#define LONG_K2		KEY_2_LONG

#define DOWN_K3		KEY_3_DOWN
#define UP_K3		KEY_3_UP
#define LONG_K3		KEY_3_LONG

#define DOWN_K4		KEY_4_DOWN		
#define UP_K4		KEY_4_UP
#define LONG_K4		KEY_4_LONG

#define DOWN_K5		KEY_5_DOWN		
#define UP_K5		KEY_5_UP
#define LONG_K5		KEY_5_LONG

#define DOWN_K6		KEY_6_DOWN		
#define UP_K6		KEY_6_UP
#define LONG_K6		KEY_6_LONG

#define DOWN_K7		KEY_7_DOWN		
#define UP_K7		KEY_7_UP
#define LONG_K7		KEY_7_LONG

#define DOWN_K8		KEY_8_DOWN		
#define UP_K8		KEY_8_UP
#define LONG_K8		KEY_8_LONG

#define DOWN_K9		KEY_9_DOWN		
#define UP_K9	    KEY_9_UP
#define LONG_K9		KEY_9_LONG

#define DOWN_K10	KEY_10_DOWN		
#define UP_K10  	KEY_10_UP
#define LONG_K10	KEY_10_LONG

#define DOWN_K11	KEY_11_DOWN		
#define UP_K11	    KEY_11_UP
#define LONG_K11	KEY_11_LONG

#define DOWN_K12	KEY_12_DOWN		
#define UP_K12  	KEY_12_UP
#define LONG_K12	KEY_12_LONG

#define DOWN_K13	KEY_13_DOWN		
#define UP_K13  	KEY_13_UP
#define LONG_K13	KEY_13_LONG

/* ����ID, ��Ҫ����bsp_KeyState()��������ڲ��� */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
	KID_K3,
	KID_K4,
	KID_K5,
	KID_K6,
	KID_K7,
	KID_K8,
	KID_K9,
	KID_K10,
	KID_K11,
	KID_K12,
	KID_K13
}KEY_ID_E;

/*
	�����˲�ʱ��50ms, ��λ10ms��
	ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
	��ʹ������·����Ӳ���˲������˲�����Ҳ���Ա�֤�ɿ��ؼ�⵽�����¼�
*/
#define KEY_FILTER_TIME   5
#define KEY_LONG_TIME     100			/* ��λ10ms�� ����1�룬��Ϊ�����¼� */

/*
	ÿ��������Ӧ1��ȫ�ֵĽṹ�������
*/
typedef struct
{
	/* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
	u8 (*IsKeyDownFunc)(void); /* �������µ��жϺ���,1��ʾ���� */

	u8  Count;			/* �˲��������� */
	u16 LongCount;		/* ���������� */
	u16 LongTime;		/* �������³���ʱ��, 0��ʾ����ⳤ�� */
	u8  State;			/* ������ǰ״̬�����»��ǵ��� */
	u8  RepeatSpeed;	/* ������������ */
	u8  RepeatCount;	/* �������������� */
}KEY_T;

/*
	�����ֵ����, ���밴���´���ʱÿ�����İ��¡�����ͳ����¼�

	�Ƽ�ʹ��enum, ����#define��ԭ��
	(1) ����������ֵ,�������˳��ʹ���뿴���������
	(2) �������ɰ����Ǳ����ֵ�ظ���
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 ��ʾ�����¼� */

	KEY_1_DOWN,				/* 1������ */
	KEY_1_UP,				/* 1������ */
	KEY_1_LONG,				/* 1������ */

	KEY_2_DOWN,				/* 2������ */
	KEY_2_UP,				/* 2������ */
	KEY_2_LONG,				/* 2������ */

	KEY_3_DOWN,				/* 3������ */
	KEY_3_UP,				/* 3������ */
	KEY_3_LONG,				/* 3������ */

	KEY_4_DOWN,				/* 4������ */
	KEY_4_UP,				/* 4������ */
	KEY_4_LONG,				/* 4������ */

	KEY_5_DOWN,				/* 5������ */
	KEY_5_UP,				/* 5������ */
	KEY_5_LONG,				/* 5������ */

	KEY_6_DOWN,				/* 6������ */
	KEY_6_UP,				/* 6������ */
	KEY_6_LONG,				/* 6������ */

	KEY_7_DOWN,				/* 7������ */
	KEY_7_UP,				/* 7������ */
	KEY_7_LONG,				/* 7������ */

	KEY_8_DOWN,				/* 8������ */
	KEY_8_UP,				/* 8������ */
	KEY_8_LONG,				/* 8������ */

	/* ��ϼ� */
	KEY_9_DOWN,				/* 9������ */
	KEY_9_UP,				/* 9������ */
	KEY_9_LONG,				/* 9������ */

	KEY_10_DOWN,			/* 10������ */
	KEY_10_UP,				/* 10������ */
	KEY_10_LONG,			/* 10������ */
	
	KEY_11_DOWN,			/* 11������ */
	KEY_11_UP,				/* 11������ */
	KEY_11_LONG,			/* 11������ */

	KEY_12_DOWN,			/* 12������ */
	KEY_12_UP,				/* 12������ */
	KEY_12_LONG,			/* 12������ */
	
	KEY_13_DOWN,			/* 13������ */
	KEY_13_UP,				/* 13������ */
	KEY_13_LONG,			/* 13������ */
}KEY_ENUM;

/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	13
typedef struct
{
	u8 Buf[KEY_FIFO_SIZE];		/* ��ֵ������ */
	u8 Read;					/* ��������ָ��1 */
	u8 Write;					/* ������дָ�� */
	u8 Read2;					/* ��������ָ��2 */
}KEY_FIFO_T;


/* ���ⲿ���õĺ������� */
void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);
//void SysTick_init(u32 SYSCLK,u16 Xms);
#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
