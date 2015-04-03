/*
 * interrupt_handler.h
 *
 *  Created on: Apr 3, 2015
 *      Author: Brian
 */

#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_

#include <stdint.h>

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
volatile uint8_t encoderCount;
volatile extern uint8_t encoderMatch;

#endif /* INTERRUPT_HANDLER_H_ */
