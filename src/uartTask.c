/******************************************************************************/
/** \file       uartTask.c
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
void changeStringToLetternumber(char *pcString, uint8_t index, int *piarray);
//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    UartTask
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
void  UartTask(void *pvData) {

	char c = 0;
	StringMsg psStringMsg;
	psStringMsg.index = 0;
	BaseType_t xTaskWokenByReceive = pdFALSE;
	uart_init();

	if(eMemCreateMemoryPool(&sMemPoolStringMsg ,(void *) memStringMsg ,sizeof (StringMsg) ,NBROFMEMBLOCKS,"StringMsgPool")==0)
	{
		USART_SendData(CARME_UART0, 'd');
	}

    queueUart = xQueueCreate(100, sizeof(char));
    queueString = xQueueCreate(10, sizeof(StringMsg *));
    vQueueAddToRegistry((xQueueHandle) queueUart, "ReceiveLetter");
    vQueueAddToRegistry((xQueueHandle) queueString, "StringMsg");

    while(1)
    {
    	if(xQueueReceiveFromISR(queueUart, &c, &xTaskWokenByReceive) == pdTRUE)
    	{
    		psStringMsg.cString[psStringMsg.index] = c;
    		psStringMsg.index++;
    		psStringMsg.cString[psStringMsg.index] = '\0';
    		if(psStringMsg.index >= (LOG_MESSAGE_SIZE-1))
    		{
    			psStringMsg.index = 0;
    		}
    		if(psStringMsg.index >= 3)
    		{
    			if((psStringMsg.cString[psStringMsg.index-2] == '\r') && (psStringMsg.cString[psStringMsg.index-1] == '\n'))
    			{
    				psStringMsg.textlength = psStringMsg.index-2;
    				completedString(psStringMsg.cString, psStringMsg.textlength);
    				psStringMsg.index = 0;
    			}
    		}
    	}
    }
}

void completedString(char *pcString, uint8_t index)
{
	StringMsg *psStringMsg;
	lcdStruct *pslcd;
	int i = 0;

	if(eMemTakeBlock(&sMemPoolStringMsg ,( void **) &psStringMsg) == 0)
	{
		sprintf(psStringMsg->cString, "%s", pcString);
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
/** \brief
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
/** \brief
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
