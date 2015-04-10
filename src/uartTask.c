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
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include <uart.h>

#include <stdio.h>
#include <lcd.h>

#include "uartTask.h"
#include "lookup.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------
static QueueHandle_t queueUart;
static QueueHandle_t queueString;
MemPoolManager sMemPoolStringMsg;

MemPoolManager sMemPoolLogMsg;

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
	StringMsg *psStringMsg_test;
	psStringMsg.index = 0;
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
    	if(xQueueReceive(queueUart, &c, portMAX_DELAY) == pdTRUE)
    	{
    		USART_SendData(CARME_UART0, c);
    		psStringMsg.cString[psStringMsg.index] = c;
    		if(psStringMsg.index < LOG_MESSAGE_SIZE)
    		{
    			psStringMsg.index++;
    		}
    		else
    		{
    			psStringMsg.index = 0;
    		}
    		if(psStringMsg.index > 3)
    		{
    			if((psStringMsg.cString[psStringMsg.index-2] == '\r') && (psStringMsg.cString[psStringMsg.index-1] == '\n'))
    			{
    				psStringMsg.textlength = psStringMsg.index-2;
    				completedString(psStringMsg.cString, psStringMsg.textlength);
    				if(xQueueReceive(queueString, &psStringMsg_test, portMAX_DELAY) == pdTRUE)
    				{
    					LCD_Clear(GUI_COLOR_BLACK);
    					LCD_DisplayStringLine(1,psStringMsg_test->cString);
    					eMemGiveBlock(&sMemPoolStringMsg , ( void *) psStringMsg_test) ;
    				}
    				psStringMsg.index = 0;
    			}
    		}
    	}
    }
}

void completedString(char *pcString, uint8_t index)
{
	StringMsg *psStringMsg;
	int i = 0;

	if(eMemTakeBlock(&sMemPoolStringMsg ,( void **) &psStringMsg) == 0)
	{
		sprintf(psStringMsg->cString, "%s", pcString);
		for(i=0;i<index;i++)
		{
			psStringMsg->iLetterNumber[i]=lookup(pcString[i]);
		}
		psStringMsg->index = index;
		xQueueSend(queueString, &psStringMsg, portMAX_DELAY);
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
	while (USART_GetITStatus(CARME_UART0, USART_IT_RXNE) != RESET)
	{
		c = USART_ReceiveData(CARME_UART0);
		xQueueSend(queueUart, &c, portMAX_DELAY);
	}
}
