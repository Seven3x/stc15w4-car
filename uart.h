#ifndef __UART_H
#define __UART_H

#include "STC15Fxxxx.H"
#define MAIN_Fosc		22118400L	//定义主时钟
#define		Baudrate1	115200UL
#define		Baudrate2	115200UL

#define	    UART1_BUF_LENGTH	32
#define	    UART2_BUF_LENGTH	32

extern 	u8      TX1_Cnt;	//发送计数
extern 	u8      RX1_Cnt;	//接收计数
extern 	u8      TX2_Cnt;	//发送计数
extern 	u8      RX2_Cnt;	//接收计数
extern  bit	    B_TX1_Busy;	//发送忙标志
extern  bit	    B_TX2_Busy;	//发送忙标志

extern 	idata   RX1_Buffer[UART1_BUF_LENGTH];	//接收缓冲
extern 	idata   RX2_Buffer[UART2_BUF_LENGTH];	//接收缓冲
extern  SendBuf3[6], SendBuf4[6];
extern  u8 RX2_Word;

void	UART1_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void	UART2_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void 	PrintString1(u8 *puts);
void 	PrintString2(u8 *puts);
void 	UART3_config();
void 	UART4_config();

void  delay_ms(u8 ms);

#endif // !__UART_H