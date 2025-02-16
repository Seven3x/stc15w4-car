#include "STC15Fxxxx.H"
#include "uart.h"
#define MAIN_Fosc		22118400L	//定义主时钟

u8	TX1_Cnt;	//发送计数
u8	RX1_Cnt;	//接收计数
u8	TX2_Cnt;	//发送计数
u8	RX2_Cnt;	//接收计数
bit	B_TX1_Busy;	//发送忙标志
bit	B_TX2_Busy;	//发送忙标志

u8 	idata RX1_Buffer[UART1_BUF_LENGTH];	//接收缓冲
u8 	idata RX2_Buffer[UART2_BUF_LENGTH];	//接收缓冲
u8 SendBuf3[6], SendBuf4[6];
u8 RX2_Word;


void UART3_config()
{

        S3CON = 0x50;//01010000 8位可变波特率，无奇偶校验，允许接收 ,选择定时器4作为波特率发生器         0x10=选择定时器2作为波特率发生器

        T3H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);;                //设定定时初值
        T3L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);;                //设定定时初值

        T4T3M |= 0x0A;        ////定时器3时钟为Fosc,即1T ，启动定时器3，

        IE2 |= 0x08;         //ES3 = 1;允许中断
        P_SW2=0x00;         //串口2、3、4的位置 RX3=P0.0 TX3=P0.1
        EA = 1;

}
void UART4_config()
{

        S4CON = 0x50;//01010000 8位可变波特率，无奇偶校验，允许接收 ,选择定时器4作为波特率发生器        0x10=选择定时器2作为波特率发生器

        T4H = 0xFE;                //设定定时初值
        T4L = 0xE0;                //设定定时初值


        T4T3M |= 0xA0;        ////定时器3时钟为Fosc,即1T ，启动定时器3，

        IE2 |= 0x10;         //ES4 = 1;
        P_SW2=0x00;         //串口2、3、4的位置 RX4=P0.2 TX4=P0.3
        EA = 1;

}



void send_UART3()
{
	unsigned char temp = 0, i=0;

        S3CON = S3CON & 0xfd;  //S2TI = 0;
        for (i=0;i<6;i++)
        {
                S3BUF = SendBuf3[i];
                do
                {
                        temp = S3CON;
                        temp = temp & 0x02;                //temp=S2TI
                }while(temp == 0);
                S3CON = S3CON & 0xfd;            //S2TI = 0;
        }

}


void send_UART4()
{
	unsigned char temp = 0, i=0;

        S4CON = S4CON & 0xfd;  //S2TI = 0;
        for (i=0;i<6;i++)
        {
                S4BUF = SendBuf4[i];
                do
                {
                        temp = S4CON;
                        temp = temp & 0x02;                //temp=S2TI
                }while(temp == 0);
                S4CON = S4CON & 0xfd;            //S2TI = 0;
        }

}



//========================================================================
// 函数: void PrintString1(u8 *puts)
// 描述: 串口1发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString1(u8 *puts)
{
    for (; *puts != 0;	puts++)   	//遇到停止符0结束
	{
		SBUF = *puts;
		B_TX1_Busy = 1;
		while(B_TX1_Busy);
	}
}

//========================================================================
// 函数: void PrintString2(u8 *puts)
// 描述: 串口2发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)   	//遇到停止符0结束
	{
		S2BUF = *puts;
		B_TX2_Busy = 1;
		while(B_TX2_Busy);
	}
}

//========================================================================
// 函数: SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	SetTimer2Baudraye(u16 dat)
{
	AUXR &= ~(1<<4);	//Timer stop
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = dat / 256;
	TL2 = dat % 256;
	IE2  &= ~(1<<2);	//禁止中断
	AUXR |=  (1<<4);	//Timer run enable
}

//========================================================================
// 函数: void	UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	UART1_config(u8 brt)
{
	/*********** 波特率使用定时器2 *****************/
	if(brt == 2)
	{
		AUXR |= 0x01;		//S1 BRT Use Timer2;
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate1);
	}

	/*********** 波特率使用定时器1 *****************/
	else
	{
		TR1 = 0;
		AUXR &= ~0x01;		//S1 BRT Use Timer1;
		AUXR |=  (1<<6);	//Timer1 set as 1T mode
		TMOD &= ~(1<<6);	//Timer1 set As Timer
		TMOD &= ~0x30;		//Timer1_16bitAutoReload;
		TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
		TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
		ET1 = 0;	//禁止中断
		INT_CLKO &= ~0x02;	//不输出时钟
		TR1  = 1;
	}
	/*************************************************/

	SCON = (SCON & 0x3f) | 0x40;	//UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//	PS  = 1;	//高优先级中断
	ES  = 1;	//允许中断
	REN = 1;	//允许接收
	P_SW1 &= 0x3f;
	P_SW1 |= 0x80;		//UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7 (必须使用内部时钟)
//	PCON2 |=  (1<<4);	//内部短路RXD与TXD, 做中继, ENABLE,DISABLE

	B_TX1_Busy = 0;
	TX1_Cnt = 0;
	RX1_Cnt = 0;
}


//========================================================================
// 函数: void	UART2_config(u8 brt)
// 描述: UART2初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	UART2_config(u8 brt)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
	if(brt == 2)
	{
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate2);

		S2CON &= ~(1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
		IE2   |= 1;			//允许中断
		S2CON |= (1<<4);	//允许接收
		P_SW2 &= ~0x01;	
		P_SW2 |= 1;			//UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

		B_TX2_Busy = 0;
		TX2_Cnt = 0;
		RX2_Cnt = 0;
	}
}


