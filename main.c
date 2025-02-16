
/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ���˺꾧�Ƽ������ϼ�����   */
/*---------------------------------------------------------------------*/


#define MAIN_Fosc		22118400L	//������ʱ��

#include	"STC15Fxxxx.H"
#include	"uart.h"



void  delay_ms(u8 ms);

//========================================================================
// ����: void main(void)
// ����: ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void main(void)
{
	P0M1 = 0;	P0M0 = 0;	//����Ϊ׼˫���
	P1M1 = 0;	P1M0 = 0;	//����Ϊ׼˫���
	P2M1 = 0;	P2M0 = 0;	//����Ϊ׼˫���
	P3M1 = 0;	P3M0 = 0;	//����Ϊ׼˫���
	P4M1 = 0;	P4M0 = 0;	//����Ϊ׼˫���
	P5M1 = 0;	P5M0 = 0;	//����Ϊ׼˫���
	P6M1 = 0;	P6M0 = 0;	//����Ϊ׼˫���
	P7M1 = 0;	P7M0 = 0;	//����Ϊ׼˫���

	// P20 = 0;
	// delay_ms(1000);
	// P20 = 1;
	// delay_ms(1000);


	UART1_config(1);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
	UART2_config(2);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
	EA = 1;				//����ȫ���ж�

	
	PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1����һ���ַ���
	PrintString2("STC15F2K60S2 UART2 Test Prgramme!\r\n");	//SUART2����һ���ַ���

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
// ����: void UART1_int (void) interrupt UART1_VECTOR
// ����: UART1�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
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
// ����: void UART2_int (void) interrupt UART2_VECTOR
// ����: UART2�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
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
        k = k & 0x01;   //�ж��Ƿ���յ�����
        if(k == 1)
        {
			S3CON = S3CON & 0xfe;
        }

}

void UART4_Interrupt_Receive(void) interrupt UART4_VECTOR
{
        unsigned char k = 0;

        k = S4CON;
        k = k & 0x01;	//�ж��Ƿ���յ�����
        if(k == 1)
        {
			S4CON = S4CON & 0xfe; 		//������ձ�־
        }

}