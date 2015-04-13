/*
 * interrupt_handler.h
 *
 *  Created on: Apr 3, 2015
 *      Author: Brian
 */

#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_

#include <stdint.h>

/*
 * Set this define to use the timer interrupt to switch the LED, set to 0 to use the encoder!
 */
#define USETIMER 1

/**
 *  Function definitions
 */

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


/**
 * Data External and Local.
 */
volatile uint16_t encoderCount;
volatile extern uint16_t encoderMatch;

#endif /* INTERRUPT_HANDLER_H_ */
