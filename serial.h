#include <stdint.h>
#define SBUFSIZE 128
typedef struct 
{
	volatile unsigned head,tail,count;
	char buffer[SBUFSIZE];
} SerialBuffer;

void initUART(void);
void putBuf(SerialBuffer *sbuf, char c);
char getBuf(SerialBuffer *sbuf);
int rx_count();
int tx_count();
char egetc(void);
void eputc(char c);
void printString(char *s);
void printInteger(unsigned int Number);
void printByte(uint8_t Number);
