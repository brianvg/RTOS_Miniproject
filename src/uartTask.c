/******************************************************************************/
/** \file       uartTask.c
 *******************************************************************************
 *
 *  \brief		Receives data from UART-Interface and make with received letter
 *  			a string
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
 *  		   uartTaskInit
 *  		   uartTask
 *
 *
 *  functions  local:
 *  		   uart_init
 *  		   completedString
 *
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include <uart.h>

#include <stdio.h>
#include <lcd.h>

#include "uartTask.h"
#include "parserTask.h"
#include "lookup.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------
static QueueHandle_t queueUart;
QueueHandle_t queueString;
MemPoolManager sMemPoolStringMsg;
StringMsg memStringMsg[NBROFMEMBLOCKS];

//----- Function prototypes ----------------------------------------------------
void uart_init(void);
void completedString(char *pcString, uint8_t index);

//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    uartTaskInit
 ******************************************************************************/
/** \brief        initialize the Queue for the uartTask
 *
 *  \type         global
 *
 *  \param[in]	  -
 *
 *  \return       void
 *
 ******************************************************************************/
void uartTaskInit(void)
{
	eMemCreateMemoryPool(&sMemPoolStringMsg ,(void *) memStringMsg ,sizeof (StringMsg) ,NBROFMEMBLOCKS,"StringMsgPool");
    queueUart = xQueueCreate(100, sizeof(char));
    queueString = xQueueCreate(10, sizeof(StringMsg *));
    vQueueAddToRegistry((xQueueHandle) queueUart, "ReceiveLetter");
    vQueueAddToRegistry((xQueueHandle) queueString, "StringMsg");
}
/*******************************************************************************
 *  function :    uartTask
 ******************************************************************************/
/** \brief        Receives letters from UART and put these to a string.
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  uartTask(void *pvData) {

	char c = 0;
	StringMsg psStringMsg;
	psStringMsg.index = 0;
	BaseType_t xTaskWokenByReceive = pdFALSE;
	uart_init();

    while(1)
    {
    	// Waiting for character
    	if(xQueueReceiveFromISR(queueUart, &c, &xTaskWokenByReceive) == pdTRUE)
    	{
    		psStringMsg.cString[psStringMsg.index] = c;
    		psStringMsg.index++;
    		psStringMsg.cString[psStringMsg.index] = '\0';
    		// detecting overflow
    		if(psStringMsg.index >= (LOG_MESSAGE_SIZE-1))
    		{
    			psStringMsg.index = 0;
    		}
    		// min one character in buffer
    		if(psStringMsg.index >= 3)
    		{
    			// detecting CR
    			if((psStringMsg.cString[psStringMsg.index-2] == '\r') && (psStringMsg.cString[psStringMsg.index-1] == '\n'))
    			{
    				// without CR
    				psStringMsg.textlength = psStringMsg.index-2;
    				completedString(psStringMsg.cString, psStringMsg.textlength);
    				psStringMsg.index = 0;
    			}
    		}
    	}
    }
}
/*******************************************************************************
 *  function :    completedString
 ******************************************************************************/
/** \brief        Convert the message string to number
 *
 *  \type         local
 *
 *  \param[in]	  char *pcString     message string
 *  			  uint8_t index      length of the message string
 *
 *  \return       void
 *
 ******************************************************************************/
void completedString(char *pcString, uint8_t index)
{
	StringMsg *psStringMsg;
	lcdStruct *pslcd;
	int i = 0;

	if(eMemTakeBlock(&sMemPoolStringMsg ,( void **) &psStringMsg) == 0)
	{
		sprintf(psStringMsg->cString, "%s", pcString);
		// Convert string to slice number
		for(i=0;i<index;i++)
		{
			psStringMsg->iLetterNumber[i]=lookup(pcString[i]);
		}
		psStringMsg->textlength = index;
		xQueueSend(queueString, &psStringMsg, portMAX_DELAY);
	}
	if(eMemTakeBlock(&sMemPoolParser ,( void **) &pslcd) == 0)
	{
			sprintf(pslcd->cString, "%s", pcString);

			pslcd->flagString = true;
			xQueueSend(queueLCD, &pslcd, portMAX_DELAY);
	}
}
/*******************************************************************************
 *  function :    uart_init
 ******************************************************************************/
/** \brief        Initialize the UART-interface
 *
 *  \type         local
 *
 *  \param[in]	  -
 *
 *  \return       void
 *
 ******************************************************************************/
void uart_init(void)
{
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	USART_StructInit(&USART_InitStruct);
	USART_InitStruct.USART_BaudRate = 115200;
	CARME_UART_Init(CARME_UART0, &USART_InitStruct);
	USART_Cmd(CARME_UART0, DISABLE);

	USART_ITConfig(CARME_UART0, USART_IT_RXNE, ENABLE);
	USART_Cmd(CARME_UART0, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/*******************************************************************************
 *  function :    USART1_IRQHandler
 ******************************************************************************/
/** \brief		  Uart interupt handler
 *
 *  \type         local
 *
 *  \param[in]	  -
 *
 *  \return       void
 *
 ******************************************************************************/
void USART1_IRQHandler(void)
{
	char c;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	while (USART_GetITStatus(CARME_UART0, USART_IT_RXNE) != RESET)
	{
		c = USART_ReceiveData(CARME_UART0);
		xQueueSendFromISR(queueUart, &c, &xHigherPriorityTaskWoken);
	}
}
