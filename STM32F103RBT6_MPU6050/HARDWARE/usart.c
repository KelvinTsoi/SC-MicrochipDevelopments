#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "usart.h"

void USART2_Init(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
		
	/* Setup Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/* GPIO Configuration -------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART Configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	/* NVIC Configuration ------------------------------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART2, ENABLE);
	USART_ClearFlag(USART2, USART_FLAG_TC);   
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
}

int Encode(unsigned char direction, const float pitch, const float roll, const float yaw, unsigned char * buffer)
{
    char tmp_pitch[16] = {0x00};
    char tmp_roll[16] = {0x00};
    char tmp_yaw[16] = {0x00};

    if(buffer == 0)
    {
        return 1;
    }

    if(pitch >= 0.000000)
    {
        sprintf(tmp_pitch, "[+%010f]", pitch);
    }
    else
    {
        sprintf(tmp_pitch, "[%011f]", pitch);
    }

    if(roll >= 0.000000)
    {
        sprintf(tmp_roll, "[+%010f]", roll);
    }
    else
    {
        sprintf(tmp_roll, "[%011f]", roll);
    }

    if(yaw >= 0.000000)
    {
        sprintf(tmp_yaw, "[+%010f]", yaw);
    }
    else
    {
        sprintf(tmp_yaw, "[%011f]", yaw);
    }

    if(LEFT_HAND_SIDE == direction)
    {
        sprintf((char *)buffer, "L%s%s%sE",  tmp_pitch, tmp_roll, tmp_yaw);
    }
    else if(RIGHT_HAND_SIDE == direction)
    {
        sprintf((char *)buffer, "R%s%s%sE",  tmp_pitch, tmp_roll, tmp_yaw);
    }
    else
    {
        return 2;
    }

    return 0;
}


#if 1
/* No Need to Modify */
#define DBG_BUFFER_SIZE		512////256
#define DBG_BUFFER_MASK		(DBG_BUFFER_SIZE-1)
typedef struct
{
    uint8_t buffer[DBG_BUFFER_SIZE];
    volatile uint16_t Head;
    volatile uint16_t Tail;
volatile uint8_t Status;
} DBG_BUFF_STRUCT;
DBG_BUFF_STRUCT uartDbgTx;
int uartDbgTxRd(uint8_t* pu8Char)
{
    	if(uartDbgTx.Head!=uartDbgTx.Tail)
    	{
		*pu8Char = uartDbgTx.buffer[uartDbgTx.Head];
		uartDbgTx.Head=(uartDbgTx.Head+1)&DBG_BUFFER_MASK;
		return 1;
    	}
	uartDbgTx.Status=0;
	return 0;
}
int uartDbgTxWr(uint8_t u8Char)
{
	if( ((uartDbgTx.Tail+1)&DBG_BUFFER_MASK)!=uartDbgTx.Head )
	{
		uartDbgTx.buffer[uartDbgTx.Tail]=u8Char;
		uartDbgTx.Tail=(uartDbgTx.Tail+1)&DBG_BUFFER_MASK;
		if(uartDbgTx.Status==0)
		{
			if(uartDbgTxRd(&u8Char))
			{
				uartDbgTx.Status=1;
				USART_SendData(USART2,  u8Char);
			}
		}
		return 1;
	}
	return 0;		
}
#include "delay.h"
void DbgPutChar(uint8_t u8Char)
{
	if(!uartDbgTxWr(u8Char))
	{
	//	DelayMs(5);
	//		printf("\n\n #Dbg full#\n\n");
		uartDbgTxWr(u8Char);
	}
//	while(uartDbgTx.Status);
}
void USART2_IRQHandler(void)
{
uint8_t u8Char;
  if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
  {   
  USART_ClearITPendingBit(USART2, USART_IT_TC);
    /* Write one byte to the transmit data register */
    	if(uartDbgTxRd(&u8Char))
    	{
				USART_SendData(USART2,  u8Char);
    	}
  }
#if 0//SER_RECVEN
  if(USART_GetITStatus(PRINT_UART, USART_IT_RXNE) != RESET)
  {
  USART_ClearITPendingBit(PRINT_UART, USART_IT_RXNE);
  /* Read one byte from the receive data register */
	Ser_Recv_Isr((uint8_t)USART_ReceiveData(PRINT_UART));
  }
#endif
}

#ifndef MicroLIB
      
struct __FILE 
{ 
	int handle; 
    /* Whatever you require here. If the only file you are using is */    
    /* standard output using printf() for debugging, no file handling */    
    /* is required. */
}; 

FILE __stdout;       

void _sys_exit(int x) 
{ 
	x = x; 
} 
 
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
#if 0
	 /* e.g. write a character to the USART */
    USART_SendData(USART2, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {}
#else
if(ch == '\n'){DbgPutChar('\r');}
  DbgPutChar(ch);
#endif
    return ch;
}


int ferror(FILE *f) {  
    /* Your implementation of ferror */  
    return EOF;  
} 
#endif 

FILE __stdin;

int fgetc(FILE *fp)
{
	int ch = 0;
	
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
    }

    ch = (int)USART2->DR & 0xFF;
	
    putchar(ch);
	
	return ch;
}
#endif
