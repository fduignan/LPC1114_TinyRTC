# Specify the compiler to use
CC=arm-none-eabi-gcc
# Specify the assembler to use
AS=arm-none-eabi-as
# Specity the linker to use
LD=arm-none-eabi-ld

CCFLAGS=-mcpu=cortex-m0 -mthumb -g  
# Tell the linker where to find the libraries -> important: use thumb versions
#LIBSPEC=-L /home/fduignan/toolchain/gcc-arm-none-eabi/lib/gcc/arm-none-eabi/4.7.4/armv6-m
LIBSPEC=-L /usr/local/gcc-arm-none-eabi/lib/gcc/arm-none-eabi/5.2.1/armv6-m


# List the object files involved in this project
OBJS=	init.o i2c.o at24c32.o ds1307.o serial.o main.o 

# The default 'target' (output) is main.elf and it depends on the object files being there.
# These object files are linked together to create main.elf
main.elf : $(OBJS)
	$(LD) $(OBJS) $(LIBSPEC) -lgcc -T linker_script.ld --cref -Map main.map -nostartfiles -o main.elf
	objcopy -O ihex main.elf main.hex
	@echo "Assuming all went well..."
	@echo "Program the chip as follows: lpc21isp main.hex /dev/ttyUSB0 9600 12000000"
	@echo "replace ttyUSB0 with the correct device name for your system"	
# The object file main.o depends on main.c.  main.c is compiled to make main.o
main.o: main.c
	$(CC) -c $(CCFLAGS) main.c -o main.o

i2c.o: i2c.c
	$(CC) -c $(CCFLAGS) i2c.c -o i2c.o

init.o: init.c
	$(CC) -c $(CCFLAGS) init.c -o init.o

at24c32.o: at24c32.c
	$(CC) -c $(CCFLAGS) at24c32.c -o at24c32.o
ds1307.o: ds1307.c
	$(CC) -c $(CCFLAGS) ds1307.c -o ds1307.o

serial.o: serial.c
	$(CC) -c $(CCFLAGS) serial.c -o serial.o

# if someone types in 'make clean' then remove all object files and executables
# associated wit this project
clean: 
	rm $(OBJS) 
	rm main.elf 
