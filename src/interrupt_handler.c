/*
 * interrupt_handler.c
 *
 *  Created on: Apr 3, 2015
 *      Author: Brian von Gunten
 */


/*----- Header-Files -------------------------------------------------------*/
#include "interrupt_handler.h"

#include <carme_io1.h>
#include <stdio.h>					/* Standard input and output			*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/

#include <FreeRTOS.h>				/* FreeRTOS								*/
#include <task.h>					/* FreeRTOS tasks						*/
#include <queue.h>					/* FreeRTOS queues						*/
#include <semphr.h>					/* FreeRTOS semaphores					*/
#include <memPoolService.h>			/* Memory pool manager service			*/

#include <stm32f4xx_exti.h>
#include <carme_io1.h>
#include <carme_io2.h>
#include <stdint.h>

volatile static uint32_t endtimerwert = 0;
volatile static uint8_t ledon = 0;
volatile static uint8_t oneshotA = 0;

void encoder_Interrupts_Setup(void) {

	EXTI_InitTypeDef EXTI_InitStructure ;
	NVIC_InitTypeDef NVIC_InitStructure ;

	/*Set up the GPIO's used in the interrupt routines (as triggers or outputs.)*/
	CARME_IO2_Init();

	if(USETIMER==0){
		/*Enable Interrupts for Encoder Channel A:*/
		/* Connect EXTI6 to G6*/
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG,EXTI_PinSource6) ;
		/*Configure EXTI6 line */
		EXTI_InitStructure . EXTI_Line = EXTI_Line6;
		EXTI_InitStructure . EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure . EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure . EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/*Enable Interrupts for Encoder Channel B:*/
		/* Connect EXTI7 to G7*/
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG,EXTI_PinSource7);
		/*Configure EXTI7 line */
		EXTI_InitStructure . EXTI_Line = EXTI_Line7;
		EXTI_InitStructure . EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure . EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure . EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	}
	/*Enable Interrupts for Encoder Channel I:*/
	/* Connect EXTI8 to G8*/
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG,EXTI_PinSource8) ;
	/*Configure EXTI8 line */
	EXTI_InitStructure . EXTI_Line = EXTI_Line8 ;
	EXTI_InitStructure . EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure . EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure . EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/*Enable and set EXTI9_5 Interrupt*/
	NVIC_InitStructure . NVIC_IRQChannel = EXTI9_5_IRQn ;
	NVIC_InitStructure . NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure . NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure . NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init (&NVIC_InitStructure);

	if(USETIMER==1){
		/////////////////////////////////////////////////////////////////
		NVIC_InitTypeDef NVIC_InitStructure_;
		/* Enable the TIM2 gloabal Interrupt */
		NVIC_InitStructure_.NVIC_IRQChannel = TIM4_IRQn;
		NVIC_InitStructure_.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure_.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure_.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure_);

		/* TIM2 clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		/* Time base configuration */
		TIM_TimeBaseStructure.TIM_Period = 65535; // 1 MHz down to 1 KHz (1 ms)
		TIM_TimeBaseStructure.TIM_Prescaler = 1024; // 24 MHz Clock down to 1 MHz (adjust per your clock)
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		/*TIM_OCInitTypeDef TIM_OCInitStructure;
		TIM_OCStructInit (& TIM_OCInitStructure);
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
		TIM_OC1Init(TIM2 , &TIM_OCInitStructure);
		TIM_OC2Init(TIM2 , &TIM_OCInitStructure);*/
		//TIM_SetCompare1(TIM2, 100);
		//TIM_SetCompare2(TIM2, 200);
		/* TIM IT enable */
		TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
		//TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
		/* TIM2 enable counter */
		TIM_Cmd(TIM4, ENABLE);
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
		CARME_IO2_GPIO_OUT_Set(0x01);
		CARME_IO2_GPIO_OUT_Set(0x00);
		//ledon = 1;
	}
	/*if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
		CARME_IO2_GPIO_OUT_Set(0x00);
	}*/
}

/**
 *  /Brief: This function determines which GPIO has caused the interrupt.
 *  /Author: Brian von Gunten
 *  /Date : 03.04.2015
 */
void EXTI9_5_IRQHandler(void){

	if(USETIMER==1){
		if( EXTI_GetITStatus(EXTI_Line8) != RESET){
			/* Clear Interrupt and call Interrupt Service Routine */
			EXTI_ClearITPendingBit(EXTI_Line8);
			EncoderChannelI_IRQ();
		}
		else{
			//Who knows? Check the interrupt setup.
		}
	}
	else{
		if( EXTI_GetITStatus(EXTI_Line6) != RESET){
			/* Clear Interrupt and call Interrupt Service Routine */
			EXTI_ClearITPendingBit(EXTI_Line6);
			EncoderChannelA_IRQ();
		}
		else{
			if( EXTI_GetITStatus(EXTI_Line7) != RESET){
				/* Clear Interrupt and call Interrupt Service Routine */
				EXTI_ClearITPendingBit(EXTI_Line7);
				EncoderChannelB_IRQ();
			}
			else{
				if( EXTI_GetITStatus(EXTI_Line8) != RESET){
					/* Clear Interrupt and call Interrupt Service Routine */
					EXTI_ClearITPendingBit(EXTI_Line8);
					EncoderChannelI_IRQ();
				}
				else{
					//Who knows? Check the interrupt setup.
				}
			}
		}
	}

}

void EncoderChannelA_IRQ(){
	if (oneshotA == 0){
		encoderCount++;
		oneshotA = 1;
	}
	if(encoderCount == encoderMatch){
		CARME_IO2_GPIO_OUT_Set(0x01); // Stroboscope trigger on.
		CARME_IO2_GPIO_OUT_Set(0x00); // Stroboscope trigger off.
	}
}

void EncoderChannelB_IRQ(){
	oneshotA = 0;
}

void EncoderChannelI_IRQ(){
	if(USETIMER==1){
		endtimerwert = TIM_GetCounter(TIM4);
		TIM_SetCompare1(TIM4, (endtimerwert/36)*encoderMatch);
		//TIM_SetCompare2(TIM4, (endtimerwert/36)*1.5);
		TIM_SetCounter(TIM4,0);
	}
	else{
		encoderCount = 0;
	}
}


