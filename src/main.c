#include <reg52.h>
#include <intrins.h>

#include " LCD12864.h "	//���涨����12864Һ���Ķ˿ڽӷ�  �Լ� 12864��������
#include "Finger_Modle.h"//ָ��ģ���ͷ�ļ�

//=====P3�ĸ�4λ���˰�����������keyscan�����=============
sfr KEY=0xB0; //����P3,�ο�reg52.h
#define G_upkey 0x80 //px.7 ���������ϼ�
#define G_downkey 0x40 //px.6 ���������¼�
#define G_entkey 0x20 //px.5 ������ȷ����
#define G_cankey 0x10//px.4 ������ȡ����
//===========����======================================

sbit relay =P1^4; //�̵�������
sbit buzzer=P1^5; //����������
sbit red=   P2^7;//¼��ģʽָʾ�� �ڰ��ӿ�����Ƭ����
sbit green= P2^0;//ʶ��ģʽָʾ�� �ڰ���Զ�뵥Ƭ����

void delayms(int ms);
void keyscan();//����ɨ��

unsigned char Trg;//����������һ��p3����ֻ�����һ��
unsigned char Cont;//������������keyscan�����ı�

unsigned char KeyFuncIndex=0;    //��ŵ�ǰ�Ĳ˵�����

void (*KeyFuncPtr)();            //���尴������ָ��
//�������� 
typedef struct 
{
   unsigned char KeyStateIndex;   //��ǰ��״̬������
   unsigned char KeyUpState;      //�������ϼ�ʱ��״̬������
	 unsigned char KeyDownState;    //�������¼�ʱ��״̬������
   unsigned char KeyEnterState;   //���»س���ʱ��״̬������
   unsigned char KeyCancle;       //����ȡ���˻��ϼ��˵�
	 void (*CurrentOperate)(void);      //��ǰ״̬Ӧ��ִ�еĹ��ܲ���
}  StateTab;

//===========================================================================
//================�����Ǹ��˵����ʵʩ����================================
//============================================================================

void Stat00(void){//��һ��ҳ����ʾ�Ƿ����ֳɹ����ɹ��Ļ��������������һ��ҳ�棬���ɹ��Ļ��Ͳ�����ʱ����ɨ���ж�
	unsigned char i;
	ET0=0;//�ȹص���ʱ���жϣ������ڴ���ʱ��Ӧ�˼��̣���������һ��״̬��������״̬��û������
	dprintf(0,0,"  ����ָ�ƿ���  ");
	for(i=0;i<6;i++)//��ʼ����6�Σ����û��һ�γɹ�����ʾģ��ͨ�Ų�������ֻҪ�ɹ���������ѭ��
	{
		if(FM_VefPSW())//��ģ������ͨ�����̵����𡣽���ʶ��ģʽ
		{
      dprintf(1,0,"    ���ֳɹ�    ");
			dprintf(3,0,"�����������ϵͳ");
			//ET0=1;//�ɹ��Ļ�����ʱ����ɨ���жϣ����Խ�����һ��״̬������Ļ���һֱ��ʾ�����ʧ�ܽ���
			break;//�˳�ȫ��ѭ����ִ������for����䣬��ͬ��continue
		}
	    else
		{
			dprintf(1,0,"    ����ʧ��    ");//���ʧ�ܼ���ִ����һ��for���᲻��ɹ�
			dprintf(3,0,"������·ģ��");
			//break;
		}
	}
	ET0=1;
}
void Stat10(void){ //ui�򿪿���
	ET0=0;//�ض�ʱ���жϱ�����Ӧ�������ǵÿ�����
	dprintf(0,0,"    ���ػ���   ");//������ʾ
	dprintf(1,0,"    ¼��ָ��    ");
	dprintf(2,0,"    ���ָ��    ");
	dprintf(3,0,"                ");
	Writecolor_hang_12864(0,1);//���׵�һ�У�0�����һ�У�1�����ɫ
	Writecolor_hang_12864(1,0);
	ET0=1;
}
void Stat11(void){ //ui¼��ָ��
	ET0=0;
	dprintf(0,0,"    ���ػ���    ");//������ʾ
	dprintf(1,0,"    ¼��ָ��    ");
	dprintf(2,0,"    ���ָ��    ");
	dprintf(3,0,"                ");
	Writecolor_hang_12864(0,0);//����һ�е������
	Writecolor_hang_12864(2,0);//ȷ���������ϰ�ʱ�������в���
	Writecolor_hang_12864(1,1);//���׵ڶ���
	ET0=1;
}
void Stat12(void){ //ui���ָ��
	ET0=0;
	dprintf(0,0,"    ���ػ���    ");//������ʾ
	dprintf(1,0,"    ¼��ָ��    ");
	dprintf(2,0,"    ���ָ��    ");
	dprintf(3,0,"                ");
	Writecolor_hang_12864(1,0);
	Writecolor_hang_12864(3,0);
	Writecolor_hang_12864(2,1);
	ET0=1;
}
/*
void Stat13(void){ 
	ET0=0;
	dprintf(0,0,"    �򿪿���    ");//������ʾ
	dprintf(1,0,"    ¼��ָ��    ");
	dprintf(2,0,"    ���ָ��    ");
	dprintf(3,0,"    �򿪵�Դ    ");
	Writecolor_hang_12864(2,0);
	Writecolor_hang_12864(3,1);
	ET0=1;
}*/

void Stat20(void){//�򿪿���--ȷ��
	unsigned char num=0;
	unsigned char strnum[4]={0};//����ת��Ϊ�ַ���
	ET0=0;
  LcmClearTXT();//����ı�
	LcmClearBMP();//���ͼ��
	if(relay==0){//����̵����ǿ�����
		dprintf(0,0,"�����Ѿ�����    ");//keil bug������
		dprintf(1,0,"����ָ�ƹر�    ");
		dprintf(2,0,"                ");
		dprintf(3,0,"��\xB4\xFD����........");
	}else{
		dprintf(0,0,"�����Ѿ��ر�    ");//keil bug������
		dprintf(1,0,"����ָ�ƿ���    ");
		dprintf(2,0,"                ");
		dprintf(3,0,"��\xB4\xFD����........");
	}
	num=FM_Search();
	strnum[0]=32;//acsii��ո�
	strnum[1]= num/100+48;     //+48��Ϊ��ת����ASCII��  ��
	strnum[2]= (num%100)/10+48;//+48��Ϊ��ת����ASCII��  ʮ
	strnum[3]= num%10+48;      //+48��Ϊ��ת����ASCII��  ��?
	if(num==0xff){//����ʧ��
		dprintf(0,0,"��ָ�Ʋ�����    ");//keil bug������
		dprintf(1,0,"����ʧ��        ");
		dprintf(2,0,"                ");
		dprintf(3,0,"�����������    ");
		buzzer=0;
		delayms(500);
		buzzer=1;
	}else{//��ָ�ƴ��ڲ����ɹ�
		dprintf(0,0,"�����ɹ�        ");
		dprintf(1,0,"                ");
	  dprintf(2,0,"�û���: ");
		dprintf(2,4,strnum);
		dprintf(2,6,"   ");
		dprintf(3,0,"�����������    ");
		relay=~relay;//�̵�����ת
		buzzer=0;
		delayms(500);
		buzzer=1;
	}
	ET0=1;
}

void Stat21(void){//¼��ָ��--ȷ�
 
	unsigned char strnum[4]={0};//����ת��Ϊ�ַ���
	unsigned char FM_model_num=0,search_num=0;//ģ�������ָ�ƴ洢����
  bit ok1=0,ok2=0;
	ET0=0;//�ذ���ɨ��
	LcmClearTXT();//����ı�
	LcmClearBMP();//���ͼ��
	FM_model_num=FM_ValidTempleteNum(0);//��ȡָ��ģ�������ģ��������λ����λ�͹�?

	
	if((FM_model_num==0xff)){//�����ģ����ָ��ش���ֵ
		dprintf(0,0,"��ȡģ��\xCA\xFD����  ");//keil bug������
		dprintf(1,0,"                ");
		dprintf(2,0,"                ");
		dprintf(3,0,"���˻�����      ");
		goto end;//�����˶�ȡָ��
	}else{//�����������ȡģ����
		if((FM_model_num==0)||(FM_model_num==1)){//���ǵ�һ������2������ָ�Ƶģ��������Ȩ��
			dprintf(0,0,"    ���Ȩ��    ");
			goto begin;
		}else{//���ǵ�����������������ˣ����¼���ǲ�ͬ�ģ������������Ҫ���Ȩ�޵�ͬ�⣬��ֻ����ͨȨ��
			dprintf(0,0,"    �û�Ȩ��    ");
			dprintf(1,0,"�����Ա����ָ��");
			dprintf(2,0,"��\xB4\xFD����........");
			search_num=FM_Search();
			if((search_num==1)||(search_num==2)){//����ǵ�	1��2��ָ�ƾ��ǹ���Ա������Ȩ�����������¼��ָ��
				strnum[0]=32;//acsii��ո�
				strnum[1]= search_num/100+48;     //+48��Ϊ��ת����ASCII��  ��
				strnum[2]= (search_num%100)/10+48;//+48��Ϊ��ת����ASCII��  ʮ
				strnum[3]= search_num%10+48;      //+48��Ϊ��ת����ASCII��  ��?
				dprintf(1,0,"��֤�ɹ�        ");
				dprintf(2,0,"����Ա: ");
				dprintf(2,4,strnum);
				dprintf(2,6,"   ");
				buzzer=0;
				delayms(500);//����
				buzzer=1;
				goto begin;
			}else if(search_num==0xff){//������������ָ��
				dprintf(0,0,"    �û�Ȩ��    ");
				dprintf(1,0,"��ָ�ƻ�û¼��  ");
				dprintf(2,0,"                ");
				dprintf(3,0,"��������˻�����");
				buzzer=0;
				delayms(500);//����
				buzzer=1;
				goto end;
			}else{//ָ����¼��ĵ�����1��2��ָ�ƣ�û�����Ȩ��
				strnum[0]=32;//acsii��ո�
				strnum[1]= search_num/100+48;     //+48��Ϊ��ת����ASCII��  ��
				strnum[2]= (search_num%100)/10+48;//+48��Ϊ��ת����ASCII��  ʮ
				strnum[3]= search_num%10+48;      //+48��Ϊ��ת����ASCII��  ��?
				dprintf(1,0,"���Ȩ�޲���    ");
				dprintf(2,0,"�û���: ");
				dprintf(2,4,strnum);
				dprintf(2,6,"   ");
				dprintf(3,0,"��������˻�����");
				buzzer=0;
				delayms(500);//����
				buzzer=1;
				goto end;
			}
		}
	}
	    //--------------------��ȡ����ָ�Ʊ��浽ģ��---------------------------------
			begin:
		//����תΪchar������ʾ
			strnum[0]=32;//acsii��ո�
			strnum[1]= FM_model_num/100+48;     //+48��Ϊ��ת����ASCII��  ��
			strnum[2]= (FM_model_num%100)/10+48;//+48��Ϊ��ת����ASCII��  ʮ
			strnum[3]= FM_model_num%10+48;      //+48��Ϊ��ת����ASCII��  ��?
			dprintf(1,0,"ָ��\xCA\xFD: ");//keil�����bugҪ�ú�������������fd�ĺ���,ָ����
			dprintf(1,4,strnum);
			dprintf(1,6,"    ");//һ���ڲ��ո��������ʾ����
			dprintf(2,0,"��һ��¼��      ");
			dprintf(3,0,"��\xB4\xFD��..........");//�ȴ���......
			if(FM_GetImage()==1){//��ȡָ��ͼ��
				dprintf(1,0,"��һ�λ�ȡ�ɹ�  ");
				dprintf(2,0,"                ");
				dprintf(3,0,"\xD5\xFD�ڱ��浽�Ĵ���");
				if(FM_CreatChar_buffer(1)==1){//���浽�Ĵ���1
					if(FM_Searchfinger1()==0xff){//��buffer1����ָ�����û���յ��Ǿ��Ǹ�ָ�ƻ�û¼��
						goto con_tinue;//���������ĳ�����
					}else{
						dprintf(1,0,"��ָ���Ѿ�����  ");
						dprintf(2,0,"                ");
						dprintf(3,0,"������˻�����  ");
						buzzer=0;
						delayms(500);//����
						buzzer=1;
						goto end;
					}
					con_tinue:
					dprintf(1,0,"����ɹ�        ");
					dprintf(2,0,"                ");
					dprintf(3,0,"                ");
					buzzer=0;
					delayms(500);//������100ms
					buzzer=1;
					delayms(1000);//����Ļ��ʾ2��
					dprintf(3,0,"��ʼ�ڶ���¼��  ");
					if(FM_GetImage()==1){//��ȡָ��ͼ��
						dprintf(1,0,"�ڶ��λ�ȡ�ɹ�  ");
						dprintf(2,0,"                ");
						dprintf(3,0,"\xD5\xFD�ڱ��浽�Ĵ���");
						if(FM_CreatChar_buffer(2)==1){//��ͼ�񱣴浽�Ĵ���2
							dprintf(1,0,"����ɹ�        ");
							dprintf(2,0,"                ");
							dprintf(3,0,"                ");
							buzzer=0;
							delayms(500);//������100ms
							buzzer=1;
							delayms(1000);//����Ļ��ʾ2��
							dprintf(3,0,"��ʼ����ģ��    ");
							if(FM_RegModel_Charbuffer()==1){//����¼�������ָ������������ģ��
								dprintf(1,0,"����ģ��ɹ�    ");
								dprintf(2,0,"                ");
								dprintf(3,0,"���ڱ���ģ��    ");
								FM_model_num=FM_model_num+1;//�õ�ǰ��Чָ������1
							  //����תΪchar������ʾ
								strnum[0]=32;//acsii��ո�
								strnum[1]= FM_model_num/100+48;     //+48��Ϊ��ת����ASCII��  ��
								strnum[2]= (FM_model_num%100)/10+48;//+48��Ϊ��ת����ASCII��  ʮ
								strnum[3]= FM_model_num%10+48;      //+48��Ϊ��ת����ASCII��  ��?
								if(FM_Save_model(FM_model_num)==1){//����ģ��ɹ�������¼�������ָ������������ģ�棬���浽FM_model_num+1��
									//dprintf(0,0,"    ���Ȩ��    ");
									dprintf(1,0,"  ����ָ�Ƴɹ�  ");
									dprintf(2,0,"ָ�ƺ�:");
									dprintf(2,4,strnum);
									dprintf(2,6,"    ");//һ���ڲ��ո��������ʾ����
									dprintf(3,0,"�����������    ");
									goto end;
								}else{//����ģ��ʧ��
									dprintf(1,0,"����ָ��ʧ��    ");
									dprintf(2,0,"                ");
									dprintf(3,0,"���˻�����      ");
									goto end;
								}
							}else{
								dprintf(1,0,"����ָ��ģ��ʧ��");
								dprintf(2,0,"                ");
								dprintf(3,0,"���˻�����      ");
								goto end;
							}
						}else{
							dprintf(1,0,"����Ĵ�����ʧ��");
							dprintf(2,0,"                ");
							dprintf(3,0,"���˻�����      ");
							goto end;
						}
					}else{
						dprintf(1,0,"��ȡָ�ƶ�ʧ��  ");
						dprintf(2,0,"                ");
						dprintf(3,0,"���˻�����      ");
						goto end;
					}
				}else
				{
					dprintf(1,0,"����Ĵ���һʧ��");
					dprintf(2,0,"                ");
					dprintf(3,0,"���˻�����      ");
					goto end;
				}
			}else{//��ȡָ��ʧ��
				dprintf(1,0,"��ȡָ��һʧ��  ");
				dprintf(2,0,"                ");
				dprintf(3,0,"���˻�����      ");
				goto end;
			}
			//----------------��ȡ����ָ�Ʋ�����ģ�棬����ģ�����-------------------------
	end:  
	ET0=1;//��ɨ��
}

void Stat22(void){//���ָ��--ȷ��
	unsigned char search_num=0;
	//unsigned int time_count=0;
	ET0=0;
  LcmClearTXT();//����ı�
	LcmClearBMP();//���ͼ��
	dprintf(0,0,"ע��ò�����ɾ\xB3\xFD");
	dprintf(1,0,"����ָ�ư�������");
	dprintf(2,0,"Աָ��!!        ");
	dprintf(3,0,"ȷ�������˳�ȡ��");
	while(1){//�ȴ��û�����ѡ����������˳,�
		keyscan();//����ɨ��
		switch(Trg){
			case  G_cankey:			  //����ȡ����,P3.4
			{
				goto end;//�˳�
			}
			case G_entkey://����ȷ������
			{

				goto begin;
			}
			default:break;
		}
		//time_count++;
	}
	goto end;//�����while��Ϊ��ʱ�˳����������֤�Ͳ�������
	begin://����û�����ȷ������
	dprintf(0,0,"ɾ\xB3\xFD����ָ��    ");
	dprintf(1,0,"�����Ա����ָ��");
	dprintf(2,0,"                ");
	dprintf(3,0,"��\xB4\xFD����........");
	search_num=FM_Search();
	buzzer=0;
	delayms(300);
	buzzer=1;
	if(search_num==1||search_num==2){//���ǹ���Ա
		if(FM_Empty()==1){//�ɹ�
			  dprintf(0,0,"���ָ�ƿ�ɹ�  ");
				dprintf(1,0,"                ");
				dprintf(2,0,"                ");
				dprintf(3,0,"��������̷���  ");
				goto end;
		}else{
				dprintf(0,0,"���ָ�ƿ�ʧ��  ");
				dprintf(1,0,"                ");
				dprintf(2,0,"                ");
				dprintf(3,0,"��������̷���  ");
				goto end;
		}
	}else if(search_num=0xff){//���ָ�������д�������
		dprintf(0,0,"���ָ�Ʋ�����  ");
		dprintf(1,0,"                ");
		dprintf(2,0,"                ");
		dprintf(3,0,"�����������    ");
		goto end;
	}else{//���������й���ԱȨ�޵�ָ��
		dprintf(0,0,"���ָ��Ȩ�޲���");
		dprintf(1,0,"������Ա���Բ���");
		dprintf(2,0,"                ");
		dprintf(3,0,"�����������    ");
		goto end;
	}
	
	end:
	ET0=1;
}

/*
void Stat23(void){//���ָ��--ȷ�
	unsigned char num;
	unsigned char strnum[4]={0};//����ת��Ϊ�ַ���
	ET0=0;
	LcmClearBMP();//���ͼ��
	num=FM_Search();
								strnum[0]=32;//acsii��ո�
								strnum[1]= num/100+48;     //+48��Ϊ��ת����ASCII��  ��
								strnum[2]= (num%100)/10+48;//+48��Ϊ��ת����ASCII��  ʮ
								strnum[3]= num%10+48;      //+48��Ϊ��ת����ASCII��  ��?
	if(num==0xff){//ʧ��
			  dprintf(0,0,"���ָ��ʧ��    ");
				dprintf(1,0,"                ");
				dprintf(2,0,"                ");
				dprintf(3,0,"���˻�          ");
	}else{//�ɹ�
				dprintf(0,0,"���ָ�Ƴɹ�    ");
				dprintf(1,0,"                ");
				dprintf(2,0,strnum);
				dprintf(2,4,"            ");
				dprintf(3,0,"���˻�����      ");
	}
	ET0=1;
}
*/
/*-------------------------------------------------------------*/
 //���ݽṹ����,���ִ���ÿ�����������ǵĽ����״̬
StateTab code KeyTab[12]=
{
	{0,1,1,1,1,   (*Stat00)}, //���ּ�⣬ok�Ļ�������ʾ�¸�״̬���еĻ��͹ؼ����ж�ɨ��
	{1,1,2,4,1,   (*Stat10)},    //���㣬�򿪿���  ��ǰ��״̬������,�������¼�ʱ��״̬������,�������ϼ�ʱ��״̬������,���»س���ʱ��״̬������,��ǰ״̬Ӧ��ִ�еĹ��ܲ���
	{2,1,3,5,2,   (*Stat11)},	   //���㣬¼��ָ��
	{3,2,3,6,3,   (*Stat12)},	   //���㣬���ָ��
	//{4,3,4,8,4,   (*Stat13)},	   //���㣬����
	
	{4,1,1,1,1,   (*Stat20)},//�ڶ��㣬�򿪿��ص��Ӳ˵�
	{5,2,2,2,2,   (*Stat21)},//�ڶ���,¼��ָ��
	{6,3,3,3,3,   (*Stat22)},//�ڶ��� ���ָ��
	//{8,2,2,2,2,   (*Stat23)},//�ڶ��� ���ָ��	
};

//==================================================================================
//========״̬���ṹ����======================================
//=================================================

//**************?????***************************
 void delayms(int ms) 
{      
 unsigned char j;
 while(ms--)
 {
  	for(j =0;j<120;j++);
 }
}
//=======����ɨ��=============
void keyscan(){//�������ĸ�����
	unsigned char ReadDate=KEY^0xff;;
	Trg=ReadDate&(ReadDate^Cont);
	Cont=ReadDate;
}
/*-------------------------------------------------------------*/
void MenuOperate() interrupt 1
{
   keyscan(); //����ɨ�裬�Ĳ���Trg��Cont��ֵ
    switch(Trg) //��ⰴ������
	{
	    case  G_upkey:		       //���ϵļ���TrgΪ1000 0000 ����P3.7������һ��
		{
		    KeyFuncIndex=KeyTab[KeyFuncIndex].KeyUpState;
				//������ִ�а����Ĳ���
			  KeyFuncPtr=KeyTab[KeyFuncIndex].CurrentOperate;
				(*KeyFuncPtr)();     //ִ�е�ǰ�İ�������
			buzzer=0;delayms(20);buzzer=1;//������������
			break; 
		}
		case  G_entkey:			  //�س���,P3.5
		{
			KeyFuncIndex=KeyTab[KeyFuncIndex].KeyEnterState;
				//������ִ�а����Ĳ���
			KeyFuncPtr=KeyTab[KeyFuncIndex].CurrentOperate;
			(*KeyFuncPtr)();     //ִ�е�ǰ�İ�������
			buzzer=0;delayms(20);buzzer=1;//������������
			break; 
		}
		case  G_downkey:			  //���µļ�,P3.6
		{
			KeyFuncIndex=KeyTab[KeyFuncIndex].KeyDownState;
				//������ִ�а����Ĳ���
			KeyFuncPtr=KeyTab[KeyFuncIndex].CurrentOperate;
			(*KeyFuncPtr)();     //ִ�е�ǰ�İ�������
			buzzer=0;delayms(20);buzzer=1;//������������
			break; 
		}
		case  G_cankey:			  //����ȡ����,P3.4
		{
			KeyFuncIndex=KeyTab[KeyFuncIndex].KeyCancle;
				//������ִ�а����Ĳ���
			KeyFuncPtr=KeyTab[KeyFuncIndex].CurrentOperate;
			(*KeyFuncPtr)();     //ִ�е�ǰ�İ�������
			buzzer=0;delayms(20);buzzer=1;//������������
			break; 
		}
		//�˴���Ӱ����������,��ʱɨ��û�м�⵽��������
		default:return;
	}
}	
void main(void){
	delayms(10);//�ȴ���Ƭ����λ
	PSB=0;      //Һ��Ϊ������ʾģʽ����PSB��������Ϊ0
	
	//����ʱ�ж�-------------------------
	TMOD=0x01;//���ö�ʱ��0λ����ģʽ1
	TH0=(65536-22936)/256;//װ��ֵ11.0592Mhz����ʱ50ms��Ϊ45872,25msΪ22936
	TL0=(65536-22936)%256;//
	EA=1;//�����ж�
	ET0=1;//����ʱ��0�ж�
	TR0=1;//������ʱ��0
	//================================
	
	LcmInit(); //12864��ʼ��
	LcmClearTXT();//����ı�
	LcmClearBMP();//���ͼ��
	FM_Init();//��ʼ��ָ��ģ����Ҫ�Ǵ���
	//����һ�ν�����ʾ
	KeyFuncPtr=KeyTab[KeyFuncIndex].CurrentOperate;
	(*KeyFuncPtr)();     //ִ�е�ǰ�İ�������������stat00();
	
	while(1){
		//relay=1;
		//50ms��ʱ�жϣ�ɨ���������,���ö�ʱ��Ӧ�����
	}
}


//-------------------�����ֺ���-----------------------------















