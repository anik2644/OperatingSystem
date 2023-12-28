# CC refers to cross compiler
CC=arm-none-eabi-gcc 


# Our Machine Architecture is cortex-m4
MACH=cortex-m4 


# Let us define a flag for the Compiler
# -c --> Stops at the compiler stage, and doesn't do linking
# -mcpu --> which cpu architecture is this code being made for
# -mthumb --> used to specify thumb mode 
# -std=gnu11 --> using the GNU 11 Language Standard 
# -Wall --> Shows all warnings
# -O0 --> No optimization is needed 
CFLAGS = -c -mcpu=$(MACH) -mthumb -std=gnu11 -Wall -O0 


# Let us define a flag for the Linker Scripts 
# -nostdlib --> No Standard Library is used
# -T --> Thumb mode
# -Map=final.map is used for memory mapping
LDFLAGS = -nostdlib -T stm32_ls.ld -Wl,-Map=final.map


# Trying to make all the object and elf files using a single command
all:main.o led.o SystemClockConfig.o stm32_startup.o final.elf


#Let us define the target and dependency
# $^ denotes dependency
# $@ denotes target
# Here, main.o is the target and main.c is the dependency
main.o:main.c
	$(CC) $(CFLAGS) -o $@ $^


# Here, led.o is the target and led.c is the dependency
led.o:led.c
	$(CC) $(CFLAGS) -o $@ $^


# Here, SystemClockConfig.o is the target and SystemClockConfig.c is the dependency
SystemClockConfig.o:SystemClockConfig.c
	$(CC) $(CFLAGS) -o $@ $^


# Here, stm32_startup.o is the target and stm32_startup.c is the dependency
stm32_startup.o:stm32_startup.c
	$(CC) $(CFLAGS) -o $@ $^


# Making of Executable and Linkable Format
final.elf: main.o led.o SystemClockConfig.o stm32_startup.o
	$(CC) $(LDFLAGS) -o $@ $^


# Removing or Cleaning all the object files, elf files and map files
clean:
	rm -rf *.o *.elf *.map


#Using Openocd with fixing the target board
load:
	openocd -f board/stm32f4discovery.cfg