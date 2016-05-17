#ifndef __LCD12864_H__
#define __LCD12864_H__

#include<reg52.h>

//Һ���˿�
sbit  CS=P1^0;	 //RS   ��ģ��Ƭѡ�ˣ��ߵ�ƽ��Ч
sbit SID=P1^1;   //RW ��  �������������
sbit SCK=P1^2;   //E ���� ����ͬ��ʱ�ӣ�������ʱ��ȡSID����
sbit PSB=P1^6;	 //���ڷ�ʽ  (PSB�ӵ͵�ƽ��
sbit RST=P1^3;	 //��λ�ˣ��͵�ƽ��Ч


void SendByte(unsigned char Dbyte);
unsigned char ReceiveByte(void);
void CheckBusy(void);                   //��ѯҺ���Ƿ�æ
void WriteCommand(unsigned char Cbyte );//д����
void WriteData(unsigned char Dbyte );	//д����
void LcmInit( void );
void LcmClearTXT( void );
void LcmClearBMP( void );
void dprintf(unsigned char row,unsigned char col,unsigned char *puts);//д���֣�col������뺺����˵��

void fangbai_hang_12864(unsigned char hang);//��ĳһ�����һ�����Σ��뺺�ֽ�ϾͿ��Է����ˣ��Զ���������ȫ����ͼʵ������
void Writecolor_hang_12864(unsigned char hang,unsigned char color);//���������
#endif
