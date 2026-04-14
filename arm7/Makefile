# ARM7 binary for Chocolate Doom DS
# Built with BlocksDS. Enables MaxMod for sound effect playback.

export BLOCKSDS		?= /opt/blocksds/core
export BLOCKSDSEXT	?= /opt/blocksds/external
export WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

SOURCEDIRS	:= source
INCLUDEDIRS	:=

DEFINES		:= -DUSE_MAXMOD

LIBS		:= -lmm7 -lnds7 -lc
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod \
		   $(BLOCKSDS)/libs/libnds

NAME		:= arm7
BUILDDIR	:= build/$(NAME)
ELF		:= build/$(NAME).elf
MAP		:= build/$(NAME).map

PREFIX		:= $(ARM_NONE_EABI_PATH)arm-none-eabi-
CC		:= $(PREFIX)gcc
LD		:= $(PREFIX)gcc
MKDIR		:= mkdir
RM		:= rm -rf

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

SOURCES_C	:= $(shell find -L $(SOURCEDIRS) -name "*.c")

ARCH		:= -mthumb -mcpu=arm7tdmi
SPECS		:= $(BLOCKSDS)/sys/crts/ds_arm7.specs
WARNFLAGS	:= -Wall -Wextra -Wpedantic -Wshadow

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include)

LIBDIRSFLAGS	:= $(foreach path,$(LIBDIRS),-L$(path)/lib)

CFLAGS		:= -std=gnu17 $(WARNFLAGS) $(INCLUDEFLAGS) $(DEFINES) \
		   $(ARCH) -O2 -g -ffunction-sections -fdata-sections \
		   -specs=$(SPECS)

LDFLAGS		:= $(ARCH) $(LIBDIRSFLAGS) -Wl,-Map,$(MAP) $(DEFINES) \
		   -Wl,--start-group $(LIBS) -Wl,--end-group -specs=$(SPECS)

OBJS		:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C)))
DEPS		:= $(OBJS:.o=.d)

.PHONY: all clean

all: $(ELF)

$(ELF): $(OBJS)
	@echo "  LD.7    $@"
	$(V)$(LD) -o $@ $(OBJS) $(LDFLAGS)

clean:
	@echo "  CLEAN.7"
	$(V)$(RM) $(ELF) $(MAP) $(BUILDDIR)

$(BUILDDIR)/%.c.o : %.c
	@echo "  CC.7    $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

-include $(DEPS)
