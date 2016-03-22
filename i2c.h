// i2c.h
// API for the NXP LPC1114FN28 I2C interface
#include <stdint.h>
// Will need to revisit this figure for max data size
#ifndef __I2C_H
#define __I2C_H
#define MAX_I2C_DATA 64
int I2CInit();	
void I2CStop(void);
void I2CStart();
typedef struct {
	int Mode; // can be 'r' or 'w'
	//uint8_t StopWhenDone;
	uint8_t SlaveAddress;  // Use the 7 bit address here
	unsigned Index;
	unsigned Count;
        int Status;
	uint8_t Data[MAX_I2C_DATA];
} I2CTransaction;
int I2CDoTransaction(I2CTransaction *pTransaction);
#endif
