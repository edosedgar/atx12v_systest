
#include <stm32f10x_gpio.h>
#include <stm32f10x.h>

#define LCD_E	GPIO_Pin_8
#define LCD_RS  GPIO_Pin_9
#define LCDPort GPIOB
//===========================================
void OutHalfByte(uint32_t HalfByte);
void Delay(volatile uint32_t nCount);
void InitLCD(void);

void WriteData(uint32_t Data);
void WriteCommand(uint32_t Command);

void RefreshLCD(void);
void ClrScr(void);
void WriteChar(int8_t X,int8_t Y,uint8_t Char);
void WriteText(int8_t X,int8_t Y,char *Text);
char* IntToString(uint32_t Var,char* ArrayVar);

//===========================================

uint8_t LCDBuffer[2][20];

void OutHalfByte(uint32_t HalfByte)
{
	GPIOC->ODR=((GPIOC->ODR&0xFF7F)+((HalfByte&0x08)<<4));
	GPIOC->ODR=((GPIOC->ODR&0xFFBF)+((HalfByte&0x04)<<4));
	GPIOB->ODR=((GPIOB->ODR&0x7FFF)+((HalfByte&0x02)<<14));
	GPIOB->ODR=((GPIOB->ODR&0xFFBF)+((HalfByte&0x01)<<6));
}

void Delay(volatile uint32_t nCount)
{
    for (; nCount > 0; nCount--);
};

void WriteData(uint32_t Data)
{
	GPIO_WriteBit(LCDPort,LCD_RS,Bit_SET);
	OutHalfByte(Data>>4);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    OutHalfByte(Data&0x0F);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    GPIO_WriteBit(LCDPort,LCD_RS,Bit_RESET);
    Delay(300);
};

void WriteCommand(uint32_t Command)
{
	GPIO_WriteBit(LCDPort,LCD_RS,Bit_RESET);
	OutHalfByte(Command>>4);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    OutHalfByte(Command&0x0F);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
    Delay(10);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
    Delay(300);
};

void InitLCD(void)
{
	Delay(300000);
	GPIO_WriteBit(LCDPort,LCD_RS,Bit_RESET);
	GPIO_WriteBit(LCDPort,LCD_E ,Bit_SET);
	OutHalfByte(0x02);
    GPIO_WriteBit(LCDPort,LCD_E ,Bit_RESET);
	Delay(300);
    WriteCommand(0x2C);
    WriteCommand(0x0C);
    WriteCommand(0x06);
    WriteCommand(0x01);
    Delay(300000);
};

void WriteChar(int8_t X,int8_t Y,uint8_t Char)
{
	if ((X==0)||(Y==0)) {return;};
	SystemSettings.IsRefreshLCD=1;
	LCDBuffer[Y-1][X-1]=Char;
};

void WriteText(int8_t X,int8_t Y,char *Text)
{
	SystemSettings.IsRefreshLCD=1;
	while (*Text)
	{
		LCDBuffer[Y-1][X-1]=*Text++;
		X++;
	}
};

void ClrScr(void)
{
	uint8_t i;
	SystemSettings.IsRefreshLCD=1;
	for (i=0;i<20;i++) {LCDBuffer[0][i]=' ';};
	for (i=0;i<20;i++) {LCDBuffer[1][i]=' ';};
}

void RefreshLCD(void)
{
	uint8_t i;
	SystemSettings.IsRefreshLCD=0;
	WriteCommand(0x80);
	for (i=0;i<20;i++) {WriteData(LCDBuffer[0][i]);};
	WriteCommand(0xC0);
	for (i=0;i<20;i++) {WriteData(LCDBuffer[1][i]);};
}

char* IntToString(uint32_t Var,char* ArrayVar)
{
	uint8_t j=0;
	ArrayVar[0]=Var/1000+0x30;
	Var-=1000*(ArrayVar[0]-0x30);
	ArrayVar[1]=Var/100+0x30;
	Var-=100*(ArrayVar[1]-0x30);
	ArrayVar[2]=Var/10+0x30;
	Var-=10*(ArrayVar[2]-0x30);
	ArrayVar[3]=Var/1+0x30;
	Var-=1*(ArrayVar[3]-0x30);
	ArrayVar[4]=0;
	while ((ArrayVar[j]=='0')&&(j!=3))
	{
		ArrayVar[j]=' ';
		j++;
	}
	return ArrayVar;
}
