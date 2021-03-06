/******************************************************************************/
/** \file       parserTask.c
 *******************************************************************************
 *
 *  \brief
 *
 *  \author     lugij1
 *
 *  \date       24.03.2015
 *
 *  \remark     Last Modification
 *
 *
 *
 ******************************************************************************/
/*
 *  functions  global: parserTask
 *
 *
 *  functions  local: -
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>

#include <carme_io1.h>
#include <carme_io2.h>

#include <stdio.h>

#include "parserTask.h"
#include "uartTask.h"

#include <lcd.h>

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------
QueueHandle_t queueLCD;
MemPoolManager sMemPoolParser;

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    parserTask
 ******************************************************************************/
/** \brief    	 Read Poti and switches
 * 				 Set PWM
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/

#define PWM_HZ ( 650000 ) /* PWM counter speed */
#define PWM_PERIOD ( 100 ) /* PWM period length */

void  parserTask(void *pvData) {

uint16_t u16Prescaler;
uint16_t value;
uint8_t flashTime;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

uint8_t LEDmask = 0xFF;
uint8_t SwitchStatus;

lcdStruct *pslcd;

/* CARME-IO initialize */
CARME_IO1_Init();
CARME_IO2_Init();

/* Calculate clock prescaler */
u16Prescaler = (uint16_t) (((SystemCoreClock) / 2) / PWM_HZ) - 1;

/* Initialize pwm clock structure */
TIM_TimeBaseStruct.TIM_Prescaler = u16Prescaler;
TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseStruct.TIM_Period = PWM_PERIOD - 1;
TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
CARME_IO2_PWM_Settings(&TIM_TimeBaseStruct);

/* Set start output value of pwm to zero */
CARME_IO2_PWM_Set(CARME_IO2_PWM0, 0);
CARME_IO2_PWM_Set(CARME_IO2_PWM1, 0);
CARME_IO2_PWM_Set(CARME_IO2_PWM2, 0);
CARME_IO2_PWM_Set(CARME_IO2_PWM3, 0);

/* Set direction of pwm3 to normal */
CARME_IO2_PWM_Phase(CARME_IO2_PWM_OPPOSITE_DIRECTION);

/* Redefine PWM0 to PWM2 as normal gpio's. Just pwm3 is defined as pwm */
CARME_IO2_GPIO_Out_Settings(CARME_IO2_GPIO_OUT_PIN0, CARME_IO2_GPIO_OUT_MODE_GPIO);
CARME_IO2_GPIO_Out_Settings(CARME_IO2_GPIO_OUT_PIN1, CARME_IO2_GPIO_OUT_MODE_GPIO);
CARME_IO2_GPIO_Out_Settings(CARME_IO2_GPIO_OUT_PIN2, CARME_IO2_GPIO_OUT_MODE_GPIO);

/* Choose SPI Channel */
CARME_IO2_SPI_Select(CARME_IO2_nPSC1);

/* Message Queue parserTask */
queueLCD = xQueueCreate(10, sizeof(lcdStruct *));
vQueueAddToRegistry((xQueueHandle) queueLCD, "LCD");

/* Memory Pool parserTask */
lcdStruct memParserMsg[10];
eMemCreateMemoryPool(&sMemPoolParser, (void *) memParserMsg, sizeof (lcdStruct), 10,"ParserPool");

/* TIM2 clock enable */
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
/* Time base configuration */
TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // 1 MHz down to 1 KHz (1 ms)  //168MHz/4 = 42MHz
TIM_TimeBaseStructure.TIM_Prescaler = 4200-1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
TIM_TimeBaseStructure.TIM_ClockDivision = 0;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
/* TIM2 enable counter */
TIM_Cmd(TIM3, ENABLE);

	while(1){

		/* Read poti */
		CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0, &value);
		/* Scale poti */
		value = (int)((float)value/(1024.0f)*(100.0f));

		/* Read Switch Status */
		CARME_IO1_SWITCH_Get(&SwitchStatus);
		/* Set LED */
		CARME_IO1_LED_Set(SwitchStatus, LEDmask);

		/* Set length of stroboscope */
		flashTime = SwitchStatus;

		/* Limit value to max period */
		if(value > (PWM_PERIOD - 1)) {
		value = PWM_PERIOD - 1;
		}

		/* Set PWM */
		CARME_IO2_PWM_Set(CARME_IO2_PWM3, value);

		/* Send length of stroboscope via SPI */
		CARME_IO2_SPI_CS_Out(0);
		CARME_IO2_SPI_Send(flashTime);
		CARME_IO2_SPI_CS_Out(1);

		/* Send flashTime and value via Queuemessage */
			if(eMemTakeBlock(&sMemPoolParser, ( void **) &pslcd) == 0)
			{
				 pslcd->flashTime = flashTime;
				 taskENTER_CRITICAL();
				 pslcd->potiValue = speed;
				 taskEXIT_CRITICAL();
				 pslcd->flagString = false;
				 xQueueSend(queueLCD, &pslcd, portMAX_DELAY);
			}

			/* Taskdelay 200ms */
			vTaskDelay(200);
	}

}


