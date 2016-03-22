// API for at24c32 serial eeprom (4 kB)
// The address bits are wired to GND on the test board.
#include <stdint.h>
#define AT24C32_ADDRESS ( 0xa0 >> 1 )
int AT24C32Init();
int AT24C32WriteByte(uint16_t address,  uint8_t data);
int AT24C32ReadByte(uint16_t address,  uint8_t *data);	
