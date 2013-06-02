//功能：本程序适用HC-SR04测量距离，范围2cm--450cm，适用LCD1602显示距离
//时间：2012年7月5日
#include <AT89x51.h>		
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
#define  TX  P1_1		
#define  RX  P1_2
#define LCM_Data  P0
#define Busy    0x80 
unsigned int  time=0;//用于存放定时器时间值
unsigned long S=0;//用于存放距离的值
bit  flag =0; //量程溢出标志位
		
unsigned char code Range[] ="==Range Finder==";//LCD1602显示格式
unsigned char code ASCII[13] = "0123456789.-M";
unsigned char code table[]="Distance:000.0cm";
unsigned char code table1[]="!!! Out of range";
sbit RS=P2^0;
sbit RW=P2^1;
sbit E=P2^2;
/************延时5ms*************************/
void Delay5Ms(void)//
{
	uint TempCyc = 5552;
	while(TempCyc--);
}
/******************读状态*****************/
uchar ReadStatusLCM(void)
{
	LCM_Data=0xFF; 
	RS=0;
	RW=1;
	E=0;
	E=0;
	E=1;
	while (LCM_Data & Busy); //检测忙信号
	return(LCM_Data);
}
/***************写数据********************/
void WriteDataLCM(uchar WDLCM) 
{
	ReadStatusLCM(); //检测忙
	LCM_Data = WDLCM;
	RS=1;
	RW=0;
	E=0; //若晶振速度太高可以在这后加小的延时
	E=0; //延时
	E=1;
}
/*****************写指令*****************/
void WriteCommandLCM(uchar WCLCM,BuysC) //BuysC为0时忽略忙检测
{
	if(BuysC) ReadStatusLCM(); //根据需要检测忙
	LCM_Data = WCLCM;
	RS=0;
	RW=0;	
	E=0;
	E=0;
	E=1;	
}
/*******************LCD1602初始化****************/
void LCMInit(void) 
{
	LCM_Data = 0;
	WriteCommandLCM(0x38,0); //三次显示模式设置，不检测忙信号
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 

	WriteCommandLCM(0x38,1); //显示模式设置,开始要求每次检测忙信号
	WriteCommandLCM(0x08,1); //关闭显示
	WriteCommandLCM(0x01,1); //显示清屏
	WriteCommandLCM(0x06,1); // 显示光标移动设置
	WriteCommandLCM(0x0c,1); // 显示开及光标设置
}
/*********在指定位置显示一个字符********************/
void DisplayOneChar(uchar X, uchar Y, uchar DData)
{
	Y &= 0x1;
	X &= 0xF; //限制X不能大于15，Y不能大于1
	if (Y) X |= 0x40; //当要显示第二行时地址码+0x40;
	X |= 0x80; //算出指令码
	WriteCommandLCM(X, 1); //发命令字
	WriteDataLCM(DData); //发数据
}
/*********************在指定位置显示字符串*************/
void DisplayListChar(uchar X, uchar Y, uchar code *DData)
{
	uchar ListLength;

  	ListLength = 0;
	Y &= 0x1;
	X &= 0xF; //限制X不能大于15，Y不能大于1
	while (DData[ListLength]>0x19) //若到达字串尾则退出
		{
			if (X <= 0xF) //X坐标应小于0xF
				{
					DisplayOneChar(X, Y, DData[ListLength]); //显示单个字符
					ListLength++;
					X++;
				}
		}
}
/*****************延时400ms****************/
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
/****************计算测量距离*******************/
void Conut(void)
{
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;
	
	S=time*1.7; 
	S=S/10; 
	if((S>=5000)||flag==1) //超出测量范围
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
/*****************计算器0******************************/
void zd0() interrupt 1 		 //T0中断用来计数器溢出,超过测距范围
{
	flag=1;			 //中断溢出标志
	RX=0;
}

/********超声波高电平脉冲宽度计算程序***************/
void Timer_Count(void)
{
	TR0=1;			    //开启计数
	while(RX);			//当RX为1计数并等待
	TR0=0;				//关闭计数
	Conut();			//计算

}
/************HC-SR04发送测量信号********************************************/
void  StartModule() 		         //启动模块
{
	uchar i;
	TX=1;			                     //启动一次模块
	for(i=0;i<15;i++)
	{
		_nop_(); 
	}
	TX=0;
}
/*************主程序***************************/
void main(void)
{
    uint a;
	Delay400Ms(); //启动等待，等LCM讲入工作状态
	LCMInit(); //LCM初始化
	Delay5Ms(); //延时片刻
	DisplayListChar(0, 0, Range);
	DisplayListChar(0, 1, table);
    TMOD=0x01;//设T0为方式1，GATE=1；
    EA=1;//开启总中断
    TH0=0;
    TL0=0;          
    ET0=1;             //允许T0中断
				   			
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

