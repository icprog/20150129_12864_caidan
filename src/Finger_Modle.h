#ifndef __FINGER_MODEL_H__
#define __FINGER_MODEL_H__
	bit FM_VefPSW(void);//��֤�豸���ֿ���,�ɹ�����1 
	void FM_Init();//��ʼ������Ҫ�Ǵ���	
	bit FM_Empty(void);//�������ָ��
	bit FM_CreatChar_buffer(unsigned char Bufid);
	bit FM_Save_model(unsigned char ID);
	unsigned char FM_ValidTempleteNum(bit addr);//���ؿ����õ�ģ����������0xff����������addr=0,�ͷ��ص�λ���ֽڣ����Ϊ1���ͷ��ظ�λ�ֽڣ�һ���λ�Ϳ����ˣ���λ��Ϊ0
	//bit FM_Enroll_One(unsigned char bufferid);//�ɼ�һ��ָ�ƣ����洢��bufferid��,����40��
	bit FM_RegModel_Charbuffer();//��������������buffer����һ��ָ��ģ��
	bit FM_GetImage(void);//̽����ָ������ͼ��ͼ��Ĵ�����
	unsigned char FM_Searchfinger1(void);//����ָ��(������������Լ����ݷ���ֵȷ���Ƿ����),����ƥ���,��buffer1  
	unsigned char FM_Search(void);//��ȡָ�ƣ������û�,������ƥ���ָ�ƺţ������0xff���Ǵ���� 
#endif