//���ܣ�����������HC-SR04�������룬��Χ2cm--450cm������LCD1602��ʾ����
//ƽ̨��ATMEGA64
//��˿λ/fuse bytes��
//		  ���ֽ�/low byte��0xc1
//		  ���ֽ�/high byte��0xd9
//		  ��չ�ֽ�/extend��0xFF
//		  �����ֽ�/Lock byte��0xff;
//�����ڲ�RC����1MHz
//ʱ�䣺2012��7��23��
#include <avr/io.h>	
#include <avr/interrupt.h>
//#include <avr/macros.h>	
#define uint unsigned int
#define uchar unsigned char
unsigned int  time=0;//���ڴ�Ŷ�ʱ��ʱ��ֵ
unsigned long S=0;//���ڴ�ž����ֵ
uchar  flag =0; //���������־λ
volatile int num=0;		
unsigned char  Range[] ="==Range Finder==";//LCD1602��ʾ��ʽ
unsigned char  ASC[] = "0123456789.-M";
unsigned char  table[]="Distance:000.0cm";
unsigned char  table1[]="!!! Out of range";
uchar cont;
//RS--PC0
//RW--PC1
//E--PC2
//TX--PD0
//RX--PD1
/************��ʱ5ms************************/
void Delay5Ms(void)//
{
	uint TempCyc = 5000;
	while(TempCyc--);
}
void delay(uint x)
{
	uint i,j;
	for(i=0;i<x;i++)
		for(j=0;j<100;j++);
}
/***************LCD1602д����********************/
void WriteDataLCM(uchar WDLCM) 
{

	PORTC &=~(1<<PC1)|(1<<PC2);
	PORTC |=(1<<PC0);
	PORTA = WDLCM;
	PORTC |=(1<<PC2);	//E--PC2
	delay(2);
	PORTC &=~(1<<PC2);	//E--PC2
}
/*****************LCD1602дָ��*****************/
void WriteCommandLCM(uchar WCLCM) 
{
	PORTC &=~(1<<PC0)|(1<<PC1)|(1<<PC2);
	PORTA = WCLCM;
	PORTC |=(1<<PC2);//E--PORTC2
	delay(2);
	PORTC &=~(1<<PC2);//E--PORTC2
}
/*******************LCD1602��ʼ��****************/
void LCMInit(void) 
{
	delay(150);
	WriteCommandLCM(0x38); //������ʾģʽ���ã������æ�ź�
	delay(50); 
	WriteCommandLCM(0x38);
	delay(50);; 
	WriteCommandLCM(0x38);
	WriteCommandLCM(0x38); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
	WriteCommandLCM(0x08); //�ر���ʾ
	WriteCommandLCM(0x01); //��ʾ����
	WriteCommandLCM(0x06); // ��ʾ����ƶ�����
	WriteCommandLCM(0x0c); // ��ʾ�����������
}
/*********��ָ��λ����ʾһ���ַ�********************/
void DisplayOneChar(uchar X, uchar Y, uchar DData)
{
	Y &= 0x1;
	X &= 0xF; //����X���ܴ���15��Y���ܴ���1
	if (Y) X += 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
	X += 0x80; //���ָ����
	WriteCommandLCM(X); //��������
	WriteDataLCM(DData); //������
}
/*****************��ʱ400ms****************/
void Delay400Ms(void)
{
	uchar TempCycA = 5;
	uint TempCycB;
	while(TempCycA--)
	{
		TempCycB=8000;
		while(TempCycB--);
	};
}
/****************�����������********************/
void Conut(void)
{
	if(num>256) 
	{
		flag=1;
		num=0;
	}
	time=num*256+TCNT0;//����ʱ��
	num=0;
	TCNT0 = 0; 
	
	S=time*17; //������룬���㵥λmm
	S=S/10; 
	if((S>=5000)||flag==1||(S<20)) //����������Χ
	{	
		flag=0;
		S=0;
		num=0;
		for(cont=0;cont<16;cont++) DisplayOneChar(cont, 1, table1[cont]);
									//��LCD1602�ĵ�һ�е�cont����ʾ�ַ�table1[cont]
	}
	else
	{
		for(cont=0;cont<16;cont++) DisplayOneChar(cont, 1, table[cont]);//��ʾ���뵥λ��cm
		DisplayOneChar(9, 1, ASC[S/1000]);
		DisplayOneChar(10, 1, ASC[(S/100)%10]);	
		DisplayOneChar(11, 1, ASC[(S/10)%10]);
		DisplayOneChar(12, 1, ASC[10]);
		DisplayOneChar(13, 1, ASC[S%10]); 
	}
}
/********�������ߵ�ƽ������ȼ������****************/
void Timer_Count(void)
{
	TCCR0 |= (1 << CS01);    //��������
	while(PIND & (1<<PD1));			//��RXΪ1�������ȴ�
	TCCR0 = 0x00;				//�رռ���
	Conut();			//����
}

/************HC-SR04���Ͳ����ź�********************************************/
void  StartModule() 		         //����ģ��
{
	uchar i;
	PORTD |=(1<<PD0);//TX=1;	//����һ��ģ��
	for(i=0;i<15;i++);
	PORTD &=~(1<<PD0);//TX=0;
}

/*************������***************************/
int main(void)
{
    uint a;
	DDRA |=0XFF;//
	DDRC |=0XFF;//
	DDRD |=0XFD;//

	TCNT0 = 0;
	TIMSK |= (1 << TOIE0);//����������0����ж�
	TCCR0 &= 0x00;//�ؼ�����0
	sei();//��ȫ���ж�

	Delay400Ms(); //�����ȴ�����LCM���빤��״̬
	LCMInit(); //LCM��ʼ��
	Delay5Ms(); //��ʱƬ��
	for(a=0;a<16;a++) DisplayOneChar(a, 0, Range[a]);
	for(a=0;a<16;a++) DisplayOneChar(a, 1, table[a]);
			   			
 	while(1)
	{
		StartModule();
		for(a=8000;a>0;a--)
		{			
			if(PIND & (1<<PD1))//RX==1
			{
				Timer_Count();
			}
		}
	}
} 
/*************������0********************/
SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = 0;
	num++;
}