// ds1307 RTC

#define DS1307_ADDRESS 0x68
// Offset definitions
#define SECOND 0
#define MINUTE 1
#define HOUR 2
#define DAY 3
#define DATE 4
#define MONTH 5
#define YEAR 6
#define CONTROL 7
#define RAM_START 8
#define RAM_SIZE 56

typedef struct {
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Day;
    uint8_t Date;
    uint8_t Month;
    uint8_t Year;   
} DS1307Date;

int DS1307Init();
int DS1307WriteByte(uint16_t address,  uint8_t data);
int DS1307ReadByte(uint16_t address,  uint8_t *data);
void DS1307GetDate(DS1307Date *theDate);
void DS1307SetDate(DS1307Date *theDate);
