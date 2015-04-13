/******************************************************************************/
/** \file       outputTask.c
 *******************************************************************************
 *
 *  \brief
 *
 *  \author     sithl1
 *
 *  \date       24.03.2015
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
#include <stdio.h>
#include <stdint.h>
#include "uartTask.h"
#include "outputTask.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------
/*******************************************************************************
 *  function :    OutputTask
 ******************************************************************************/
/** \brief        Writes arriving Log msg to the Uart.
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  OutputTask(void *pvData)
{
	StringMsg *psOutputMsg_test;
	int i = 0;
	vTaskDelay(200);
	while(1)
	{
		if(xQueueReceive(queueString, &psOutputMsg_test, portMAX_DELAY) == pdTRUE)
		{
			for(i=0;i<psOutputMsg_test->textlength;i++)
			{
				taskENTER_CRITICAL();
				encoderMatch = psOutputMsg_test->iLetterNumber[i];
				taskEXIT_CRITICAL();
				vTaskDelay(1000);
			}
			eMemGiveBlock(&sMemPoolStringMsg , ( void *) psOutputMsg_test) ;
		}
	}
}
