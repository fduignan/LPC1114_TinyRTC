#include "i2c.h"
#include "at24c32.h"
#include "lpc111x.h"
#include "serial.h"

int AT24C32Init()
{
	return I2CInit();
}
int AT24C32WriteByte(uint16_t address,  uint8_t data)
{
        I2CTransaction Trans;
	Trans.SlaveAddress = AT24C32_ADDRESS;
	Trans.Count = 3;
	Trans.Mode = 'w';
	Trans.Data[0]=address >> 8;
	Trans.Data[1]=address & 0xff;
	Trans.Data[2]=data;
	I2CDoTransaction(&Trans);
// Write cycles can take up to 10ms (an age) so do a dummy read which 
// will not return until the write cycle is complete.        
        Trans.SlaveAddress = AT24C32_ADDRESS;
	Trans.Count = 1;
	Trans.Mode = 'r';
	Trans.Data[0]=address >> 8;
	Trans.Data[1]=address & 0xff;
	Trans.Data[2]=data;
	I2CDoTransaction(&Trans);
}
int AT24C32ReadByte(uint16_t address,  uint8_t *data)
{
    // Do a dummy write to set the internal address pointer in the at24c32
        I2CTransaction Trans;
        Trans.SlaveAddress = AT24C32_ADDRESS;
	Trans.Count = 2;
	Trans.Mode = 'w';
	Trans.Data[0]=address >> 8;
	Trans.Data[1]=address & 0xff;
	Trans.Data[2]=0;
	I2CDoTransaction(&Trans);
   // now do a read operation     
	Trans.SlaveAddress = AT24C32_ADDRESS;
	Trans.Count = 1;
	Trans.Mode = 'r';
	Trans.Data[0]=0;
        Trans.Data[1]=0;
	I2CDoTransaction(&Trans);
	*data = Trans.Data[0];
	return 0;
}
