//功能：本程序适用HC-SR04测量距离，范围2cm--450cm，适用LCD1602显示距离
//平台：ATMEGA64
//熔丝位/fuse bytes：
//		  低字节/low byte：0xc1
//		  高字节/high byte：0xd9
//		  扩展字节/extend：0xFF
//		  加密字节/Lock byte：0xff;
//晶振：内部RC振荡器1MHz
//时间：2012年7月23日
#include <avr/io.h>	
#include <avr/interrupt.h>
//#include <avr/macros.h>	
#define uint unsigned int
#define uchar unsigned char
unsigned int  time=0;//用于存放定时器时间值
unsigned long S=0;//用于存放距离的值
uchar  flag =0; //量程溢出标志位
volatile int num=0;		
unsigned char  Range[] ="==Range Finder==";//LCD1602显示格式
unsigned char  ASC[] = "0123456789.-M";
unsigned char  table[]="Distance:000.0cm";
unsigned char  table1[]="!!! Out of range";
uchar cont;
//RS--PC0
//RW--PC1
//E--PC2
//TX--PD0
//RX--PD1
/************延时5ms************************/
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
/***************LCD1602写数据********************/
void WriteDataLCM(uchar WDLCM) 
{

	PORTC &=~(1<<PC1)|(1<<PC2);
	PORTC |=(1<<PC0);
	PORTA = WDLCM;
	PORTC |=(1<<PC2);	//E--PC2
	delay(2);
	PORTC &=~(1<<PC2);	//E--PC2
}
/*****************LCD1602写指令*****************/
void WriteCommandLCM(uchar WCLCM) 
{
	PORTC &=~(1<<PC0)|(1<<PC1)|(1<<PC2);
	PORTA = WCLCM;
	PORTC |=(1<<PC2);//E--PORTC2
	delay(2);
	PORTC &=~(1<<PC2);//E--PORTC2
}
/*******************LCD1602初始化****************/
void LCMInit(void) 
{
	delay(150);
	WriteCommandLCM(0x38); //三次显示模式设置，不检测忙信号
	delay(50); 
	WriteCommandLCM(0x38);
	delay(50);; 
	WriteCommandLCM(0x38);
	WriteCommandLCM(0x38); //显示模式设置,开始要求每次检测忙信号
	WriteCommandLCM(0x08); //关闭显示
	WriteCommandLCM(0x01); //显示清屏
	WriteCommandLCM(0x06); // 显示光标移动设置
	WriteCommandLCM(0x0c); // 显示开及光标设置
}
/*********在指定位置显示一个字符********************/
void DisplayOneChar(uchar X, uchar Y, uchar DData)
{
	Y &= 0x1;
	X &= 0xF; //限制X不能大于15，Y不能大于1
	if (Y) X += 0x40; //当要显示第二行时地址码+0x40;
	X += 0x80; //算出指令码
	WriteCommandLCM(X); //发命令字
	WriteDataLCM(DData); //发数据
}
/*****************延时400ms****************/
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
/****************计算测量距离********************/
void Conut(void)
{
	if(num>256) 
	{
		flag=1;
		num=0;
	}
	time=num*256+TCNT0;//计算时间
	num=0;
	TCNT0 = 0; 
	
	S=time*17; //计算距离，计算单位mm
	S=S/10; 
	if((S>=5000)||flag==1||(S<20)) //超出测量范围
	{	
		flag=0;
		S=0;
		num=0;
		for(cont=0;cont<16;cont++) DisplayOneChar(cont, 1, table1[cont]);
									//在LCD1602的第一行第cont列显示字符table1[cont]
	}
	else
	{
		for(cont=0;cont<16;cont++) DisplayOneChar(cont, 1, table[cont]);//显示距离单位：cm
		DisplayOneChar(9, 1, ASC[S/1000]);
		DisplayOneChar(10, 1, ASC[(S/100)%10]);	
		DisplayOneChar(11, 1, ASC[(S/10)%10]);
		DisplayOneChar(12, 1, ASC[10]);
		DisplayOneChar(13, 1, ASC[S%10]); 
	}
}
/********超声波高电平脉冲宽度计算程序****************/
void Timer_Count(void)
{
	TCCR0 |= (1 << CS01);    //开启计数
	while(PIND & (1<<PD1));			//当RX为1计数并等待
	TCCR0 = 0x00;				//关闭计数
	Conut();			//计算
}

/************HC-SR04发送测量信号********************************************/
void  StartModule() 		         //启动模块
{
	uchar i;
	PORTD |=(1<<PD0);//TX=1;	//启动一次模块
	for(i=0;i<15;i++);
	PORTD &=~(1<<PD0);//TX=0;
}

/*************主程序***************************/
int main(void)
{
    uint a;
	DDRA |=0XFF;//
	DDRC |=0XFF;//
	DDRD |=0XFD;//

	TCNT0 = 0;
	TIMSK |= (1 << TOIE0);//允许计数器0溢出中断
	TCCR0 &= 0x00;//关计数器0
	sei();//开全局中断

	Delay400Ms(); //启动等待，等LCM讲入工作状态
	LCMInit(); //LCM初始化
	Delay5Ms(); //延时片刻
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
/*************计数器0********************/
SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = 0;
	num++;
}
