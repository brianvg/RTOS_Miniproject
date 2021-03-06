#ifndef OUTPUTTASK_H_
#define OUTPUTTASK_H_
/******************************************************************************/
/** \file       outputTask.h
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

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------
extern volatile uint16_t encoderMatch;
//----- Function prototypes ----------------------------------------------------
void  outputTask(void *pvData);

//----- Data -------------------------------------------------------------------
#endif /* OUTPUTTASK_H_ */
