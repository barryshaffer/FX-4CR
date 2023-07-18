#include "encode.h"
//#include "sys.h"




__IO EncoderSelection	encSel[4];

/*Ƶ�ʱ�������ʼ��*/
void FreqEncode_Init(u8 dir)
{	
	/*encoce 0*/
	encSel[ENCFREQ].value_old 		= ENCODER_MAX>>1;
    encSel[ENCFREQ].value_new		= ENCODER_MAX>>1;
    encSel[ENCFREQ].de_detent		= 0;
    encSel[ENCFREQ].tim 			= TIM2;
	
	RCC->AHB4ENR|=1<<0;     //ʹ��PORT Aʱ�� 
	RCC->APB1LENR|=1<<0; 	//TIM2ʱ��ʹ��    
	GPIO_Set(GPIOB,PIN3,GPIO_MODE_AF,0,0,GPIO_PUPD_NONE); //PA0����Ϊ�������� //FMģʽ����
	GPIO_Set(GPIOA,PIN15,GPIO_MODE_AF,0,0,GPIO_PUPD_NONE); //PA1����Ϊ�������� //CWģʽ����
	GPIO_AF_Set(GPIOB,3,1);	//PA0,AF2 
	GPIO_AF_Set(GPIOA,15,1);	//PA1,AF2 
	TIM2->ARR=(0X1FFF);		//�趨�������Զ���װֵ 
	TIM2->PSC=0;			//Ԥ��Ƶ������Ƶ 

	/* Timer configuration in Encoder mode */ 
	
	TIM2->CR1 &=~(3<<8);// ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM2->CR1 &=~(3<<5);// ѡ�����ģʽ:���Ӷ���ģʽ
		
	TIM2->CCMR1 |= 1<<0; //CC1S='01' IC1FP1ӳ�䵽TI1
	TIM2->CCMR1 |= 1<<8; //CC2S='01' IC2FP2ӳ�䵽TI2
	TIM2->CCER  |= (1<<1);	 //CC1P='0'	 IC1FP1�����࣬IC1FP1=TI1
	TIM2->CCER  |= (dir<<5);	 //CC2P='0'	 IC2FP2�����࣬IC2FP2=TI2
	TIM2->CCMR1 |= 3<<4; //	IC1F='1000' ���벶��1�˲���
	TIM2->SMCR  |= 2<<0;	 //SMS='011' ���е�������������غ��½�����Ч
	TIM2->CNT    = (0X0FFF);
	TIM2->CR1   |= 0x01;    //CEN=1��ʹ�ܶ�ʱ�� 	
	//MY_NVIC_Init(10,0, TIM2_IRQn ,4);	//��ռ1�������ȼ�3����2
}
/*���Ƶ�ʱ�������ʼ��*/
void ExtEncode_Init(void)
{	
	/*encoce 1*/
	encSel[ENC1].value_old 		= ENCODER_MAX>>1;
    encSel[ENC1].value_new		= ENCODER_MAX>>1;
    encSel[ENC1].de_detent		= 0;
    encSel[ENC1].tim 			= TIM8;
	
	RCC->AHB4ENR|=1<<2;     //ʹ��PORT Cʱ�� 
	RCC->APB2ENR|=1<<1; 	//TIM8ʱ��ʹ��    
	GPIO_Set(GPIOC,PIN6,GPIO_MODE_AF,0,0,GPIO_PUPD_NONE); //PA0����Ϊ�������� 
	GPIO_Set(GPIOC,PIN7,GPIO_MODE_AF,0,0,GPIO_PUPD_NONE); //PA1����Ϊ�������� 
	GPIO_AF_Set(GPIOC,6,3);	//PC6,AF3 
	GPIO_AF_Set(GPIOC,7,3);	//PC7,AF3 
	TIM8->ARR=0X1FFF;		//�趨�������Զ���װֵ 
	TIM8->PSC=0;			//Ԥ��Ƶ������Ƶ 

	/* Timer configuration in Encoder mode */ 
	
	TIM8->CR1 &=~(3<<8);// ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM8->CR1 &=~(3<<5);// ѡ�����ģʽ:���Ӷ���ģʽ
		
	TIM8->CCMR1 |= 1<<0; //CC1S='01' IC1FP1ӳ�䵽TI1
	TIM8->CCMR1 |= 1<<8; //CC2S='01' IC2FP2ӳ�䵽TI2
	TIM8->CCER  |= (0<<1);	 //CC1P='0'	 IC1FP1�����࣬IC1FP1=TI1
	TIM8->CCER  |= (1<<5);	 //CC2P='0'	 IC2FP2�����࣬IC2FP2=TI2
	TIM8->CCMR1 |= 3<<4; //	IC1F='1000' ���벶��1�˲���
	TIM8->SMCR  |= 2<<0;	 //SMS='011' ���е�������������غ��½�����Ч
	TIM8->CNT = 0X0FFF;
	TIM8->CR1   |= 0x01;    //CEN=1��ʹ�ܶ�ʱ�� 	
	//MY_NVIC_Init(10,0, TIM2_IRQn ,4);	//��ռ1�������ȼ�3����2
}
/*������������ʼ��*/
void VolEncode_Init(u8 dir)
{	
	/*encoce 0*/
	encSel[ENC2].value_old 		= ENCODER_MAX>>1;
    encSel[ENC2].value_new		= ENCODER_MAX>>1;
    encSel[ENC2].de_detent		= 0;
    encSel[ENC2].tim 			= TIM4;
	
	RCC->AHB4ENR|=1<<3;     //ʹ��PORT Aʱ�� 
	RCC->APB1LENR|=1<<2; 	//TIM2ʱ��ʹ��    
	GPIO_Set(GPIOD,PIN12,GPIO_MODE_AF,0,0,GPIO_PUPD_NONE); //PA0����Ϊ�������� 
	GPIO_Set(GPIOD,PIN13,GPIO_MODE_AF,0,0,GPIO_PUPD_NONE); //PA1����Ϊ�������� 
	GPIO_AF_Set(GPIOD,12,2);	//PD12,AF2 
	GPIO_AF_Set(GPIOD,13,2);	//PD13,AF2 
	TIM4->ARR=(0X1FFF);			//�趨�������Զ���װֵ 
	TIM4->PSC=0;			//Ԥ��Ƶ������Ƶ 

	/* Timer configuration in Encoder mode */ 
	
	TIM4->CR1 &=~(3<<8);// ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM4->CR1 &=~(3<<5);// ѡ�����ģʽ:���Ӷ���ģʽ
		
	TIM4->CCMR1 |= 1<<0; //CC1S='01' IC1FP1ӳ�䵽TI1
	TIM4->CCMR1 |= 1<<8; //CC2S='01' IC2FP2ӳ�䵽TI2
	TIM4->CCER  |= (0<<1);	 //CC1P='0'	 IC1FP1�����࣬IC1FP1=TI1
	TIM4->CCER  |= (dir<<5);	 //CC2P='0'	 IC2FP2�����࣬IC2FP2=TI2
	TIM4->CCMR1 |= 2<<4; //	IC1F='1000' ���벶��1�˲���
	TIM4->SMCR  |= 1<<0;	 //SMS='011' ���е�������������غ��½�����Ч
	TIM4->CNT    = (0X0FFF);
	TIM4->CR1   |= 0x01;    //CEN=1��ʹ�ܶ�ʱ�� 	
	//MY_NVIC_Init(10,0, TIM2_IRQn ,4);	//��ռ1�������ȼ�3����2
}
void Encode_Init(u8 dir)
{
	FreqEncode_Init(dir);
	//ExtEncode_Init();
	VolEncode_Init(dir);
}
uint32_t TIM_GetCounter(TIM_TypeDef* TIMx)
{
  /* Check the parameters */
 // assert_param(IS_TIM_ALL_PERIPH(TIMx));

  /* Get the Counter Register value */
  return TIMx->CNT;
}
void TIM_SetCounter(TIM_TypeDef* TIMx,uint32_t num)
{
  /* Check the parameters */
 // assert_param(IS_TIM_ALL_PERIPH(TIMx));

  /* Get the Counter Register value */
   TIMx->CNT=num;
}
int EncoderRead(const u8 encId ,u8 mode)
{
	int put_diff = 0;
	u8 dat;
	//static int32_t Encode_num=0;
    if (encId < ENC_MAX)
    {
        encSel[encId].value_new = TIM_GetCounter(encSel[encId].tim);		//��ȡ��������ֵ
//		if (encSel[encId].value_new < ENCODER_MIN)							//�Ͷ���ֵ�޶�
//        {
//            TIM_SetCounter(encSel[encId].tim, ENCODER_MAX - ENCODER_MIN -1);
//			encSel[encId].value_old = (ENCODER_MAX  );
//			encSel[encId].value_new = (ENCODER_MAX -1);
//			//no_change = 1;
//        }
//        else if (encSel[encId].value_new >(ENCODER_MAX  - ENCODER_MIN))//�߶���ֵ�޶�
//        {
//            TIM_SetCounter(encSel[encId].tim,ENCODER_MIN+1);
//			encSel[encId].value_old = ENCODER_MIN;
//			encSel[encId].value_new = ENCODER_MIN +1;
//			//no_change = 1;
//        }
        if (encSel[encId].value_old != encSel[encId].value_new)
        {
			if(encSel[encId].value_old > encSel[encId].value_new)dat = encSel[encId].value_old - encSel[encId].value_new;
			else dat = encSel[encId].value_new - encSel[encId].value_old;
			
            put_diff = encSel[encId].value_new - encSel[encId].value_old;
			if (mode)
			{
				mode *=2;
				if(dat < (12-mode)) put_diff *=1;
				else  
				{
					if(dat >=(12-mode)) dat =12-mode;
					put_diff *= mode*5;
				}
			}
			TIM_SetCounter(encSel[encId].tim,0X0FFF);
			encSel[encId].value_new = 0X0FFF;
            encSel[encId].value_old = encSel[encId].value_new;
        }
		//encSel[encId].value_old = encSel[encId].value_new;
	}
	return put_diff;
}
