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

#include "uartTask.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------
static QueueHandle_t queueUart;
//----- Function prototypes ----------------------------------------------------
void uart_init(void);
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
	uart_init();
    queueUart = xQueueCreate(100, sizeof(char));
    vQueueAddToRegistry((xQueueHandle) queueUart, "ReceiveLetter");

    while(1)
    {
    	if(xQueueReceive(queueUart, &c, portMAX_DELAY) == pdTRUE)
    	{
    		USART_SendData(CARME_UART0, c);
    	}
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
