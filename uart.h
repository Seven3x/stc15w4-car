#ifndef __UART_H
#define __UART_H

#include "STC15Fxxxx.H"
#define MAIN_Fosc		22118400L	//������ʱ��
#define		Baudrate1	115200UL
#define		Baudrate2	115200UL

#define	    UART1_BUF_LENGTH	32
#define	    UART2_BUF_LENGTH	32

extern 	u8      TX1_Cnt;	//���ͼ���
extern 	u8      RX1_Cnt;	//���ռ���
extern 	u8      TX2_Cnt;	//���ͼ���
extern 	u8      RX2_Cnt;	//���ռ���
extern  bit	    B_TX1_Busy;	//����æ��־
extern  bit	    B_TX2_Busy;	//����æ��־

extern 	idata   RX1_Buffer[UART1_BUF_LENGTH];	//���ջ���
extern 	idata   RX2_Buffer[UART2_BUF_LENGTH];	//���ջ���
extern  SendBuf3[6], SendBuf4[6];
extern  u8 RX2_Word;

void	UART1_config(u8 brt);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
void	UART2_config(u8 brt);	// ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void 	PrintString1(u8 *puts);
void 	PrintString2(u8 *puts);
void 	UART3_config();
void 	UART4_config();

void  delay_ms(u8 ms);

#endif // !__UART_H