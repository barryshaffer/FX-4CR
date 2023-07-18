
/*
*********************************************************************************************************
*
*	ģ������ : ������������ģ��
*	�ļ����� : key.c
*	��    �� : V1.0
*	˵    �� : ɨ�������������������˲����ƣ����а���FIFO�����Լ�������¼���
*				(1) ��������
*				(2) ��������
*				(3) ������
*				(4) ����ʱ�Զ�����
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*		V1.1    2013-06-29 armfly  ����1����ָ�룬����bsp_Idle() ������ȡϵͳ������ϼ���������
*								   ���� K1 K2 ��ϼ� �� K2 K3 ��ϼ�������ϵͳ����
*
*********************************************************************************************************
*/

#include "key.h"
#include "dwt.h"

/*	
	�����������Ӳ�������޸�GPIO����� IsKeyDown1 - IsKeyDown13 ����

	����û��İ�������С��13��������Խ�����İ���ȫ������Ϊ�͵�1������һ��������Ӱ�������
	#define KEY_COUNT    8	  ����� bsp_key.h �ļ��ж���
*/
/********************************************************************************************************/
//u16 adc_key=0;//ADC��������
//extern u8 TX_flong;

//#define PIN_K1	    PDin(0)
//#define PIN_K2	    PDin(1)
//#define PIN_K3	    PDin(2)
//#define PIN_K4	    PDin(3)
//#define PIN_K5	    PDin(4)
//#define PIN_K6	    PDin(5)
//#define PIN_K7	    PDin(6)
//#define PIN_K8	    PDin(7)
//#define PIN_K9	  	PBin(3)
//#define PIN_K10		PBin(4)
//#define PIN_K11	    PBin(5)
//#define PIN_K12	  	PEin(4)
//#define PIN_K13		PEin(3)
static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;		/* ����FIFO����,�ṹ�� */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

/*
*********************************************************************************************************
*	�� �� ��: IsKeyDownX
*	����˵��: �жϰ����Ƿ���
*	��    ��: ��
*	�� �� ֵ: ����ֵ1 ��ʾ���£�0��ʾδ����
*********************************************************************************************************
*/
	static uint8_t IsKeyDown1(void) {if (PIN_K1 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown2(void) {if (PIN_K2 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown3(void) {if (PIN_K3 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown4(void) {if (PIN_K4 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown5(void) {if (PIN_K5 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown6(void) {if (PIN_K6 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown7(void) {if (PIN_K7 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown8(void) {if (PIN_K8 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown9(void) {if (PIN_K9 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown10(void){if (PIN_K10== 0) return 1;else return 0;}
	static uint8_t IsKeyDown11(void){if (PIN_K11== 0) return 1;else return 0;}
	static uint8_t IsKeyDown12(void){if (PIN_K12== 0) return 1;else return 0;}
	static uint8_t IsKeyDown13(void){if (PIN_K13== 0) return 1;else return 0;}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKey
*	����˵��: ��ʼ������. �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();		/* ��ʼ���������� */
	bsp_InitKeyHard();		/* ��ʼ������Ӳ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_PutKey
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    ��:  _KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    ��:  ��
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey2
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ�������Ķ�ָ�롣
*	��    ��:  ��
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey2(void)
{
	uint8_t ret;

	if (s_tKey.Read2 == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read2];

		if (++s_tKey.Read2 >= KEY_FIFO_SIZE)
		{
			s_tKey.Read2 = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKeyState
*	����˵��: ��ȡ������״̬
*	��    ��:  _ucKeyID : ����ID����0��ʼ
*	�� �� ֵ: 1 ��ʾ���£� 0 ��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SetKeyParam
*	����˵��: ���ð�������
*	��    �Σ�_ucKeyID : ����ID����0��ʼ
*			_LongTime : �����¼�ʱ��
*			 _RepeatSpeed : �����ٶ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			/* �����������ٶȣ�0��ʾ��֧������ */
	s_tBtn[_ucKeyID].RepeatCount = 0;						/* ���������� */
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_ClearKey
*	����˵��: ��հ���FIFO������
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyHard
*	����˵��: ���ð�����Ӧ��GPIO
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitKeyHard(void)
{
	RCC->AHB4ENR|=1<<0;	    //ʹ��PORTAʱ�� 
	RCC->AHB4ENR|=1<<1;     //ʹ��PORTBʱ��
	RCC->AHB4ENR|=1<<2;  	//ʹ��PORTCʱ�� 
	//RCC->AHB4ENR|=1<<3;     //ʹ��PORTDʱ��
	RCC->AHB4ENR|=1<<4;     //ʹ��PORTEʱ��
	GPIO_Set(GPIOA,PIN2|PIN3|PIN6|PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //PB0,PB1����
	GPIO_Set(GPIOB,PIN0|PIN1,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //PB0,PB1����
	GPIO_Set(GPIOC,PIN4|PIN5,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //������������
	//GPIO_Set(GPIOD,PIN0|PIN1|PIN2|PIN3|PIN4|PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);	//������������
	GPIO_Set(GPIOE,PIN11|PIN12|PIN13|PIN14|PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //����Ϊ�������� 	
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyVar
*	����˵��: ��ʼ����������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* �԰���FIFO��дָ������ */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;
	//adc_key=ADC1->JDR1;
	/* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* ����������Ϊ�˲�ʱ���һ�� */
		s_tBtn[i].State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
		//s_tBtn[i].KeyCodeDown = 3 * i + 1;				/* �������µļ�ֵ���� */
		//s_tBtn[i].KeyCodeUp   = 3 * i + 2;				/* ��������ļ�ֵ���� */
		//s_tBtn[i].KeyCodeLong = 3 * i + 3;				/* �������������µļ�ֵ���� */
		s_tBtn[i].RepeatSpeed = 0;						/* �����������ٶȣ�0��ʾ��֧������ */
		s_tBtn[i].RepeatCount = 0;						/* ���������� */
	}
	
	/* �жϰ������µĺ��� */
	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
	s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
	s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
	s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
	s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
	s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
	s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
	s_tBtn[7].IsKeyDownFunc = IsKeyDown8;
	s_tBtn[8].IsKeyDownFunc = IsKeyDown9;
	s_tBtn[9].IsKeyDownFunc = IsKeyDown10;
	s_tBtn[10].IsKeyDownFunc = IsKeyDown11;
	s_tBtn[11].IsKeyDownFunc = IsKeyDown12;
	s_tBtn[12].IsKeyDownFunc = IsKeyDown13;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DetectKey
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    ��:  �����ṹ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;

	/*
		���û�г�ʼ�������������򱨴�
		if (s_tBtn[i].IsKeyDownFunc == 0)
		{
			printf("Fault : DetectButton(), s_tBtn[i].IsKeyDownFunc undefine");
		}
	*/

	pBtn = &s_tBtn[i];
	if (pBtn->IsKeyDownFunc())
	{
		if (pBtn->Count < KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;

				/* ���Ͱ�ť���µ���Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));
					}
				}
				else
				{
					if (pBtn->RepeatSpeed > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{
		if (pBtn->LongCount >=   KEY_LONG_TIME)
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				bsp_PutKey((uint8_t)(0));
			}
		}
		if(pBtn->LongCount < KEY_LONG_TIME)
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}
		pBtn->Count = 0;
		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyScan
*	����˵��: ɨ�����а���������������systick�ж������Եĵ���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_KeyScan(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_DetectKey(i);
	}
}
/*
*********************************************************************************************************
*	�� �� ��: SysTick_init(u16 SYSCLK)
*	����˵��: ���ն�ʱ����ʼ��
*	��    ��: SYSCLK��ʹ���ں�ʱ��Դ,ͬCPUͬƵ��
*			  Xms: SysTick�жϵļ��ʱ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void SysTick_init(u32 SYSCLK,u16 Xms)
//{
//	u32 reload;
// 	SysTick->CTRL|=1<<2;	//SYSTICKʹ���ں�ʱ��Դ,ͬCPUͬƵ��	 
//							//�����Ƿ�ʹ��OS,fac_us����Ҫʹ�� 
//	reload =Xms * SYSCLK/1000;	//Xms�ж�һ��
//							//reloadΪ24λ�Ĵ���,���ֵ:16777216,��400M��,Լ��0.042s����	
//	SysTick->CTRL|=1<<1;   					//����SYSTICK�ж�
//	SysTick->LOAD=reload; 					//ÿ1/delay_ostickspersec���ж�һ��	
//	SysTick->CTRL|=1<<0;   					//����SYSTICK    
//}
/*
*********************************************************************************************************
*	�� �� ��:  SysTick_Handler(void)
*	����˵��: �����ն�ʱ���ж�ɨ�谴��
*	��    ��: SYSCLK��ʹ���ں�ʱ��Դ,ͬCPUͬƵ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
//void SysTick_Handler(void)
//{
//	//static uint8_t s_count = 0;	
//	//if (++s_count >= 10)
//	//{
//	//	s_count = 0;

//		bsp_KeyScan();	/* ÿ��10ms����һ�δ˺������˺����� bsp.c */
//	//}
//}
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
