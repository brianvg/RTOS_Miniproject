/******************************************************************************/
/** \file       lcdTask.c
 *******************************************************************************
 *
 *  \brief
 *
 *  \author     lugij1
 *
 *  \date       10.04.2015
 *
 *  \remark     Last Modification
 *
 *
 *
 ******************************************************************************/
/*
 *  functions  global:
 *
 *
 *  functions  local:
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
#include "lcdTask.h"

#include <lcd.h>



//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    lcdTask
 ******************************************************************************/
/** \brief        Display the UART String, the potiValue and the flashTime on the LCD
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/



void  lcdTask(void *pvData) {

	uint8_t flashTime;
	uint16_t potiValue;
	char bufferflashTime[20];
	char bufferpotiValue[20];
	lcdStruct *pslcdReceive;

	vTaskDelay(200);

	while(1){


		/* Receive flashTime, potiValue and UART String */
		if( xQueueReceive(queueLCD, &pslcdReceive, portMAX_DELAY) == pdTRUE)
						{
							/* Clear flashTime and potiValue on LCD */
							LCD_ClearLine(5);
							LCD_ClearLine(6);

							/* Set flashTime and potiValue */
							flashTime = pslcdReceive->flashTime;
							potiValue = pslcdReceive->potiValue;

							sprintf(bufferflashTime, "flashTime : %d",flashTime);
							sprintf(bufferpotiValue, "PotiValue : %d",potiValue);

							/* Display UART String */
							if (pslcdReceive->flagString==true){
							LCD_ClearLine(2);
							LCD_DisplayStringLine(2, pslcdReceive->cString);
							}

							/* Display flashTime and potiValue */
							LCD_DisplayStringLine(5, bufferflashTime);
							LCD_DisplayStringLine(6, bufferpotiValue);

							eMemGiveBlock(&sMemPoolParser, ( void *) pslcdReceive) ;
						}
			}

}




