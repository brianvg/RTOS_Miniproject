/******************************************************************************/
/** \file       lcdTask.c
 *******************************************************************************
 *
 *  \brief
 *
 *  \author     vongb3
 *
 *  \date       03.04.2014
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

#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_

#include <stdint.h>

//----- Macros -----------------------------------------------------------------

/*
 * Set this define to use the timer interrupt to switch the LED, set to 0 to use the encoder!
 */

#define USETIMER 0

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

/**
 * This function sets up the GPIOs used by the real time model.
 */

void encoder_Interrupts_Setup(void);

/**
 * These are the functions that handle the work which needs to be done in the interrupt.
 */
void MyEXTI9_5_IRQHandler (void);
void EncoderChannelA_IRQ(void);
void EncoderChannelB_IRQ(void);
void EncoderChannelI_IRQ(void);

//----- Data -------------------------------------------------------------------

/**
 * External Global Variables.
 */
volatile uint16_t encoderCount;
volatile extern uint16_t encoderMatch;
volatile extern uint16_t speed;



#endif /* INTERRUPT_HANDLER_H_ */
