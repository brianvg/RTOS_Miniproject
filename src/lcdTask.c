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
/** \brief        -
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/



void  lcdTask(void *pvData) {

	StringMsg *psStringMsg_test;
	vTaskDelay(200);

	while(1){

			if(xQueueReceive(queueString, &psStringMsg_test, portMAX_DELAY) == pdTRUE)
						{
							LCD_Clear(GUI_COLOR_BLACK);
							LCD_DisplayStringLine(1,psStringMsg_test->cString);
							eMemGiveBlock(&sMemPoolStringMsg , ( void *) psStringMsg_test) ;
						}

	}

}


