# mkxp-nx - Hito 1 (devkitPro / libnx)
ifeq ($(strip $(DEVKITPRO)),)
$(error "Define DEVKITPRO (en el contenedor de GitHub Actions ya viene definido)")
endif

export PATH := $(DEVKITPRO)/tools/bin:$(DEVKITPRO)/devkitA64/bin:$(PATH)

TARGET      := mkxp-nx
APP_TITLE   := mkxp-nx (hito 1)
APP_AUTHOR  := mkxp-nx
APP_VERSION := 0.1.0

PORTLIBS := $(DEVKITPRO)/portlibs/switch
LIBNX    := $(DEVKITPRO)/libnx
CC       := aarch64-none-elf-gcc

ARCH    := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
CFLAGS  := -g -Wall -O2 -ffunction-sections $(ARCH) -D__SWITCH__ \
           -I$(LIBNX)/include -I$(PORTLIBS)/include \
           $(shell $(PORTLIBS)/bin/sdl2-config --cflags)
LDFLAGS := -specs=$(LIBNX)/switch.specs -g $(ARCH)
LIBS    := $(shell $(PORTLIBS)/bin/sdl2-config --libs) \
           -lEGL -lglapi -ldrm_nouveau \
           -L$(PORTLIBS)/lib -L$(LIBNX)/lib -lnx -lm

.PHONY: all clean

all: $(TARGET).nro

$(TARGET).elf: main.c
	$(CC) $(CFLAGS) main.c $(LDFLAGS) $(LIBS) -o $@

$(TARGET).nacp:
	nacptool --create "$(APP_TITLE)" "$(APP_AUTHOR)" "$(APP_VERSION)" $@

$(TARGET).nro: $(TARGET).elf $(TARGET).nacp
	elf2nro $(TARGET).elf $@ --nacp=$(TARGET).nacp

clean:
	rm -f $(TARGET).elf $(TARGET).nro $(TARGET).nacp
