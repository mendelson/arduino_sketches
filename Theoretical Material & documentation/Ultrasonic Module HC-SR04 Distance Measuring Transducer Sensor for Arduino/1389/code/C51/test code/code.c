//���ܣ�����������HC-SR04�������룬��Χ2cm--450cm������LCD1602��ʾ����
//ʱ�䣺2012��7��5��
#include <AT89x51.h>		
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
#define  TX  P1_1		
#define  RX  P1_2
#define LCM_Data  P0
#define Busy    0x80 
unsigned int  time=0;//���ڴ�Ŷ�ʱ��ʱ��ֵ
unsigned long S=0;//���ڴ�ž����ֵ
bit  flag =0; //���������־λ
		
unsigned char code Range[] ="==Range Finder==";//LCD1602��ʾ��ʽ
unsigned char code ASCII[13] = "0123456789.-M";
unsigned char code table[]="Distance:000.0cm";
unsigned char code table1[]="!!! Out of range";
sbit RS=P2^0;
sbit RW=P2^1;
sbit E=P2^2;
/************��ʱ5ms*************************/
void Delay5Ms(void)//
{
	uint TempCyc = 5552;
	while(TempCyc--);
}
/******************��״̬*****************/
uchar ReadStatusLCM(void)
{
	LCM_Data=0xFF; 
	RS=0;
	RW=1;
	E=0;
	E=0;
	E=1;
	while (LCM_Data & Busy); //���æ�ź�
	return(LCM_Data);
}
/***************д����********************/
void WriteDataLCM(uchar WDLCM) 
{
	ReadStatusLCM(); //���æ
	LCM_Data = WDLCM;
	RS=1;
	RW=0;
	E=0; //�������ٶ�̫�߿���������С����ʱ
	E=0; //��ʱ
	E=1;
}
/*****************дָ��*****************/
void WriteCommandLCM(uchar WCLCM,BuysC) //BuysCΪ0ʱ����æ���
{
	if(BuysC) ReadStatusLCM(); //������Ҫ���æ
	LCM_Data = WCLCM;
	RS=0;
	RW=0;	
	E=0;
	E=0;
	E=1;	
}
/*******************LCD1602��ʼ��****************/
void LCMInit(void) 
{
	LCM_Data = 0;
	WriteCommandLCM(0x38,0); //������ʾģʽ���ã������æ�ź�
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 

	WriteCommandLCM(0x38,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
	WriteCommandLCM(0x08,1); //�ر���ʾ
	WriteCommandLCM(0x01,1); //��ʾ����
	WriteCommandLCM(0x06,1); // ��ʾ����ƶ�����
	WriteCommandLCM(0x0c,1); // ��ʾ�����������
}
/*********��ָ��λ����ʾһ���ַ�********************/
void DisplayOneChar(uchar X, uchar Y, uchar DData)
{
	Y &= 0x1;
	X &= 0xF; //����X���ܴ���15��Y���ܴ���1
	if (Y) X |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
	X |= 0x80; //���ָ����
	WriteCommandLCM(X, 1); //��������
	WriteDataLCM(DData); //������
}
/*********************��ָ��λ����ʾ�ַ���*************/
void DisplayListChar(uchar X, uchar Y, uchar code *DData)
{
	uchar ListLength;

  	ListLength = 0;
	Y &= 0x1;
	X &= 0xF; //����X���ܴ���15��Y���ܴ���1
	while (DData[ListLength]>0x19) //�������ִ�β���˳�
		{
			if (X <= 0xF) //X����ӦС��0xF
				{
					DisplayOneChar(X, Y, DData[ListLength]); //��ʾ�����ַ�
					ListLength++;
					X++;
				}
		}
}
/*****************��ʱ400ms****************/
void Delay400Ms(void)
{
	uchar TempCycA = 5;
	uint TempCycB;
	while(TempCycA--)
	{
		TempCycB=7269;
		while(TempCycB--);
	};
}
/****************�����������*******************/
void Conut(void)
{
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;
	
	S=time*1.7; 
	S=S/10; 
	if((S>=5000)||flag==1) //����������Χ
	{	
		flag=0;
		DisplayListChar(0, 1, table1);
	}
	else
	{
		DisplayListChar(0, 1, table);
		DisplayOneChar(9, 1, ASCII[S/1000]);
		DisplayOneChar(10, 1, ASCII[S/100%10]);	
		DisplayOneChar(11, 1, ASCII[S/10%10]);
		DisplayOneChar(12, 1, ASCII[10]);
		DisplayOneChar(13, 1, ASCII[S%10]);
	}
}
/*****************������0******************************/
void zd0() interrupt 1 		 //T0�ж��������������,������෶Χ
{
	flag=1;			 //�ж������־
	RX=0;
}

/********�������ߵ�ƽ�����ȼ������***************/
void Timer_Count(void)
{
	TR0=1;			    //��������
	while(RX);			//��RXΪ1�������ȴ�
	TR0=0;				//�رռ���
	Conut();			//����

}
/************HC-SR04���Ͳ����ź�********************************************/
void  StartModule() 		         //����ģ��
{
	uchar i;
	TX=1;			                     //����һ��ģ��
	for(i=0;i<15;i++)
	{
		_nop_(); 
	}
	TX=0;
}
/*************������***************************/
void main(void)
{
    uint a;
	Delay400Ms(); //�����ȴ�����LCM���빤��״̬
	LCMInit(); //LCM��ʼ��
	Delay5Ms(); //��ʱƬ��
	DisplayListChar(0, 0, Range);
	DisplayListChar(0, 1, table);
    TMOD=0x01;//��T0Ϊ��ʽ1��GATE=1��
    EA=1;//�������ж�
    TH0=0;
    TL0=0;          
    ET0=1;             //����T0�ж�
				   			
 	while(1)
	{
		RX=1;
		StartModule();
		for(a=7510;a>0;a--)
		{			
			if(RX==1)
			{
				Timer_Count();
			}
		}
	}
} 

