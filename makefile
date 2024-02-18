# makefile for MCQ Version=28

################################################################################
# Starting from OS017S00B07 the OSQ file delivers its own makefile which is
# extracted to SYS/make/makefile and used by GSe-VISU. If you use an OS
# which delivers a makefile this file is no longer used
################################################################################

#Check if the OS delivers a makefile and if so error out
ifneq (,$(wildcard ./SYS/make/makefile))
    $(warning The OS you are using includes a makefile in ./SYS/make.)
    $(warning Please use that when compiling outside GSe-VISU)
    $(error )
endif

#This file has to be generated by C Editor
include srcList.mk

#Optional user suplied makefile
-include user.mk

#output directories
BUILDDIR = .build
DEPDIR = .dep

GDSPATH := $(subst \,/,$(GDSPATH))

export PATH := $(PATH);$(GDSPATH)/bin/compiler/mingw/bin

#Path to compiler/linker/tools/...
ECHO := $(GDSPATH)/bin/gnu/echo.exe
MKDIR := $(GDSPATH)/bin/gnu/mkdir
RM := $(GDSPATH)/bin/gnu/rm
CP := $(GDSPATH)/bin/gnu/cp 

#WIN32 paths
CCW32 = $(GDSPATH)/bin/compiler/mingw/bin/gcc.exe

ifeq (armv7, $(ARCH))
    GCC_PREFIX := arm-gs-linux-gnueabihf

    CC = $(GDSPATH)/bin/compiler/$(GCC_PREFIX)/bin/$(GCC_PREFIX)-gcc
    LD = $(CC)
    OBJCOPY = $(GDSPATH)/bin/compiler/$(GCC_PREFIX)/bin/$(GCC_PREFIX)-objcopy
    STRIP = $(GDSPATH)/bin/compiler/$(GCC_PREFIX)/bin/$(GCC_PREFIX)-strip

    LIBS_LIST_TGT := $(addprefix -l, $(addsuffix _armv7, $(LIBS)))
    #additional library directories
    LIBDIRS += ./SYS

    LDFLAGS += -Wl,--cref,-Map,$(BUILDDIR)/$(TARGET).map 
    LDFLAGS += -shared $(patsubst %,-L%,$(LIBDIRS))
    LDFLAGS += -Wl,--warn-common
    LDFLAGS += -Wl,--start-group $(LIBS_LIST_TGT) -Wl,--end-group
    LDFLAGS += -Wl,-soname,$@
else
    GCC_PREFIX := arm-elf

    CC = $(GDSPATH)/bin/compiler/arm-gcc/bin/$(GCC_PREFIX)-gcc
    LD = $(GDSPATH)/bin/compiler/arm-gcc/bin/$(GCC_PREFIX)-ld
    OBJCOPY = $(GDSPATH)/bin/compiler/arm-gcc/bin/$(GCC_PREFIX)-objcopy
    STRIP = $(GDSPATH)/bin/compiler/arm-gcc/bin/$(GCC_PREFIX)-strip
    
    CFLAGS += -mcpu=arm926ej-s -msoft-float -mfpu=vfp

    LIBS_LIST_TGT = $(addprefix -l, $(LIBS) gcc)
    #additional library directories
    LIBDIRS += ./SYS $(GDSPATH)/bin/compiler/arm-gcc/lib

    LDFLAGS += -soname $@
    LDFLAGS += --cref 
    LDFLAGS += -Map $(BUILDDIR)/$(TARGET).map 
    LDFLAGS += -shared 
    LDFLAGS += $(patsubst %,-L%,$(LIBDIRS))
    LDFLAGS += --warn-common
    LDFLAGS += --start-group $(LIBS_LIST_TGT) --end-group
endif

#additional include directories
INCDIRS += ./SYS ./cgen $(subst \,/,$(sort $(dir $(SRC))))

#flags passed to the C compiler
CFLAGS += -O2 -fpic -fvisibility=hidden -fpic -std=c99 -nostdinc
CFLAGS += -DGDSPROJECT
CFLAGS += -DCURL_DISABLE_TYPECHECK
CFLAGS += -DCURL_STATICLIB
CFLAGS += -pedantic -Wall -Wextra -Wno-long-long
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Winit-self
CFLAGS += -Wfloat-equal
CFLAGS += -Wshadow
CFLAGS += -Wcast-qual
CFLAGS += -Wcast-align
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wold-style-definition
CFLAGS += -Wno-unused-parameter
CFLAGS += $(patsubst %,-I%,$(INCDIRS)) -I.

#flags passed to the WIN32 C compiler
CFLAGSW32  = -g -std=c99 -nostdinc
CFLAGSW32 += -DGDSPROJECT
CFLAGSW32 += -DCURL_DISABLE_TYPECHECK
CFLAGSW32 += -DCURL_STATICLIB
CFLAGSW32 += -pedantic -Wall -Wextra -Wno-long-long
CFLAGSW32 += -Werror-implicit-function-declaration
CFLAGSW32 += -Winit-self
CFLAGSW32 += -Wfloat-equal
CFLAGSW32 += -Wshadow
CFLAGSW32 += -Wcast-qual
CFLAGSW32 += -Wcast-align
CFLAGSW32 += -Wstrict-prototypes
CFLAGSW32 += -Wold-style-definition
CFLAGSW32 += -Wno-unused-parameter
CFLAGSW32 += $(patsubst %,-I%,$(INCDIRS)) -I.

#flags for GCC (dependency generation)
DEPFLAGS  = $(patsubst %,-I%,$(INCDIRS))
DEPFLAGS += -MM -MP -MG -MT $@ -MF .dep/$(notdir $@).d

#flags passed to the linker
LDFLAGSW32 = -Wl,--export-all-symbols
LDFLAGSW32 += -Wl,--warn-common

SRC_C = $(notdir $(filter %.c,$(SRC)))

# Define all object files.
OBJ = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SRC_C))))
OBJW32 = $(addprefix $(BUILDDIR)/, $(addsuffix .ow32, $(basename $(SRC_C))))

ADDOBJW32 := $(filter %.ow32,$(ADDOBJ))
ADDOBJ := $(filter %.o,$(ADDOBJ))

LIBS_LIST_W32 = $(addprefix -l, $(addsuffix _win32, $(LIBS)) stdc++.dll pthread.dll ws2_32 iphlpapi)

vpath %.c $(subst \,/,$(dir $(filter %.c,$(SRC))))

# Default target.
ifeq ($(BUILDWIN32), 1)
all: begin so dll sobin dllbin end
else
all: begin so sobin end
endif

begin:
	@$(ECHO) "### Building UserC for $(TARGET) ###"
	$(shell $(MKDIR) .dep 2>NUL)
	$(shell $(MKDIR) $(BUILDDIR) 2>NUL)

ifeq ($(BUILDWIN32), 1)
end: | sobin dllbin
else
end: | sobin
endif
	@$(ECHO) "### Done ###"

so: $(BUILDDIR)/$(TARGET).so | begin
dll: $(BUILDDIR)/$(TARGET).dll | begin

sobin: so | begin
	@$(ECHO) CV $(TARGET).so 
	@$(CP) $(BUILDDIR)/$(TARGET).so USERPROG.BIN
	@$(STRIP) --strip-unneeded USERPROG.BIN

dllbin: dll | begin
	@$(ECHO) CV $(TARGET).dll
	@$(CP) $(BUILDDIR)/$(TARGET).dll USERPROG.WIN32.BIN

# Link: create ELF output file from object files.
$(BUILDDIR)/%.so: $(OBJ)
	@$(ECHO) LD $(notdir $^)
	@$(LD) $^ $(ADDOBJ) $(LDFLAGS) -o $@ 

$(BUILDDIR)/%.dll: $(OBJW32)
	@$(ECHO) LDW32 $(notdir $^)
	@$(CCW32) -g -shared $(LDFLAGSW32) -o$@ -L ./sys $(OBJW32) $(ADDOBJW32) -Wl,--start-group $(LIBS_LIST_W32) -Wl,--end-group

# Compile: create object files from C source files.
$(BUILDDIR)/%.o : %.c
	@$(ECHO) CC $(notdir $<)
	@$(CC) -c $(CFLAGS) -o $@ $<
	@$(CC) $(DEPFLAGS) $<
    
# Compile: create WIN32 object files from C source files.
$(BUILDDIR)/%.ow32 : %.c
	@$(ECHO) CCW32 $(notdir $<)
	@$(CCW32) -c $(CFLAGSW32) -o $@ $<

# Target: clean project.
clean: 
	@$(ECHO) "### Removing build files ###"
	@$(RM) -rf $(DEPDIR)
	@$(RM) -rf $(BUILDDIR)
	@$(ECHO) "### Done ###"

# Include the dependency files.
-include $(wildcard $(DEPDIR)/*)
    
# Listing of phony targets.
.PHONY : all begin clean end
.PRECIOUS : $(OBJ) $(OBJW32)