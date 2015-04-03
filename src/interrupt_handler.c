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

void encoder_Interrupts_Setup(void) {

	EXTI_InitTypeDef EXTI_InitStructure ;
	NVIC_InitTypeDef NVIC_InitStructure ;

	/*Set up the GPIO's used in the interrupt routines (as triggers or outputs.)*/
	CARME_IO2_Init();

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
	NVIC_InitStructure . NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure . NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure . NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init (&NVIC_InitStructure);
}

/**
 *  /Brief: This function determines which GPIO has caused the interrupt.
 *  /Author: Brian von Gunten
 *  /Date : 03.04.2015
 */
void MyEXTI9_5_IRQHandler(void){

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

void EncoderChannelA_IRQ(){
	encoderCount++;
	if(encoderCount == encoderMatch){
		CARME_IO2_GPIO_OUT_Set(0x01); // Stroboscope trigger on.
	}
}

void EncoderChannelB_IRQ(){
	if(encoderCount == encoderMatch){
		CARME_IO2_GPIO_OUT_Set(0x00); // Stroboscope trigger off.
	}
}

void EncoderChannelI_IRQ(){
	encoderCount = 0;
}


