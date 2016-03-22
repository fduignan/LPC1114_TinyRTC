#include <stdint.h>	
#include "lpc111x.h"
#include "i2c.h"
// Will use a file operations pattern to access the I2C system
// Only one transaction can processed at a time 
I2CTransaction * pTransaction;
//#define DEBUG 1
#ifdef DEBUG
#include "serial.h"
#define MAX_DEBUG_TRACE 32
int DebugTraceData[MAX_DEBUG_TRACE];
int DebugIndex=0;
void DebugTrace(int Value)
{
	
	if (DebugIndex < MAX_DEBUG_TRACE)
	{
		DebugTraceData[DebugIndex++]=Value;
	}
}
#endif
int I2CInit()
{	
	unsigned Divisor;
	SYSAHBCLKCTRL |= BIT5 | BIT16; // enable IOCON and I2C	
	IOCON_PIO0_5 = 1; // select SDA mode for PIO0_5, standard I2C
	IOCON_PIO0_4 = 1; // select SCL mode for PIO0_4, standard I2C
	PRESETCTRL |= BIT1; // de-assert reset on I2C
	Divisor = 48000000 / (2*100000); // configure for 100kHz @ 48MHz PCLK
	I2C0SCLH = (Divisor >> 16);
	I2C0SCLL = (Divisor & 0xffff);
	ISER = BIT15; // enable I2C interrupts in NVIC
	I2C0CONSET = BIT6; // enable the I2C system
	
	return 0;

}
int I2CDoTransaction(I2CTransaction *pTrans)
{
	unsigned Timeout=0xfffff;
	pTransaction = pTrans;
	pTransaction->Index=0;
	pTransaction->Status=0;
        I2CStart();
	// Now need to wait for the transaction to complete.  The thing is, how long and how do I know
	while ( (pTransaction->Status ==  0) && (Timeout--) ); // Wait for transaction to complete.
	#ifdef DEBUG
	int Index;
	printString("dbg\r\n");
	for (Index = 0; Index < MAX_DEBUG_TRACE; Index++)
	{
		printByte(DebugTraceData[Index]);
		DebugTraceData[Index] = 0; // clear out debug trace for next time around
		printString(",");
		while(tx_count()); // Wait for transmitter FIFO to drain
	}
	DebugIndex = 0;
        
	printString("\r\n");
	#endif

	if (pTransaction->Status)
		return -1;  // something went wrong maybe. 
	else
		return 0; 	//  Not clear about status values and success - come back here.
}
void I2CISR()
{  // See page 271 on UM10398 for LPC1114.  Only using master mode states
	switch(I2C0STAT)
	{
		case 0x00: {
			// Bus error
			I2C0CONSET = 0x14; // set STO and AA
			I2C0CONCLR = 0x08; // clear SI 
			#ifdef DEBUG
			DebugTrace(0x00);
			#endif
			break;
		}
		case 0x08:{
			// Start condition sent			
			//	 -> send slave address + R/W
			//	 -> clear STA
			//	 -> expect ACK
			if (pTransaction->Mode == 'w')
				I2C0DAT = (pTransaction->SlaveAddress << 1);  
			else 
				I2C0DAT = (pTransaction->SlaveAddress << 1) | 1;	
			I2C0CONCLR = BIT5; // stop sending STA
			I2C0CONSET = 0x04; // set AA flag
			I2C0CONCLR = 0x08; // clear the SI flag
			#ifdef DEBUG
			DebugTrace(0x08);
			#endif
			break;
		}
		case 0x10:{
			// Repeated start condition sent 
			// if Writing: 			
			//	 -> send slave address + W
			// 	 -> clear STA 
			//	 -> expect ACK
			if (pTransaction->Mode == 'w')
			{
				I2C0DAT = (pTransaction->SlaveAddress << 1);  
			}
			// if Reading:
			// 	 -> send slave address + R
			// 	 -> clear STA 			
			if (pTransaction->Mode == 'r')
			{				
				I2C0DAT = (pTransaction->SlaveAddress << 1) + BIT0;
				
			}
			I2C0CONCLR = BIT5; // Clear STA
			I2C0CONSET = 0x04; // set AA bit
			I2C0CONCLR = 0x08; // set SI flag
			#ifdef DEBUG
			DebugTrace(0x10);
			#endif
			break;
		}
		case 0x18:{
			// SLA+W has been transmitted, ACK received
			// Time to send the first data byte			
			// 	-> Send first data byte			
			// 	-> Set AA, clear SI
			//	-> expect ACK			
			I2C0DAT = pTransaction->Data[pTransaction->Index++];
			I2C0CONSET = 0x04; // Set Auto Acknowledge bit
			I2C0CONCLR = 0x08; // Clear the SI flag			
			#ifdef DEBUG
			DebugTrace(0x18);
			#endif
			break;
		}
		case 0x20:{
			// SLA+W has been transmitted, NOT ACK received
			// Send STOP
			// 	-> Set STO and AA bits
			//	-> clear SI			
			I2C0CONSET = BIT4+BIT2; // set STO and AA
			I2C0CONCLR = BIT3; // clear the SI flag
			pTransaction->Status = 0x20; // update status field so higher level can debug
			#ifdef DEBUGIndex
			DebugTrace(0x20);
			#endif
			break;
		}
		case 0x28:{
			// Data byte has been sent, ACK received			
			// If there is more data send it, otherwise send Stop			
                       
			if (pTransaction->Index < pTransaction->Count)
			{
				I2C0DAT = pTransaction->Data[pTransaction->Index++];
				I2C0CONSET = 0x04; // set AA
				I2C0CONCLR = 0x08; // clear SI
				#ifdef DEBUG
                                DebugTrace(0x27);  // using a 'fake' status here to help me track what is going on
                                #endif
			}
			else
			{
				I2C0CONSET = 0x14; // set STO and AA					
				I2C0CONCLR = 0x08; // clear SI
                                pTransaction->Status = 0x28; // update status field so higher level can debug/move on
                                #ifdef DEBUG
                                DebugTrace(0x28);
                                #endif
			}
			
			break;
		}
		case 0x30:{
			// Data byte has been sent, NOT ACK received			
			I2C0CONSET = 0x14; // set STO and AA
			I2C0CONCLR = 0x08; // clear SI
			pTransaction->Status = 0x30; // update status field so higher level can debug	
			#ifdef DEBUG
			DebugTrace(0x30);
			#endif		
			break;
		}
		case 0x38:{
			// Arbitration lost!
			I2C0CONSET = 0x24; // set STA and AA
			I2C0CONCLR = 0x08; // clear SI
			pTransaction->Status = 0x38; // update status field so higher level can debug
			#ifdef DEBUG
			DebugTrace(0x38);
			#endif
			break;
		}
		case 0x40:{
			// Slave address + read transmitted, ACK received
			I2C0CONSET = 0x04; // set AA
			I2C0CONCLR = 0x08; // clear SI
			#ifdef DEBUG
			DebugTrace(0x40);
			#endif
			break;
		}
		case 0x48: {
			// Slave address + read transmitted, NOT ACK received
			I2C0CONSET = 0x14; // set STO and AA
			I2C0CONCLR = 0x08; // clear SI
			pTransaction->Status = 0x48; // update status field so higher level can debug			
			#ifdef DEBUG
			DebugTrace(0x48);
			#endif
			break;
		}
		case 0x50:{
			// Data received, ACK returned, need to read data
			pTransaction->Data[pTransaction->Index++]=I2C0DAT;
			if (pTransaction->Index >= pTransaction->Count)
			{
				// Got all expected data
				I2C0CONCLR = 0x0c; // clear SI and AA
				//pTransaction->Status = 0x50; // update status field so higher level can debug
			}
			else
			{
				// More data to come
				I2C0CONSET = 0x04; // set AA
				I2C0CONCLR = 0x08; // clear SI								
			}
			#ifdef DEBUG
			DebugTrace(0x50);
			#endif
			break;
		}
		case 0x58:{
			// Not clear what state of affairs calls this.
			// Data received, NOT ACK returned so read last byte
			pTransaction->Data[pTransaction->Index++]=I2C0DAT;
			I2C0CONSET = 0x14; // set STO and AA
			I2C0CONCLR = 0x08; // clear SI
			pTransaction->Status = 0x58; // update status field so higher level can debug/ move on
			#ifdef DEBUG
			DebugTrace(0x58);
			#endif
			break;
		}
	}	
}

void I2CStart()
{
	I2C0CONSET = BIT5; // send a start signal
}
void I2CStop(void)
{
	I2C0CONSET = BIT4; // send a stop signal
}

