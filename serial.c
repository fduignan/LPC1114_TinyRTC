// Serial I/O routines
#include "serial.h"
#include "lpc111x.h"
SerialBuffer TXBuffer,RXBuffer;
void UART_isr(void);
void initUART()
{
	RXBuffer.count = RXBuffer.head = RXBuffer.tail = 0;
	TXBuffer.count = TXBuffer.head = TXBuffer.tail = 0;

	SYSAHBCLKCTRL |= BIT6 + BIT16; // Turn on clock for GPIO and IOCON
	// Enable UART RX function on PIO1_6
	IOCON_PIO1_6 |= BIT0; 
	IOCON_PIO1_6 &= ~(BIT1+BIT2);
	// Enable UART TX function on PIO1_7
	IOCON_PIO1_7 |= BIT0;
	IOCON_PIO1_7 &= ~(BIT1+BIT2);
	// Turn on clock for UART
	SYSAHBCLKCTRL |= BIT12;
	UARTCLKDIV = 1;
	// PCLK = 48Mhz. Desired Baud rate = 9600
	// See table 199
	// 9600=48MHz/(16* (256*U0DLM + U0DLL)*(1+DivAddVal/MulVal))
	// 312.5 = (256*U0DLM+U0DLL)*(1+DivAddVal/MulVal)
	// let U0DLM=1, DivAddVal=0,MulVal =1
	// 312.5=256+U0DLL
	// U0DLL=56.5.  
	// Choose U0DLL=56.
	// Actual baud rate achieved = 9615 - close enough.
	U0LCR |= BIT7; // Enable divisor latch access
	U0FDR = (1<<4)+0; // Set DivAddVal = 0; MulVal = 1
	U0DLL = 56;
	U0DLM = 1;
	U0LCR &= ~BIT7; // Disable divisor latch access
	U0LCR |= (BIT1+BIT0); // set word length to 8 bits.
	U0IER = BIT0+BIT1+BIT2;	// Enable UART TX,RX Data  interrupts
	ISER |= BIT21; 			// enable UART IRQ's in NVIC was 13
}
void UART_isr(void)
{
	int Source=U0IIR; // have to cache the interrupt id register
					  // as the 'if' clause below were reading it
					  // and hence changing its contents.
	if (Source & BIT2) // RX Interrupt
	{
		putBuf(&RXBuffer,U0RBR);
	}
	if (Source & BIT1) // TX Interrupt
	{
		if (TXBuffer.count > 0)
			U0THR = getBuf(&TXBuffer);
	}
}
void putBuf(SerialBuffer *sbuf, char c)
{
	if (sbuf->count < SBUFSIZE)
	{
		disable_interrupts();
		sbuf->count++;
		sbuf->buffer[sbuf->head] = c;
		sbuf->head=(sbuf->head+1) % SBUFSIZE;
		enable_interrupts();
	}
}
char getBuf(SerialBuffer *sbuf)
{
	char c=0;
	if (sbuf->count >0 )
	{
		disable_interrupts();
		sbuf->count--;
		c=sbuf->buffer[sbuf->tail];
		sbuf->tail=(sbuf->tail+1) % SBUFSIZE;
		enable_interrupts();
	}
	return c;
}
int rx_count()
{
	return RXBuffer.count;
}
int tx_count()
{
	return TXBuffer.count;
}
void eputc(char c)
{
	if (U0LSR & BIT6)
		U0THR = c; // Transmitter idle, so just write out directly
	else
	{
		putBuf(&TXBuffer,c);
	}
}
char egetc()
{
	return getBuf(&RXBuffer);
}
void printString(char *String)
{
	while(*String)
	{
		eputc(*String);
		String++;
	}
}
char HexDigit(int Value)
{
	if ((Value >=0) && (Value < 10))
		return Value+'0';
	else if ((Value >9) && (Value < 16))
		return Value-10 + 'A';
	else
		return 'z';
}
void printInteger(unsigned int Number)
{
	// Output the number over the serial port as
	// as hexadecimal string.
	char TxString[9];
	int Index=8;
	TxString[Index]=0; // terminate the string
	Index--;
	while(Index >=0)
	{
		TxString[Index]=HexDigit(Number & 0x0f);
		Number = Number >> 4;
		Index--;
	}
	printString(TxString);
}
void printByte(uint8_t Number)
{
	// Output the number over the serial port as
	// as hexadecimal string.
	char TxString[3];
	int Index=2;
	TxString[Index]=0; // terminate the string
	Index--;
	while(Index >=0)
	{
		TxString[Index]=HexDigit(Number & 0x0f);
		Number = Number >> 4;
		Index--;
	}
	printString(TxString);
}
