/******************************************************************************/
/** \file       interrupt_handler.c
 *******************************************************************************
 *
 *  \brief		Interrupt Handler Functions
 *
 *  \author     vongb3
 *
 *  \date       3.04.2015
 *
 *  \remark     Last Modification
 *
 *
 *
 ******************************************************************************/
/*
 *  **functions  global:**
 *
 *	void encoder_Interrupts_Setup(void) : Sets up the hardware used in the interrupts,
 *	as well as initializing the interrupts.
 *
 *	void TIM4_IRQHandler(void) : Timer handler.
 *
 *	void EXTI9_5_IRQHandler(void) : GPIO in handler (for encoder)
 *
 *  **functions  local:**
 *
 *  void EncoderChannelI_IRQ() : Takes care of necessary work when a channel I interrupt arrives.
 *
 *  void EncoderChannelA_IRQ() :Takes care of necessary work when a channel A interrupt arrives.
 *
 *  void EncoderChannelB_IRQ() :Takes care of necessary work when a channel B interrupt arrives.
 *
 ******************************************************************************/

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
#include "parserTask.h"

//----- Macros -----------------------------------------------------------------
	/* Defined in interrupt_handler.h*/
//----- Data types -------------------------------------------------------------
	/* Defined in interrupt_handler.h*/
//----- Function prototypes ----------------------------------------------------
	/* Defined in interrupt_handler.h*/
//----- Data -------------------------------------------------------------------

/*
 * Local Static Variables.
 */

volatile static uint32_t endtimerwert = 0;
volatile static uint8_t ledon = 0;
volatile static uint8_t oneshotA = 0;
volatile static uint32_t timer3_test = 0;

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    encoder_Interrupts_Setup
 ******************************************************************************/
/** \brief    	 Sets up the encoder and timer interrupts used for determining disc position.
 *
 *  \type         global
 *
 *  \param[in]	  void
 *
 *  \return       void
 *
 *  \description	This function initializes the hardware used in the position tracking of the disc.
 *  				if the timer use flag is not set, then all three channels of the disc encoder are
 *  				used: A,B,I and the timer is not initialized. If the timer is used, only the encoder
 *  				I channel is necessary, and the timer is appropriately initialized to count upwards,
 *  				and generate an interrupt at a specific match value, which is dynamically changed at
 *  				runtime.
 *
 *
 ******************************************************************************/

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
		NVIC_InitTypeDef NVIC_InitStructure_;
		/* Enable the TIM2 global Interrupt */
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
		/* TIM IT enable */
		TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
		/* TIM2 enable counter */
		TIM_Cmd(TIM4, ENABLE);
	}
}

/*******************************************************************************
 *  function :    TIM4_IRQHandler
 ******************************************************************************/
/** \brief    	 Timer interrupt handler, triggers the flash.
 *
 *  \type         global
 *
 *  \param[in]	  void
 *
 *  \return       void
 *
 *  \description	This function, which runs at a timer match interrupt, briefly switches on, then off
 *  				the IO2 input which triggers the flash. This handler is only used if the USE_TIMER
 *  				flag is set to '1'.
 *
 *
 ******************************************************************************/

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
		CARME_IO2_GPIO_OUT_Set(0x01);
		CARME_IO2_GPIO_OUT_Set(0x00);
	}
}

/*******************************************************************************
 *  function :    EXTI9_5_IRQHandler
 ******************************************************************************/
/** \brief    	 External GPIO in interrupt handler.
 *
 *  \type         global
 *
 *  \param[in]	  void
 *
 *  \return       void
 *
 *  \description	This function, which is called at any GPIO in (5 to 9) simply
 *  				determines the source of the interrupt, resets it, then calls a
 *  				sub-handler function appropriately.
 *
 *
 ******************************************************************************/

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

/*******************************************************************************
 *  function :    EncoderChannelA_IRQ
 ******************************************************************************/
/** \brief    	 Function called if the source of the GPIO interrupt is from chan. A.
 *
 *  \type         local
 *
 *  \param[in]	  void
 *
 *  \return       void
 *
 *  \description	This function increments the encoder counter, to track the position
 *  				of the disc. If the match value which is presently set is reached by
 *  				the counter, then the flash is triggered. This function is only used
 *  				in the case that the timer is not used. A "one-shot" variable is used
 *  				here since the encoder is a mechanical device, and multiple channel A
 *  				rising flanks can be possible. This is good practice when using encoders.
 *
 ******************************************************************************/
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
/*******************************************************************************
 *  function :    EncoderChannelB_IRQ
 ******************************************************************************/
/** \brief    	 Function called if the source of the GPIO interrupt is from chan. B.
 *
 *  \type         local
 *
 *  \param[in]	  void
 *
 *  \return       void
 *
 *  \description	This function simply resets the one-shot variable set in the channel A handler.
 *  				This ensures that channel A flanks are not counted more than once. This is good
 *  				practice when using an encoder, even if the additional resolution of the second
 *  				channel is not necessary.
 *
 ******************************************************************************/
void EncoderChannelB_IRQ(){
	oneshotA = 0;
}

/*******************************************************************************
 *  function :    EncoderChannelI_IRQ
 ******************************************************************************/
/** \brief    	 Function called if the source of the GPIO interrupt is from chan. A.
 *
 *  \type         local
 *
 *  \param[in]	  void
 *
 *  \return       void
 *
 *  \description	This function resets the encoder count variable if an encoder based
 *  				interrupt scheme is used, since it is the zero point of the encoder.
 *  				(USETIMER is set to '0'). If the timer is used, this is the point where
 *  				the period of rotation is read out of the timer, the match value for the
 *  				timer is calculated and set, and finally the timer is reset to zero. (USETIMER
 *  				is set to '1')
 *
 ******************************************************************************/

void EncoderChannelI_IRQ(){
	if(USETIMER==1){
		endtimerwert = TIM_GetCounter(TIM4);
		TIM_SetCompare1(TIM4, (endtimerwert/36)*encoderMatch);
		TIM_SetCounter(TIM4,0);
	}
	else{
		encoderCount = 0;
	}
	speed = ((600000*2)/TIM_GetCounter(TIM3));
	TIM_SetCounter(TIM3,0);
}


