#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)

include $(DEVKITPRO)/wut/share/wut_rules

#-------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
#-------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		?=	debug
SOURCES		:=	source
DATA		:=	data
INCLUDES	:=	include src/wiiu

#-------------------------------------------------------------------------------
# options for code generation
#-------------------------------------------------------------------------------
CFLAGS		:=	$(MACHDEP) -Ofast -flto=auto -fno-fat-lto-objects \
				-fuse-linker-plugin -pipe $(INCLUDE) -I$(TOPDIR)/$(BUILD) \
				-D__WIIU__ -D__WUT__

CXXFLAGS	:=	$(CFLAGS) -fpermissive
ASFLAGS		:=	-g $(ARCH)
LDFLAGS		:=	-g $(ARCH) $(RPXSPECS) $(CFLAGS) -Wl,-Map,$(notdir $*.map)

LIBS		:=	-lwut -lgui -lfreetype -lgd -lpng -ljpeg -lz -lmad \
				-lvorbisidec -logg -lbz2 `$(PREFIX)pkg-config --libs SDL2_mixer SDL2_ttf SDL2_image`

#-------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level
# containing include and lib
#-------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(WUT_ROOT) $(WUT_ROOT)/usr


#-------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#-------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#-------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#-------------------------------------------------------------------------------
	export LD	:=	$(CC)
#-------------------------------------------------------------------------------
else
#-------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: debug release real clean all

#-------------------------------------------------------------------------------
all: debug

real: export INCLUDE :=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
						$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
						-I$(PORTLIBS_PATH)/ppc/include/freetype2
real:
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@$(MAKE) -C $(BUILD) -f $(CURDIR)/Makefile BUILD=$(BUILD) $(MAKE_CMD)

#-------------------------------------------------------------------------------
clean:
	@rm -fr debug release $(TARGET).rpx $(TARGET).elf

#-------------------------------------------------------------------------------
debug:		MAKE_CMD	:=	debug
debug:		LIBGUIFLAGS	:=	-O0 -g
debug:		export V	:=	1
debug:		real

#-------------------------------------------------------------------------------
release:	MAKE_CMD	:=	all
release:	LIBGUIFLAGS	:=	-Ofast -flto=auto -fno-fat-lto-objects \
							-fuse-linker-plugin -s -DNDEBUG=1
release:	BUILD		:=	release
release:	real

#-------------------------------------------------------------------------------
else
.PHONY:	debug all

DEPSDIR :=	$(TOPDIR)/$(BUILD)
DEPENDS	:=	$(OFILES:.o=.d)

#-------------------------------------------------------------------------------
# main targets
#-------------------------------------------------------------------------------
all	:	$(OUTPUT).rpx

$(OUTPUT).rpx	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#-------------------------------------------------------------------------------
debug: CFLAGS	+=	-g -Wall -O0 -fno-lto -fno-use-linker-plugin
debug: CXXFLAGS	+=	-g -Wall -O0 -fno-lto -fno-use-linker-plugin
debug: LDFLAGS	+=	-g -Wall -O0 -fno-lto -fno-use-linker-plugin
debug: all

#-------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#-------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#-------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------
