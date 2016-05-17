#include "Finger_Modle.h"
#include <reg52.h>
#include "LCD12864.h"

#define FALSE 0
#define TURE  1
#define MAX_NUMBER    30 //������������ֵ
#define _Nop()  _nop_()

unsigned char 		g_FifoNumber=0; //���ص�fifo���ֽ���������㿴���ֲ�ķ��ذ���12���ֽ���ô�����ֵ����11
unsigned char     g_FIFO[MAX_NUMBER+1]={0};//�������ݵĻ�����
unsigned int      g_SearchNumber=0;//����ƥ��õ���ƥ��ҳ��

//Ĭ��Ϊʶ��ģʽ 
//bit g_modeflag= 0 ,  g_clearallflag=0, g_changeflag=0;

//////////////////////////////////////����ָ���/////////////////////////////

//Verify  Password   ����֤�豸���ֿ���
unsigned char code VPWD[16]={16,0x01,0Xff,0xff,0xff,0xff, 0x01,0,7,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};	 //����12��
//�����豸���ֿ���
unsigned char code STWD[16]={16,0X01 ,0Xff,0xff,0xff,0xff, 0x01,0,7,0x12,0x00,0x00,0x00,0x00,0x00,0x1a};	 //����12��
//GetImage           ��̽����ָ���Ӵ������϶���ͼ��
unsigned char code PS_GetImage[12]={12, 0X01 ,0Xff,0xff,0xff,0xff, 0x01, 0,3,1,0x00,0x05};	//����12��
//Gen Templet1        ������ԭʼͼ������ָ������1������buffer1��
unsigned char code PS_GenChar1[14]={13,0X01 ,0Xff,0xff,0xff,0xff,0x01,0,4,2,1,0x00,0x08};	//����12��
//Gen Templet2        ������ԭʼͼ��buffer����ָ������2
unsigned char code PS_GenChar2[13]={13,0X01 ,0Xff,0xff,0xff,0xff,0x01,0,4,2,2,0x00,0x09}; //����12��		
//Search Finger      ����CharBufferA��CharBufferB�е������ļ����������򲿷�ָ�ƿ�,��������buffer1
unsigned char code PS_Search1[18]={17,  0X01 ,0Xff,0xff,0xff,0xff, 0x01,   0,8, 4,1,0,0,    0,0x65,  0x00,0x73};	//����16��
//Merge Templet      ;��CharBufferA��CharBufferB�е������ļ��ϲ�����ģ�壬�������ModelBuffer��
unsigned char code PS_RegModel[14]={12,  0X01 ,0Xff,0xff,0xff,0xff, 0x01,  0,3,5 , 0x00,0x09};//����12��	
//Store Templet      ����ModelBuffer�е��ļ����浽flashָ�ƿ���
unsigned char code PS_StoreChar[16]={15,  0X01 ,0Xff,0xff,0xff,0xff, 0x01,  0,6,6,2,     0x00,0x00,     0x00,0x0f}; //����12��,ʵ�ʷ�����ʱҪָ���洢��ָ�ƿ��
//Read Note
unsigned char code RDNT[14]={13,0X01 ,0Xff,0xff,0xff,0xff, 0x01, 0,4,0x19,  0, 0x00,0x1e};
//Clear Note
unsigned char code DENT[46]={45,0X01 ,0Xff,0xff,0xff,0xff, 0x01, 0,36,0x18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x00,0x3d};
//DEL one templet
unsigned char code DELE_one[16]={16, 0X01 ,0Xff,0xff,0xff,0xff, 0x01,   0,7,  0x0c,    0x00,0x00, 0,1, 0x00,0x15};
//DEL templet      ;���ָ�ƿ�
unsigned char code PS_Empty[12]={12,0X01 ,0Xff,0xff,0xff,0xff, 0x01, 0,3,  0x0d,0x00,0x11};
//����Чģ�����,��һ�����ǳ��ȣ��ڶ����ǿյģ���֪��������Ҳ�����ͣ���û�к�����ǲ���
unsigned char code PS_ValidTempleteNum[12]={12,0X01 ,0Xff,0xff,0xff,0xff, 0x01,0,3,0x1d,0x00,0x21};

void delay1ms(unsigned char count){
	unsigned char i=0;
	unsigned int  k=0; 
	for(k=0;k<count;k++) 
	{
		for(i=0;i<160;i++)
		{
		   ;
		} 
	} 
}

void FM_Init(){//��ʼ������Ҫ�Ǵ���
  //���ڳ�ʼ�� 
    SCON=0x50;   //UART��ʽ1:8λUART;   REN=1:������� 
    PCON=0x00;   //SMOD=0:�����ʲ��ӱ� 
    TMOD=0x21;   //T1��ʽ2,����UART�����ʣ���ʱ��1����������
    TH1=0xFD; //�����ʸ�λ
    TL1=0xFD;   //UART����������:9600
		TR1=1; //����ʱ��1

	  TI=0;//�巢���жϱ�־
	  RI=0;//������жϱ�־
}

void TxdByte(unsigned char dat){//���ڷ�����Ϣ,ͨ����ѯ��ʽ����һ���ַ�
    TI = 0;		 //��TI��0����������ж�����Ϊ0
    SBUF = dat;	 //��������
    while(!TI){};	 //�ȴ�������ϣ����ж�Ϊ1������жϣ��Ͳ�ѭ��
    TI = 0;		 //����
}

//command����ʱҲ����ҪҪ������ʵ�Ϸ��͵���������Ч��Ӧ�˵�����ʱ̫�̵�������Ϊû����������,��oxef
bit Command(unsigned char *p,unsigned char COM_TIMEOUT){//�������,��ģ�鷢��һ������,����������ѭ������,���ص�ֵ�ᱣ����g_FIFO��
	unsigned char clk_high=0,clk_low=0;//��doѭ���л�ı䣬��λΪ��λ�Ľ�λ��clk_high���ں궨��TIMEOUT���˳�do�����ǳ�ʱ
	unsigned char count=0,tmpdat=0,temp=0,i=0,package=0,flag=0,checksum=0; 		
	bit result=0, start=0,stop=0;
	TxdByte(0xef);//���ݰ���ͷʶ����
	TxdByte(0x01);//���ݰ���ͷʶ����
	i=*p;         //����ĵڡ�0����Ԫ�ء��������˱�����ĳ��ȣ���������ȸ�����i��������в���
	p++; 
	p++;//������һ��0x01,��֪�������ʲô�õ�ȥ���ֳ�������
	for (count=i-1; count!=1;count--)  //Sent command String
	{
		temp=*p++;//ȡ�ڸ���1����Ԫ�ص����ݣ�Ȼ���� 
		TxdByte(temp);//�����ݷ��ͳ�ȥ
	} 
	//dprintf(1,0," aa ");	
	result=TURE;//�������,���Ϊ�� (��Ϊ1)���ȼ���Ϊ��   	
	g_FifoNumber=0;
	for (count=MAX_NUMBER+1; count!=0; count--)//�������FIFO[]������������ݣ�д��0X00
	{
	  g_FIFO[count-1]=0x00; 
	}	  
	if (result)   //����Ϊ���ǵ�һ��һ����������if
	{
			//dprintf(1,0," aa ");
		result=FALSE;
		start =FALSE;
		stop  =FALSE;
		count=0;
		clk_low=0;
		clk_high=0;	//����CL0����
		
		do /////////////////////////////do������////////////////////////////////
		{	//������break����
			restart0:
			//dprintf(1,0," aa ");
			clk_low++;//ÿѭ��һ�ξͼ�1
			if(clk_low==0xff)//��λ��������
			{
				clk_high++;//��λ
				clk_low=0x00;//����
			}
			if (RI==1)//������յ����ݣ���������˴��ڽ�������жϣ���ִ������ģ������do��ѭ��,���յ�1���ֽ�
			{ 				
				tmpdat=SBUF;//�Ȱѽ��յ������ݷŵ�tmpdat��
				RI=0;//��λ������һ���ֽڵĽ���
				if ((tmpdat==0xef)&&(start==FALSE))//�������Ϊ��һ�������������ݣ�Ҳ���ǡ�ָ��Ӧ�𡱵ĵ�һ���ֽ�
				{ 
					count=0;
					g_FIFO[0]=tmpdat;//�����һ��Ӧ���ֽ�(0XEF)�����ڵڡ�0����Ԫ����    
					flag=1;	
					goto 
					restart0;//�������жϷ�ʽ����			
				}

				if(flag==1)//��һ���ֽ��Ѿ�����������flag==1����
				{  
					if(tmpdat!=0x01)  //�������ݴ��󣬽����´ӻ������������ݣ�������յ�����0xef01
					{  	
						flag=0;//����Ӧ��ʧ��
						result=FALSE;
						start =FALSE;
						stop=FALSE;
						count=0;
						goto 
						restart0;					
					}
					//����ɹ����յ�0xef01�����Կ�ʼ��������
					flag=2;//flag=2;��ʾӦ��ɹ������Կ�ʼ����������
					count++;//����count=1;
					g_FIFO[count]=tmpdat;//����ڶ���Ӧ���ֽڣ�0X01�������ڵڡ�1����Ԫ����    
					start=TURE;	//Ӧ��ɹ����Կ�ʼ��������
					goto 
					restart0;	
				}
										  
				if((flag==2)&&(start==TURE))//flag=2;��ʾӦ��ɹ������Կ�ʼ����������
				{	   	  					 
					count++; //����Ԫ���±꣫��
					g_FIFO[count]=tmpdat;//��������
					if(count>=6)
					{
						checksum=g_FIFO[count]+checksum; //����У���
					}

					if(count==8)
					{ 
						package = g_FIFO[7]*256 + g_FIFO[8];	//���������							
						stop= TURE;
					}

					if(stop)
					{						
						if(count == package+8)
						{
							checksum=checksum-g_FIFO[count-1] - g_FIFO[count];
							if(checksum != (g_FIFO[count]&0xff)) 
							result=FALSE; //У��ʧ��,�ý����־Ϊ0							
							else 
							result=TURE;
							flag=0;
							break;//�˳�ѭ��
						} 
					}
				}
			}
		}/////////////////////////////do-while������----------------����////////////////////////////////
		 //����ڳ�ʱ��Χ�ڣ����յ�����������������ڣ���һֱdoѭ��  
		while ((clk_high<COM_TIMEOUT)&&(count<=MAX_NUMBER)); //�ɶ�ʱ���Լ����������������ƣ���֤�����ڴ�һֱѭ��
		  
		g_FifoNumber=count;	//������յ������ݸ���
	}
	return (result);
}


bit FM_VefPSW(void){//��֤�豸���ֿ���,�ɹ�����1     
 	unsigned char  count=0;
	//dprintf(1,0," aa ");
	while (1)
   	{
     	if(Command(VPWD,0xef) && (g_FifoNumber==11) && (g_FIFO[9]==0x00))  //ע������command()����COM_TIMEOUT��ֵ��Ҫ����?
		{
		  return (1) ;
			
		}	
     	count++;
   	  	if (count>=2)//������ɹ�������֤һ�Σ�������β��ɹ�������ʧ��
	    {  
	        return(0);   
	 	}
		
	}
}
bit FM_Empty(void) //���ָ�ƿ�   
{				
     delay1ms(200);
	 if(Command(PS_Empty,0xef)&&(g_FifoNumber==11)&&(g_FIFO[9]==0x00)){
			return 1;
	 }else{ //���ָ�ƿ�  
			return 0;
		}
}

bit FM_CreatChar_buffer(unsigned char Bufid){//����ͼ�����������룬���浽ָ����charbufid�У�����Ϊ1��2
	  unsigned char i=0,j=0;
	  delay1ms(20);//����Ϊ����һ���������ͼ��Ԥ��һЩִ��ʱ�䣬���ֵҲʮ����Ҫ��Ҫ�Ҹ�
	  for(i=0;i<0x2f;i++){//һ��Ҫ��Σ���Ϊ������ʱ����ģ�黹������Ӧ�㣬ѭ��256��,���ѭ��ֵ�ǳ���Ҫ��̫���������ѭ���ĸо�̫С�в��ܱ�ִ֤�к�����
			for(j=0;j<0xff;j++){
		if(Bufid==1){//���¼���ͼ��Ҫ���浽buf1	   			
      if(Command(PS_GenChar1,0xef) && (g_FifoNumber==11) && (g_FIFO[9]==0x00)){
				return 1;
			}  
		}
		else if(Bufid==2){//���Ҫ���浽buf2
		  	if(Command(PS_GenChar2,0xef) && (g_FifoNumber==11) && (g_FIFO[9]==0x00))  {				
				return 1;
			}  			
		}
		else//������������bufid
		{
		  return 0;//ʧ��
		}
	}
	}
	return 0;
}

bit FM_Save_model(unsigned char ID)//����ָ��ģ��buffer����Ķ�����ָ����id������
{
	 unsigned char i=0;

	 //���ڿ�ʼ���д洢ָ��ģ��Ĳ���
     for (i=0;i<16;i++)	//����ָ����Ϣ
 	 {
		g_FIFO[i]=PS_StoreChar[i];
	 }  
     g_FIFO[12]=ID;           //��ָ��ģ���ŵ�PAGE_IDҲ����FLASH��λ��
     g_FIFO[14]=g_FIFO[14]+ID;	//У���
     if (Command(g_FIFO,0xef)==1)//���ɹ�����0	//�˴����д��ָ��ģ�����
	 {
	   return(1);
	 }

	 return (0) ; //���ɹ�����0
}

unsigned char FM_ValidTempleteNum(bit addr){//���ؿ����õ�ģ���������addr=0,�ͷ��ص�λ���ֽڣ����Ϊ1���ͷ��ظ�λ�ֽڣ�һ���λ�Ϳ����ˣ���λ��Ϊ0
	if(Command(PS_ValidTempleteNum,0xef) && (g_FifoNumber==13) && (g_FIFO[9]==0x00)){
		if(addr==0){return g_FIFO[11];}//���ص�λֵ
		else{return g_FIFO[10];}//���ظ�λֵ
	}
	else{
		return 0xff;//����
	}
}

bit FM_RegModel_Charbuffer(){//��������������buffer����һ��ָ��ģ��
	if (  Command(PS_RegModel,0xef)&& (g_FifoNumber==11) && (g_FIFO[9]==0x00) )
	{
		return 1;//�ɹ�
	}else{
		return 0;
	}
}

bit FM_GetImage(void){//̽����ָ������ͼ��ͼ��Ĵ�����
	unsigned char i=0;
	for(i=0;i<40;i++){
		if(Command(PS_GetImage,0xef)&&(g_FifoNumber==11)&&(g_FIFO[9]==0x00)){//����ͼ��imgbuffer�ɹ�
			return 1;
		}else{
			;
		}
	}
	return 0;//40�κ��ǲ��ɹ�
}

unsigned char FM_Search(void)//�����û�,������ƥ���ָ�ƺţ������0xff���Ǵ���� 
{
 	unsigned char matchnum=0,i=0;
	bit ok1=0,ok2=0;
  	while (i<2)//��̫���ܳ�
    {
			ok1=FM_GetImage();//��ȡͼ��
			ok2=FM_CreatChar_buffer(1);//��ͼ��1����������ŵ�buffer1��
     	if ( ok1&&ok2 ) //
      {
       		matchnum=FM_Searchfinger1();//����ָ�Ʊȶԣ������������������������ָ�����
       		if(!(matchnum==0xff))//������ز���0xff������������������ָ�ƺ�
       		{
       			return matchnum; 
       		}else {
						; //�������ʧ��ʲô�����ɽ����´�ѭ��
					}     
      }
			i++;	
    }
   return 255;//�����255
}

unsigned char FM_Searchfinger1(void)//����ָ��(������������Լ����ݷ���ֵȷ���Ƿ����),����ƥ���,��buffer1        
{		
   	if(Command(PS_Search1,0xef) && (g_FifoNumber==15) && (g_FIFO[9]==0x00) )  
    {
		//SearchNumber=FIFO[10]*0x100+FIFO[11];//��������ҳ��
		//MatchScore=FIFO[12]*0x100+FIFO[13]   �����ڴ˼���÷֣��Ӷ����а�ȫ�����趨�����������
	   	return g_FIFO[11];//ֻ���ص�λ��
		}else{
       	return 0xff;//�������
    }  
}





