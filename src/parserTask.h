#ifndef PARSERTASK_H_
#define PARSERTASK_H_
/******************************************************************************/
/** \file       parserTask.h
 *******************************************************************************
 *
 *  \brief
 *
 *
 *
 *  \author     lugij1
 *
 ******************************************************************************/
/*
 *  function
 *
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <memPoolService.h>
#include <stdbool.h>

//----- Macros -----------------------------------------------------------------
#define LOG_MESSAGE_SIZE ( 64 )

//----- Data types -------------------------------------------------------------
extern QueueHandle_t queueLCD;

typedef struct lcd {

    char        cString[LOG_MESSAGE_SIZE];
    bool		flagString;
    uint8_t 	flashTime;
    uint16_t 	potiValue;
} lcdStruct;

extern lcdStruct memParserMsg [10];
extern MemPoolManager sMemPoolParser;

//----- Function prototypes ----------------------------------------------------
extern void  parserTask(void *pvData);

//----- Data -------------------------------------------------------------------

#endif /* PARSERTASK_H_ */
