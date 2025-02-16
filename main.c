
/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/


#define MAIN_Fosc		22118400L	//定义主时钟

#include	"STC15Fxxxx.H"
#include	"uart.h"



void  delay_ms(u8 ms);

//========================================================================
// 函数: void main(void)
// 描述: 主函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void main(void)
{
	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口

	// P20 = 0;
	// delay_ms(1000);
	// P20 = 1;
	// delay_ms(1000);


	UART1_config(1);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	UART2_config(2);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
	EA = 1;				//允许全局中断

	
	PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1发送一个字符串
	PrintString2("STC15F2K60S2 UART2 Test Prgramme!\r\n");	//SUART2发送一个字符串

	while(1);
}


void  delay_ms(u8 ms)
{
	u16 i;
	do{
		i = MAIN_Fosc / 13000;
		while(--i)	;   //14T per loop
	}while(--ms);
}



//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		RX1_Buffer[RX1_Cnt] = SBUF;
		if(++RX1_Cnt >= UART1_BUF_LENGTH)	RX1_Cnt = 0;
	}

	if(TI)
	{
		TI = 0;
		B_TX1_Busy = 0;
	}
}

//========================================================================
// 函数: void UART2_int (void) interrupt UART2_VECTOR
// 描述: UART2中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART2_int (void) interrupt UART2_VECTOR
{
	if((S2CON & 1) != 0)
	{
		S2CON &= ~1;	//Clear Rx flag
		RX2_Word = S2BUF;
		S2BUF=RX2_Word;
		while(!(S2CON & 0x02));
		if(++RX2_Cnt >= UART2_BUF_LENGTH)	RX2_Cnt = 0;
	}

	if((S2CON & 2) != 0)
	{
		S2CON &= ~2;	//Clear Tx flag
		B_TX2_Busy = 0;
	}

}

void UART3_Interrupt_Receive(void) interrupt UART3_VECTOR
{
        unsigned char k = 0;

        k = S3CON;
        k = k & 0x01;   //判断是否接收到数据
        if(k == 1)
        {
			S3CON = S3CON & 0xfe;
        }

}

void UART4_Interrupt_Receive(void) interrupt UART4_VECTOR
{
        unsigned char k = 0;

        k = S4CON;
        k = k & 0x01;	//判断是否接收到数据
        if(k == 1)
        {
			S4CON = S4CON & 0xfe; 		//清除接收标志
        }

}