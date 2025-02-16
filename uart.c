#include "STC15Fxxxx.H"
#include "uart.h"
#define MAIN_Fosc		22118400L	//������ʱ��

u8	TX1_Cnt;	//���ͼ���
u8	RX1_Cnt;	//���ռ���
u8	TX2_Cnt;	//���ͼ���
u8	RX2_Cnt;	//���ռ���
bit	B_TX1_Busy;	//����æ��־
bit	B_TX2_Busy;	//����æ��־

u8 	idata RX1_Buffer[UART1_BUF_LENGTH];	//���ջ���
u8 	idata RX2_Buffer[UART2_BUF_LENGTH];	//���ջ���
u8 SendBuf3[6], SendBuf4[6];
u8 RX2_Word;


void UART3_config()
{

        S3CON = 0x50;//01010000 8λ�ɱ䲨���ʣ�����żУ�飬������� ,ѡ��ʱ��4��Ϊ�����ʷ�����         0x10=ѡ��ʱ��2��Ϊ�����ʷ�����

        T3H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);;                //�趨��ʱ��ֵ
        T3L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);;                //�趨��ʱ��ֵ

        T4T3M |= 0x0A;        ////��ʱ��3ʱ��ΪFosc,��1T ��������ʱ��3��

        IE2 |= 0x08;         //ES3 = 1;�����ж�
        P_SW2=0x00;         //����2��3��4��λ�� RX3=P0.0 TX3=P0.1
        EA = 1;

}
void UART4_config()
{

        S4CON = 0x50;//01010000 8λ�ɱ䲨���ʣ�����żУ�飬������� ,ѡ��ʱ��4��Ϊ�����ʷ�����        0x10=ѡ��ʱ��2��Ϊ�����ʷ�����

        T4H = 0xFE;                //�趨��ʱ��ֵ
        T4L = 0xE0;                //�趨��ʱ��ֵ


        T4T3M |= 0xA0;        ////��ʱ��3ʱ��ΪFosc,��1T ��������ʱ��3��

        IE2 |= 0x10;         //ES4 = 1;
        P_SW2=0x00;         //����2��3��4��λ�� RX4=P0.2 TX4=P0.3
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
// ����: void PrintString1(u8 *puts)
// ����: ����1�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void PrintString1(u8 *puts)
{
    for (; *puts != 0;	puts++)   	//����ֹͣ��0����
	{
		SBUF = *puts;
		B_TX1_Busy = 1;
		while(B_TX1_Busy);
	}
}

//========================================================================
// ����: void PrintString2(u8 *puts)
// ����: ����2�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)   	//����ֹͣ��0����
	{
		S2BUF = *puts;
		B_TX2_Busy = 1;
		while(B_TX2_Busy);
	}
}

//========================================================================
// ����: SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void	SetTimer2Baudraye(u16 dat)
{
	AUXR &= ~(1<<4);	//Timer stop
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = dat / 256;
	TL2 = dat % 256;
	IE2  &= ~(1<<2);	//��ֹ�ж�
	AUXR |=  (1<<4);	//Timer run enable
}

//========================================================================
// ����: void	UART1_config(u8 brt)
// ����: UART1��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void	UART1_config(u8 brt)
{
	/*********** ������ʹ�ö�ʱ��2 *****************/
	if(brt == 2)
	{
		AUXR |= 0x01;		//S1 BRT Use Timer2;
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate1);
	}

	/*********** ������ʹ�ö�ʱ��1 *****************/
	else
	{
		TR1 = 0;
		AUXR &= ~0x01;		//S1 BRT Use Timer1;
		AUXR |=  (1<<6);	//Timer1 set as 1T mode
		TMOD &= ~(1<<6);	//Timer1 set As Timer
		TMOD &= ~0x30;		//Timer1_16bitAutoReload;
		TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
		TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
		ET1 = 0;	//��ֹ�ж�
		INT_CLKO &= ~0x02;	//�����ʱ��
		TR1  = 1;
	}
	/*************************************************/

	SCON = (SCON & 0x3f) | 0x40;	//UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//	PS  = 1;	//�����ȼ��ж�
	ES  = 1;	//�����ж�
	REN = 1;	//�������
	P_SW1 &= 0x3f;
	P_SW1 |= 0x80;		//UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7 (����ʹ���ڲ�ʱ��)
//	PCON2 |=  (1<<4);	//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE

	B_TX1_Busy = 0;
	TX1_Cnt = 0;
	RX1_Cnt = 0;
}


//========================================================================
// ����: void	UART2_config(u8 brt)
// ����: UART2��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void	UART2_config(u8 brt)	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
	if(brt == 2)
	{
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate2);

		S2CON &= ~(1<<7);	// 8λ����, 1λ��ʼλ, 1λֹͣλ, ��У��
		IE2   |= 1;			//�����ж�
		S2CON |= (1<<4);	//�������
		P_SW2 &= ~0x01;	
		P_SW2 |= 1;			//UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

		B_TX2_Busy = 0;
		TX2_Cnt = 0;
		RX2_Cnt = 0;
	}
}


