/******************************************************************************/
/** \file       outputTask.c
 *******************************************************************************
 *
 *  \brief
 *
 *  \author     rothd8
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
 *			   outputTask
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
/** \brief        Writes each letter with RT-Modell .
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  outputTask(void *pvData)
{
	StringMsg *psOutputMsg_test;
	int i = 0;
	//vTaskDelay(200);
	while(1)
	{
		if(xQueueReceive(queueString, &psOutputMsg_test, portMAX_DELAY) == pdTRUE)
		{
			for(i=0;i<psOutputMsg_test->textlength;i++)
			{
				//encoderMatch is used in interrupt_handler
				taskENTER_CRITICAL();
				encoderMatch = psOutputMsg_test->iLetterNumber[i];
				taskEXIT_CRITICAL();
				//Delay for the next letter
				vTaskDelay(1000);
			}
			eMemGiveBlock(&sMemPoolStringMsg , ( void *) psOutputMsg_test) ;
		}
	}
}
