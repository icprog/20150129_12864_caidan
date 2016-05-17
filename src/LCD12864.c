#include " LCD12864.h "

unsigned char code AC_TABLE[32]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //��һ�к���λ��
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //�ڶ��к���λ��
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //�����к���λ��
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //�����к���λ��
};

void SendByte(unsigned char Dbyte)
{
     unsigned char i;
     for(i=0;i<8;i++)
     {
           SCK = 0;
           Dbyte=Dbyte<<1;      //����һλ
           SID = CY;            //�Ƴ���λ��SID
           SCK = 1;
           SCK = 0;
     }
}

unsigned char ReceiveByte(void)
{
      unsigned char i,temp1,temp2;
     temp1 = 0;
     temp2 = 0;
     for(i=0;i<8;i++)
     {
           temp1=temp1<<1;
           SCK = 0;
           SCK = 1;            
           SCK = 0;
           if(SID) temp1++;
     }
     for(i=0;i<8;i++)
     {
           temp2=temp2<<1;
           SCK = 0;
           SCK = 1;
           SCK = 0;
           if(SID) temp2++;
     }
     return ((0xf0&temp1)+(0x0f&temp2));
}

void CheckBusy(void)//��ѯҺ���Ƿ�æ
{
     do   SendByte(0xfc);            //11111,RW(1),RS(0),0
     while(0x80&ReceiveByte());      //BF(.7)=1 Busy
}


void WriteCommand(unsigned char Cbyte )//д����
{
     CS = 1;
     CheckBusy();
     SendByte(0xf8);            //11111,RW(0),RS(0),0
     SendByte(0xf0&Cbyte);      //����λ
     SendByte(0xf0&Cbyte<<4);//����λ(��ִ��<<)
     CS = 0;
}

void WriteData(unsigned char Dbyte )
{
     CS = 1;
     CheckBusy();
     SendByte(0xfa);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dbyte);      //����λ
     SendByte(0xf0&Dbyte<<4);//����λ(��ִ��<<)
     CS = 0;
}

void LcmInit( void )
{
     WriteCommand(0x30);      //8BitMCU,����ָ���
     //WriteCommand(0x03);      //AC��0,���ı�DDRAM����
     WriteCommand(0x0C);      //��ʾON,�α�OFF,�α�λ����OFF
     WriteCommand(0x01);      //����,AC��0
     WriteCommand(0x06);      //д��ʱ,�α����ƶ�
}

void LcmClearTXT( void )
{
      unsigned char i;
     WriteCommand(0x30);      //8BitMCU,����ָ���
     WriteCommand(0x80);      //AC����ʼλ
     for(i=0;i<64;i++)
     WriteData(0x20);
}

void LcmClearBMP( void )
{
       unsigned char i,j;
     WriteCommand(0x34);      //8Bit����ָ�,��ʹ��36HҲҪд����
     WriteCommand(0x36);      //��ͼON,����ָ�����36H���ܿ���ͼ
     for(i=0;i<32;i++)        
     {
           WriteCommand(0x80|i);      //��λ��
           WriteCommand(0x80);      //��λ��
           for(j=0;j<32;j++)            //256/8=32 byte
                WriteData(0);
     }
}

void dprintf(unsigned char row,unsigned char col,unsigned char *puts)
{
     WriteCommand(0x32);//0011-00(��ָͨ��ģʽ)1��ͼ����Ȼ����Ҫ����һ��һ�ص���������0
     WriteCommand(AC_TABLE[8*row+col]);      //��ʼλ��
     while(*puts != 0)      //�ж��ַ����Ƿ���ʾ���
     {
           if(col==8)            //�жϻ���
           {            //�����ж�,���Զ��ӵ�һ�е�������
                 col=0;
                 row++;
           }
           if(row==4) {row=0;}    //һ����ʾ��,�ص������Ͻ�
           WriteCommand(AC_TABLE[8*row+col]);
           WriteData(*puts);      //һ������Ҫд����
           puts++;
           WriteData(*puts);
           puts++;
           col++;
     }
}

//���������ȫ��дһ�ε�ʵ����Ӧ̫���ˡ�������ĺ�����ֻ���ض���д
void fangbai_hang_12864(unsigned char hang){
	unsigned char i,y,ygroup,x,color_0,color_1,color_2,color_3,color_tmp;
	WriteCommand(0x36);//0011-01������ָ�1����ͼ��0
	switch(hang){
		case 0:{color_0=0xff;color_1=0x00;color_2=0x00;color_3=0x00;break;}//����������ÿһ�и�ֵ��ɫ
		case 1:{color_0=0x00;color_1=0xff;color_2=0x00;color_3=0x00;break;}
		case 2:{color_0=0x00;color_1=0x00;color_2=0xff;color_3=0x00;break;}
		case 3:{color_0=0x00;color_1=0x00;color_2=0x00;color_3=0xff;break;}
	}
	for(ygroup=0;ygroup<64;ygroup++){//��ѭ��
		if(ygroup<16){x=0x80;y=ygroup+0x80;color_tmp=color_0;}//���ѭ������һ��
		if(16<=ygroup&&ygroup<32){x=0x80;y=ygroup+0x80;color_tmp=color_1;}
		if(32<=ygroup&&ygroup<48){x=0x88;y=ygroup-32+0x80;color_tmp=color_2;}//�°���Ļ
		if(48<=ygroup&&ygroup<64){x=0x88;y=ygroup-32+0x80;color_tmp=color_3;}
		//if(ygroup<32){x=0x80;y=ygroup+0x80;}
		//else{x=0x88;y=ygroup-32+0x80;}
		WriteCommand(y);//˳�������y
		WriteCommand(x);
		for(i=0;i<16;i++){//����д��һ�е����ݣ�x������Զ�����
			WriteData(color_tmp);
		}
	}
	WriteCommand(0x32);//0011-00(��ָͨ��ģʽ)1��ͼ����Ȼ����Ҫ����һ��һ�ص���������0
}

void Writecolor_hang_12864(unsigned char hang,unsigned char color){
	unsigned char i,x,y,color_tmp,ygroup;//һ��Ҫ�ŵ�һ��
	WriteCommand(0x36);//0011-01������ָ�1����ͼ��0
	if(color==1){color_tmp=0xff;}//����ɫ
		else{color_tmp=0x00;}
	switch(hang){
		case 0:{x=0x80;y=0x80;break;}//
		case 1:{x=0x80,y=0x90;break;}
		case 2:{x=0x88;y=0x80;break;}
		case 3:{x=0x88;y=0x90;break;}
	}
	for(ygroup=0;ygroup<16;ygroup++){//16��ѭ��
			WriteCommand(y+ygroup);//˳�������y
			WriteCommand(x);
			for(i=0;i<16;i++){//����д��һ�е����ݣ�x������Զ�����
				WriteData(color_tmp);
			}
	}
	WriteCommand(0x32);//0011-00(��ָͨ��ģʽ)1��ͼ����Ȼ����Ҫ����һ��һ�ص���������0
}