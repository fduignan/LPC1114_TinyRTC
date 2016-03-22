#include "lpc111x.h"
#include "at24c32.h"
#include "ds1307.h"
#include "i2c.h"
#include "serial.h"
DS1307Date TheDate;
void delay(int dly)
{
    while(dly--);
}

int main()
{
    uint8_t InData=0;
    uint8_t OutData=0;
    DS1307Init();
    AT24C32Init();
    initUART();
    enable_interrupts();
    int value=0;
    TheDate.Second = 0;
    TheDate.Minute = 0x59;
    TheDate.Hour = 0x23;
    TheDate.Day = 2;
    TheDate.Date = 0x22;
    TheDate.Month = 3;
    TheDate.Year = 0x16;
    DS1307SetDate(&TheDate);
    while(1)
    {
        AT24C32WriteByte(1,OutData);
        AT24C32ReadByte(1,&InData);
        printString("AT24C32 Out: ");
        printByte(OutData);
        printString(", In: ");
        printByte(InData);
        OutData++;
        printString(". DS1307: ");
        DS1307GetDate(&TheDate);
        printByte(TheDate.Year);printString(":");
        printByte(TheDate.Month);printString(":");
        printByte(TheDate.Date);printString(":");
        printByte(TheDate.Day);printString(":");
        printByte(TheDate.Hour);printString(":");
        printByte(TheDate.Minute);printString(":");
        printByte(TheDate.Second);printString("\r\n");        
        delay(1000000); 
    }
}
