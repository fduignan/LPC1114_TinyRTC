#include "i2c.h"
#include "ds1307.h"
#include "lpc111x.h"
#include "serial.h"

void DS1307SetDate(DS1307Date *theDate)
{
    DS1307WriteByte(SECOND,theDate->Second);
    DS1307WriteByte(MINUTE,theDate->Minute);
    DS1307WriteByte(HOUR,theDate->Hour);
    DS1307WriteByte(DAY,theDate->Day);
    DS1307WriteByte(DATE,theDate->Date);
    DS1307WriteByte(MONTH,theDate->Month);
    DS1307WriteByte(YEAR,theDate->Year);
}

void DS1307GetDate(DS1307Date *theDate)
{
    DS1307ReadByte(SECOND,&theDate->Second);
    DS1307ReadByte(MINUTE,&theDate->Minute);
    DS1307ReadByte(HOUR,&theDate->Hour);
    DS1307ReadByte(DAY,&theDate->Day);
    DS1307ReadByte(DATE,&theDate->Date);
    DS1307ReadByte(MONTH,&theDate->Month);
    DS1307ReadByte(YEAR,&theDate->Year);
}

int DS1307Init()
{
    I2CInit();
    DS1307WriteByte(SECOND,0); // enable the oscillator
        
}
int DS1307WriteByte(uint16_t address,  uint8_t data)
{
        I2CTransaction Trans;
	Trans.SlaveAddress = DS1307_ADDRESS;
	Trans.Count = 2;
	Trans.Mode = 'w';
	Trans.Data[0]=address & 0xff;
	Trans.Data[1]=data;
	I2CDoTransaction(&Trans);
/*// Write cycles can take up to 10ms (an age) so do a dummy read which 
// will not return until the write cycle is complete.        
        Trans.SlaveAddress = DS1307_ADDRESS;
	Trans.Count = 1;
	Trans.Mode = 'r';
	Trans.Data[0]=address >> 8;
	Trans.Data[1]=address & 0xff;
	Trans.Data[2]=data;
	I2CDoTransaction(&Trans);*/
}
int DS1307ReadByte(uint16_t address,  uint8_t *data)
{
    // Do a dummy write to set the internal address pointer in the at24c32
        I2CTransaction Trans;
        Trans.SlaveAddress = DS1307_ADDRESS;
	Trans.Count = 1;
	Trans.Mode = 'w';
	Trans.Data[0]=address & 0xff;
	Trans.Data[1]=0;
	I2CDoTransaction(&Trans);
   // now do a read operation     
	Trans.SlaveAddress = DS1307_ADDRESS;
	Trans.Count = 1;
	Trans.Mode = 'r';
	Trans.Data[0]=0;
        Trans.Data[1]=0;
	I2CDoTransaction(&Trans);
	*data = Trans.Data[0];
	return 0;
}
