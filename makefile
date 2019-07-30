# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
# WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
#
# Released to the Public Domain
#
# Additional material for this makefile was written by:
# Peter Fleury
# Tim Henigan
# Colin O'Flynn
# Reiner Patommel
# Markus Pfaff
# Sander Pool
# Frederik Rouleau
# Carlos Lamas
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------


# MCU name
MCU = atxmega128a1
# MCU = atmega128
# MCU = at90can128
# MCU = atmega2561
# MCU = atmega644
# MCU = atmega644p
# MCU = atmega1284p

# Hardwareplatform
# HARDWARE = OpenMCP
# HARDWARE = AVRNETIO
# HARDWARE = myAVR
# HARDWARE = UPP
# HARDWARE = XPLAIN
HARDWARE = ATXM2
# HARDWARE = EtherSense

ADDON = NONE_ADDON
# ADDON = AVRNETIO_ADDON


# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the 
#     processor frequency. You can then use this symbol in your source code to 
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#     Typical values are:
#         F_CPU =  1000000
#         F_CPU =  1843200
#         F_CPU =  2000000
#         F_CPU =  3686400
#         F_CPU =  4000000
#         F_CPU =  6250000
#         F_CPU =  7372800
#         F_CPU =  8000000
#         F_CPU = 11059200
#         F_CPU = 12500000
#         F_CPU = 14745600
#         F_CPU = 16000000
#         F_CPU = 18432000
#         F_CPU = 20000000
         F_CPU = 32000000


# Output format. (can be srec, ihex, binary)
FORMAT = ihex


# Target file name (without extension).
TARGET = main


# Object files directory
#     To put object files in current directory, use a dot (.), do NOT make
#     this an empty or blank macro!
OBJDIR = .


# List C source files here. (C dependencies are automatically generated.)
SRC = $(TARGET).c
SRC += hardware/ext_int/ext_int.c system/clock/clock.c hardware/timer1/timer1.c hardware/spi/spi_core.c 
SRC += hardware/keyboard/keyboard.c hardware/keyboard/encoder_p2w1.c hardware/uart/uart_core.c 
SRC += hardware/uart/mega/uart_0.c hardware/uart/mega/uart_1.c
SRC += hardware/uart/xmega/uart_c0.c hardware/uart/xmega/uart_d0.c hardware/uart/xmega/uart_e0.c hardware/uart/xmega/uart_f0.c 
SRC += hardware/uart/xmega/uart_c1.c hardware/uart/xmega/uart_d1.c hardware/uart/xmega/uart_e1.c hardware/uart/xmega/uart_f1.c
SRC += hardware/adc/adc.c hardware/gpio/gpio_core.c hardware/gpio/gpio_out.c hardware/gpio/gpio_in.c hardware/network/enc28j60.c hardware/1wire/1wire.c hardware/1wire/DS16xxx.c
SRC += hardware/twi/twi.c hardware/twi/ds1307.c hardware/twi/lm75.c hardware/twi/hh10d.c hardware/pcint/pcint.c hardware/memory/xram.c hardware/led/led_core.c
SRC += hardware/spi/spi_0.c hardware/spi/spi_1.c hardware/spi/spi_2.c hardware/spi/xmega_spi.c hardware/dcf77/dcf77.c
SRC += hardware/vs10xx/vs10xx.c hardware/vs10xx/vs10xx_buffer.c hardware/vs10xx/vs10xx_spi.c hardware/vs10xx/vs10xx_plugin.c hardware/rfm12/rfm12.c hardware/timer0/timer0.c hardware/sd_raw/sd_raw.c hardware/cpu_freq/cpu_freq.c
SRC += hardware/lcd/hd44780.c hardware/lcd/ks0073_twi.c hardware/lcd/hd44780_twi.c hardware/lcd/dogl128w6.c hardware/led-tafel/led_tafel.c hardware/led-tafel/led_tafel-par.c hardware/lcd/lcd.c
SRC += hardware/network/zg2100/zg2100_com.c hardware/network/zg2100/zg2100_driver.c hardware/network/zg2100/zg2100_mgmt.c
SRC += hardware/heater/heater.c
SRC += system/net/icmp.c system/net/udp.c system/net/tcp.c system/net/ethernet.c system/net/arp.c system/net/ip.c
SRC += system/net/dhcpc.c system/net/ntp.c system/net/dns.c system/net/network.c system/math/math.c system/math/checksum.c
SRC += system/stdout/stdout.c system/config/eeconfig.c system/softreset/softreset.c system/buffer/fifo.c system/init.c
SRC += system/base64/base64.c system/net/twitter.c system/net/dyndns.c system/thread/thread.c
SRC += system/string/string.c system/sensor/temp.c system/nano_DB/nano_db.c system/net/endian.c system/nano_DB/logger.c system/nano_DB/highchart.c
SRC += system/filesystem/fat.c system/filesystem/partition.c system/filesystem/byteordering.c system/filesystem/filesystem.c
SRC += system/shell/shell.c system/math/crc8.c
SRC += apps/httpd/httpd2.c apps/httpd/httpd2_pharse.c apps/httpd/cgibin/cgi-bin.c apps/httpd/files.c apps/telnet/telnet.c
SRC += apps/cron/cron.c apps/apps_init.c apps/modul_init.c apps/modules/lcd_info.c apps/modules/cmd_foo.c apps/modules/cmd_mon.c apps/modules/cmd_ping.c
SRC += apps/modules/cmd_arp.c apps/modules/cmd_stats.c apps/modules/cmd_reset.c apps/modules/cmd_dns.c apps/modules/cmd_adc.c apps/modules/cmd_iwconfig.c
SRC += apps/modules/cmd_gpio.c apps/modules/cmd_ifconfig.c apps/modules/cmd_ntp.c apps/modules/cmd_dyndns.c apps/modules/cmd_temp.c
SRC += apps/modules/cmd_cron.c apps/modules/cmd_twitter.c apps/modules/cmd_eemem.c apps/modules/cmd_twi.c apps/modules/cmd_onewire.c
SRC += apps/modules/cmd_stream.c apps/modules/cmd_tafel.c apps/modules/impulse.c apps/modules/temp_json.c apps/modules/cmd_heater.c
SRC += apps/mp3-streamingclient/mp3-streaming.c apps/mp3-streamingclient/mp3-clientserver.c apps/mp3-streamingclient/playlist.c
SRC += apps/can/can_eth_rep.c apps/mp3-streamingclient/udp-stream.c apps/mp3-streamingclient/streaminfo.c
SRC += apps/tftp-server/tftp-server.c
SRC += apps/mp3-streamingclient/menu/menu-interpreter.c apps/mp3-streamingclient/menu/menu-text8x16.c

# Add user C source files here.
SRC +=


# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC = 


# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC =

#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s


# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2


# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS = 


# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99


# Place -D or -U options here for C sources
CDEFS = -DF_CPU=$(F_CPU)UL


# Place -D or -U options here for ASM sources
ADEFS = -DF_CPU=$(F_CPU)


# Place -D or -U options here for C++ sources
CPPDEFS = -DF_CPU=$(F_CPU)UL
#CPPDEFS += -D__STDC_LIMIT_MACROS
#CPPDEFS += -D__STDC_CONSTANT_MACROS



#---------------- Compiler Options C ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS)
CFLAGS += -O$(OPT)
CFLAGS += -fno-common
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wall -Wshadow
CFLAGS += -Wstrict-prototypes
#CFLAGS += -fno-unit-at-a-time
#CFLAGS += -Wundef
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wsign-compare
CFLAGS += -Wa,-adhlns=$(<:%.c=$(OBJDIR)/%.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)

# Special settings by OpenMCP
CFLAGS += -D__heap_end=0x80ffff -D$(HARDWARE) -D$(ADDON)
CFLAGS += -mcall-prologues
CFLAGS += -fdata-sections
CFLAGS += -ffunction-sections
CFLAGS += -fmerge-constants
# CFLAGS += -mno-tablejump
CFLAGS += -ffreestanding
CFLAGS += -fno-move-loop-invariants
# CFLAGS += --param inline-call-cost=2
CFLAGS += -finline-limit=10
# Some settings to make smaller code with avr-gcc 4.3.x
# CFLAGS += -fno-inline-small-functions
# CFLAGS += -fno-split-wide-types
# CFLAGS += -Wl,--relax

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CPPFLAGS = -g$(DEBUG)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -O$(OPT)
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fpack-struct
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wall
CPPFLAGS += -Wundef
#CPPFLAGS += -mshort-calls
#CPPFLAGS += -fno-unit-at-a-time
#CPPFLAGS += -Wstrict-prototypes
#CPPFLAGS += -Wunreachable-code
#CPPFLAGS += -Wsign-compare
CPPFLAGS += -Wa,-adhlns=$(<:%.cpp=$(OBJDIR)/%.lst)
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
#CPPFLAGS += $(CSTANDARD)


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -adhlns:   create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex 
#       dump that will be displayed for a given single line of source input.
ASFLAGS = $(ADEFS) -Wa,-adhlns=$(<:%.S=$(OBJDIR)/%.lst),-gstabs,--listing-cont-lines=100


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


MATH_LIB = -lm


# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS =

EXTRALIB =



#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

# Special settings for OpenMCP and XPLAIN
# The OpenMCP platform has external RAM, used for heap and variables
ifeq ($(HARDWARE),OpenMCP)
    EXTMEMOPTS = -Wl,--section-start,.data=0x802200,--defsym=__heap_end=0x80ffff
endif
ifeq ($(HARDWARE),XPLAIN)
    EXTMEMOPTS = -Wl,--section-start,.data=0x804000,--defsym=__heap_end=0x80ffff
endif
ifeq ($(HARDWARE),ATXM2)
    EXTMEMOPTS = -Wl,--section-start,.data=0x804000,--defsym=__heap_end=0x80ffff
endif

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref,--gc-sections
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += -Wl,--relax
# LDFLAGS += -mrelax
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
LDFLAGS += $(EXTRALIB)
# LDFLAGS += -T linker_script.x



#---------------- Programming Options (avrdude) ----------------

# Programming hardware
# Type: avrdude -c ?
# to get a full listing.
#
# AVRDUDE_PROGRAMMER = stk500v2
AVRDUDE_PROGRAMMER = avrispmkII
# AVRDUDE_PROGRAMMER = avr911
# AVRDUDE_PROGRAMMER = jtag2
# AVRDUDE_PROGRAMMER = dragon_isp

# com1 = serial port. Use lpt1 to connect to parallel port.
# AVRDUDE_PORT = com1 # programmer connected to serial device
# AVRDUDE_PORT = com3
AVRDUDE_PORT = usb # Device discovery for avrispmkII
# AVRDUDE_PORT = /dev/ttyUSB0 # programmer connected to USB on Unix systems

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

# Fuse settings
AVRDUDE_FUSE = -i 15    # Slow down the writing

ifeq ($(MCU),at90can128)
AVRDUDE_LFUSE = 0xFF
# Enable SPI
AVRDUDE_HFUSE = 0xDF
# Highest brown out detection
AVRDUDE_EFUSE = 0xFD
endif

ifeq ($(MCU),atmega2561)
AVRDUDE_LFUSE = 0xe6
AVRDUDE_HFUSE = 0xd1
# enable JTAG
#AVRDUDE_HFUSE = 0x91
AVRDUDE_EFUSE = 0xfd
endif

ifeq ($(MCU),atmega644)
AVRDUDE_LFUSE = 0xe6
AVRDUDE_HFUSE = 0xd1
AVRDUDE_EFUSE = 0xfd
endif

ifeq ($(MCU),atmega644p)
AVRDUDE_LFUSE = 0xe6
AVRDUDE_HFUSE = 0xd1
AVRDUDE_EFUSE = 0xfd
endif

ifeq ($(MCU),atmega1284p)
AVRDUDE_LFUSE = 0xe6
AVRDUDE_HFUSE = 0xd1
AVRDUDE_EFUSE = 0xfd
endif

ifeq ($(HARDWARE),EtherSense)
AVRDUDE_LFUSE = 0xe0
endif

# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)

# Commands for fuse writing
AVRDUDE_WRITE_EFUSE = -U efuse:w:$(AVRDUDE_EFUSE):m
AVRDUDE_WRITE_HFUSE = -U hfuse:w:$(AVRDUDE_HFUSE):m
AVRDUDE_WRITE_LFUSE = -U lfuse:w:$(AVRDUDE_LFUSE):m


#---------------- Debugging Options ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)

# Set the DEBUG_UI to either gdb or insight.
DEBUG_UI = gdb
# DEBUG_UI = insight

# Set the debugging back-end to either avarice, simulavr.
#DEBUG_BACKEND = avarice
DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when 
#     avarice is running on a different computer.
DEBUG_HOST = localhost



#============================================================================


# Define programs and commands.
SHELL = sh
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp
WINSHELL = cmd

# The following lets define you alternate locations for your avr files.
# Useful if you use multiple versions of gcc, avrlibc and such.

# Leave empty if you have them in your path
#DIRAVRPREFIX =
#DIRAVRBIN =

# DIRAVRPREFIX = /usr/local/avr/
# DIRAVRBIN = $(DIRAVRPREFIX)bin/

CC = $(DIRAVRBIN)avr-gcc
# CC = $(DIRAVRBIN)avr-gcc-4.3.4
# CC = $(DIRAVRBIN)avr-gcc-4.2.4
OBJCOPY = $(DIRAVRBIN)avr-objcopy
OBJDUMP = $(DIRAVRBIN)avr-objdump
SIZE = $(DIRAVRBIN)avr-size
AR = $(DIRAVRBIN)avr-ar rcs
NM = $(DIRAVRBIN)avr-nm
AVRDUDE = avrdude


# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:




# Define all object files.
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) $(CPPSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.S=$(OBJDIR)/%.o) 

# Define all listing files.
LST = $(SRC:%.c=$(OBJDIR)/%.lst) $(CPPSRC:%.cpp=$(OBJDIR)/%.lst) $(ASRC:%.S=$(OBJDIR)/%.lst) 


# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -I. -x c++ $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)



# Default target.
all: begin gccversion sizebefore build sizeafter end

# Change the build target to build a HEX file or a library.
build: elf hex eep lss sym
#build: lib


elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym
LIBNAME=lib$(TARGET).a
lib: $(LIBNAME)



# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo
	@if test ! -f "config.h"; then echo "empty" > config.h; fi
	@if ! cmp $(HARDWARE).config.h config.h; then cp $(HARDWARE).config.h config.h ;fi
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) $(TARGET).elf

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
	2>/dev/null; echo; fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
	2>/dev/null; echo; fi



# Display compiler version information.
gccversion : 
	@$(CC) --version

# Program the device.  
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)
#	sleep 3
#	avarice -2 -D -j com2 -x

# Write the fuses
fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_FUSE) $(AVRDUDE_WRITE_EFUSE) $(AVRDUDE_WRITE_HFUSE) $(AVRDUDE_WRITE_LFUSE)

# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set 
#     a breakpoint at main().
gdb-config: 
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif
	@echo break main >> $(GDBINIT_FILE)

debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
	$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause

else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT = $(OBJCOPY) --debugging
COFFCONVERT += --change-section-address .data-0x800000
COFFCONVERT += --change-section-address .bss-0x800000
COFFCONVERT += --change-section-address .noinit-0x800000
COFFCONVERT += --change-section-address .eeprom-0x810000



coff: $(TARGET).elf
	@echo
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof



# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@



# Create library from object files.
.SECONDARY : $(TARGET).a
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
	@echo
	@echo $(MSG_CREATING_LIBRARY) $@
	$(AR) $@ $(OBJ)


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp
	@echo
	@echo $(MSG_COMPILING_CPP) $<
	$(CC) -c $(ALL_CPPFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C++ source files.
%.s : %.cpp
	$(CC) -S $(ALL_CPPFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@ 


# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(SRC:%.c=$(OBJDIR)/%.o)
	$(REMOVE) $(SRC:%.c=$(OBJDIR)/%.lst)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(SRC:.c=.i)
	$(REMOVEDIR) .dep


# Create object files directory
$(shell mkdir $(OBJDIR) 2>/dev/null)


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program debug gdb-config
