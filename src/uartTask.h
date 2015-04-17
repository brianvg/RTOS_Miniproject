#ifndef UARTTASK_H_
#define UARTTASK_H_
/******************************************************************************/
/** \file       uartTask.h
 *******************************************************************************
 *
 *  \brief
 *
 *
 *
 *  \author     sithl1
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

//----- Macros -----------------------------------------------------------------
#define LOG_MESSAGE_SIZE ( 64 )
#define NBROFMEMBLOCKS   ( 10 )

//----- Data types -------------------------------------------------------------
typedef struct __LogMsg {

    char         cString[LOG_MESSAGE_SIZE];
    int      	 iLetterNumber[LOG_MESSAGE_SIZE];
    uint8_t 	 index;
    uint8_t 	 textlength;
} StringMsg;

extern MemPoolManager sMemPoolStringMsg;
extern StringMsg memStringMsg [NBROFMEMBLOCKS];
extern QueueHandle_t queueString;
extern QueueHandle_t queueStringLCD;
//----- Function prototypes ----------------------------------------------------
extern void  UartTask(void *pvData);

//----- Data -------------------------------------------------------------------

#endif /* UARTTASK_H_ */
