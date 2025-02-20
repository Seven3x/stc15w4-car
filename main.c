
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
#include 	"stdio.h"
#include	"uart.h"
#include 	"PWM.h"
#include 	"motor.h"

#define        Timer0_Reload        (MAIN_Fosc / 5000)                //Timer 0 中断频率, 1000次/秒

unsigned char rx3buf[20];	//uart3接收缓冲区
unsigned char rx3count;
unsigned char rx3state;

unsigned int gpwm1, gpwm2;

void  delay_ms(u8 ms);
/**
 * 
 * @brief  PWMA-P37; PWMB-P21; PWMC-P23; PWMD-P22
 * 
 */
void PWM_config();
void Timer0_init(void);

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
	char msg[5];
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
	PWM_config();

	UART1_config(1);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	UART2_config(2);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
	UART3_config();
	UART4_config();
	EA = 1;				//允许全局中断
	
	PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1发送一个字符串
	PrintString2("STC15F2K60S2 UART2 Test Prgramme!\r\n");	//SUART2发送一个字符串

	PWMx_SetPwmWide(PWM2_ID, 16, 155);	//PWM_id: PWM通道, PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID.
	PWMx_SetPwmWide(PWM3_ID, 16, 155);
	PWMx_SetPwmWide(PWM4_ID, 16, 155);
	PWMx_SetPwmWide(PWM5_ID, 16, 155);
	Timer0_init();
	P20=0;


	while(1){
		if (rx3state == 2) {
			rx3state = 1;
			gpwm1 =  rx3buf[3] / 20 + 2 ; //0.5ms-2.5ms 0.2ms一档
			gpwm2 =  rx3buf[5] / 20 + 2;
			if(rx3buf[6] == 0) {
				stop();
			}
			if(rx3buf[6] == 1) {
				forward();
			}
			if(rx3buf[6] == 2) {
				backward();
			}
			rx3state = 0;
			rx3count = 0;
			sprintf(msg, "state:");
			msg[6] = gpwm1/10 + '0';
			msg[7] = gpwm1%10 + '0';
			msg[8] = ' ';
			msg[9] = gpwm2/10 + '0';
			msg[10] = gpwm2%10 + '0';
			msg[11] = ' ';
			msg[12] = rx3buf[6] + '0';
			msg[13] = '\n';
			msg[14] = '\r';
			PrintString1(msg);
		}
	}
}


void Timer0_init(void)
{
	TR0 = 0;        //停止计数
	P20=1;

	#if (Timer0_Reload < 64)        // 如果用户设置值不合适， 则不启动定时器
		#error "Timer0设置的中断过快!"

	#elif ((Timer0_Reload/12) < 65536UL)        // 如果用户设置值不合适， 则不启动定时器
		ET0 = 1;        //允许中断
//        PT0 = 1;        //高优先级中断
		TMOD &= ~0x03;
		TMOD |= 0;        //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
//        TMOD |=  0x04;        //对外计数或分频
		TMOD &= ~0x04;        //定时
//        INT_CLKO |=  0x01;        //输出时钟
		INT_CLKO &= ~0x01;        //不输出时钟

		#if (Timer0_Reload < 65536UL)
				AUXR |=  0x80;        //1T mode
				TH0 = (u8)((65536UL - Timer0_Reload) / 256);
				TL0 = (u8)((65536UL - Timer0_Reload) % 256);
		#else
				AUXR &= ~0x80;        //12T mode
				TH0 = (u8)((65536UL - Timer0_Reload/12) / 256);
				TL0 = (u8)((65536UL - Timer0_Reload/12) % 256);
		#endif

		TR0 = 1;        //开始运行

	#else
			#error "Timer0设置的中断过慢!"
	#endif
}

u32 t0count = 0;
void timer0_int (void) interrupt TIMER0_VECTOR
{
	t0count ++;
	P70= t0count < gpwm1;
	P71= t0count < gpwm2;
	P20 = t0count < gpwm1;
	if(t0count >= 100) { //20ms一周期
		t0count = 0;
	}
}


void PWM_config() {
	
	PWMx_InitDefine		PWMx_InitStructure;					//结构定义
	
	PWMx_InitStructure.PWMx_IO_Select       = PWM2_P37;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM2_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P3n_standard(1<<7);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM3_P21;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM3_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P2n_standard(1<<1);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM4_P22;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM4_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P2n_standard(1<<2);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM5_P23;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM5_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P2n_standard(1<<3);										//IO初始化, 上电时为高阻

	TH2 = (u8)((65536UL - MAIN_Fosc / 500000UL) / 256);	//波特率使用Timer2
	TL2 = (u8)((65536UL - MAIN_Fosc / 500000UL) % 256);
	AUXR = 0x14;		//Timer2 set As Timer, Timer2 set as 1T mode;
	PWM_SourceClk_Duty(PwmClk_T2, 256);	//时钟源: PwmClk_1T,PwmClk_2T, ... PwmClk_16T, PwmClk_T2,   PWM周期: 1~32767
	
	PWMCR |= ENPWM;		// 使能PWM波形发生器，PWM计数器开始计数
	PWMCR &= ~ECBI;		//禁止PWM计数器归零中断
	//	PWMCR |=  ECBI;		//允许PWM计数器归零中断

//	PWMFDCR = ENFD | FLTFLIO | FDIO;	//PWM失效中断控制,  ENFD | FLTFLIO | EFDI | FDCMP | FDIO
}



void  delay_ms(u8 ms)
{
	u16 i;
	do{
		i = MAIN_Fosc / 13000;
		while(--i)	;   //14T per loop
	}while(--ms);
}

/** 
 * @brief  中断函数必须放在main.c，否则无法触发
 * 
 */

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
			
			RX3_Word = S3BUF;
			if(RX3_Word == 'A' && rx3count == 0 && rx3state == 0) {
				rx3state = 1;
				rx3count++;
				S3BUF = RX3_Word;
				while(!(S3CON & 0x02));
			}
			if(RX3_Word == 'B' && rx3state == 1) {
				rx3state = 2;
				rx3count = 0;
				S3BUF = 'P';
				while(!(S3CON & 0x02));
			}
			if(rx3state == 1 && rx3count < 10) {
				rx3buf[rx3count] = RX3_Word;
				rx3count++;
				
				S3BUF = '0'+rx3count;
				while(!(S3CON & 0x02));
			}

			S3CON = S3CON & 0xfe;

        }

}

void UART4_Interrupt_Receive(void) interrupt UART4_VECTOR
{
        unsigned char k = 0;
		// P20 = 0;
        k = S4CON;
        k = k & 0x01;	//判断是否接收到数据
        if(k == 1)
        {
			S4CON = S4CON & 0xfe; 		//清除接收标志
			RX4_Word = S4BUF;
			S4BUF = RX4_Word;
			// P20 = 0;
			while(!(S4CON & 0x02));		//等待发送完成
			
        }

}