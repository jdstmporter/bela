RM := rm -fr

# define compiler and flags

PREFIX := arm-linux-gnueabihf
GCC_VERSION := 6.3.1

GCC_ROOT := /usr/local/linaro/arm-bela-linux-gnueabihf
GCC_PATH := $(GCC_ROOT)/bin

CC := $(GCC_PATH)/$(PREFIX)-gcc
CXX := $(GCC_PATH)/$(PREFIX)-g++
LD := $(GCC_PATH)/$(PREFIX)-g++

BELA_FLAGS := -D_GNU_SOURCE -D_REENTRANT -D__COBALT__ -D__COBALT_WRAP__ \
              -DXENOMAI_SKIN_posix -DXENOMAI_MAJOR=3 -DBELA_USE_RTDM
BASE_FLAGS := -O0 -g3 -pthread \
              -Wall -fmessage-length=0 -Wpointer-arith -Wunused-result
ARM_FLAGS := -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon \
             -ftree-vectorize -ftree-vectorizer-verbose=10 -ffast-math \
             -fPIC -fpermissive
CFLAGS := $(BELA_FLAGS) $(BASE_FLAGS) $(ARM_FLAGS) 
CXXFLAGS := -std=c++14 $(CFLAGS)
LDFLAGS := -pthread -rdynamic -Wl,-unresolved-symbols=ignore-in-shared-libs

# stdlibc++ include and library paths

GCC_BASE := $(GCC_ROOT)/$(PREFIX)
GCC_INC := $(GCC_ROOT)/lib/gcc/$(PREFIX)/$(GCC_VERSION)/include
GCC_LIB := $(GCC_BASE)/lib \
           $(GCC_BASE)/libc/lib
GCC_LIBS := stdc++ pthread rt

# Bela include and library paths

BELA_ROOT := /usr/local/bela/Bela
BELA_INC := $(BELA_ROOT) \
            $(BELA_ROOT)/include \
            $(BELA_ROOT)/libraries
BELA_LIB :=
BELA_LIBS :=

# Bela SYSROOT include and library paths

SYSROOT := /usr/local/linaro/BelaSysroot
SYSROOT_INC := $(SYSROOT)/usr/include \
               $(SYSROOT)/usr/local/include \
               $(SYSROOT)/root/Bela/include
SYSROOT_LIB := $(SYSROOT)/usr/lib \
               $(SYSROOT)/usr/local/lib \
               $(SYSROOT)/root/Bela/lib \
               $(SYSROOT)/usr/lib/arm-linux-gnueabihf             
SYSROOT_LIBS := bela NE10 mathneon asound sndfile prussdrv seasocks
             
# XENOMAI  include and library paths

XENOMAI := $(SYSROOT)/usr/xenomai
XENOMAI_INC := $(XENOMAI)/include \
               $(XENOMAI)/include/cobalt
XENOMAI_LIB := $(XENOMAI)/lib
XENOMAI_LIBS := cobalt

# put it all together

INCLUDE := $(addprefix -I, $(GCC_INC) $(BELA_INC) $(SYSROOT_INC) $(XENOMAI_INC))
LIB := $(addprefix -L, $(GCC_LIB) $(BELA_LIB) $(SYSROOT_LIB) $(XENOMAI_LIB))
LIBS := $(addprefix -l, $(GCC_LIBS)  $(BELA_LIBS) $(SYSROOT_LIBS) $(XENOMAI_LIBS))

# the executable

GetObjs = $(subst .$1,.o,$(shell find . -name "*.$1"))

OBJ := $(call GetObjs,cpp) $(call GetObjs,c)
EXE := render


# rule to build executable

$(EXE):	$(OBJ)
	$(LD) $(LDFLAGS) $(LIB) $(LIBS) $^ -o $@

build: $(EXE)

# rule to just build object files

obj: $(OBJ)

# clean



clean:
	$(RM) $(OBJ)	

.PHONY: distclean
distclean: clean
	$(RM) $(EXE)

# information

.PHONY: list
list:
	$(info CXX       = $(CXX))
	$(info CXX FLAGS = $(CXXFLAGS))
	$(info CC        = $(CC))
	$(info CC FLAGS  = $(CFLAGS))
	$(info LD        = $(LD))
	$(info LD FLAGS  = $(LDFLAGS))
	$(info INCLUDE   = $(INCLUDE))
	$(info LIBPATH   = $(LIB))
	$(info LIBS      = $(LIBS))
	$(info OBJECTS   = $(OBJ))

# compiler recipes

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -o $@ -c $<
%.o: %.c
	$(CC) $(CC_FLAGS) $(INCLUDE) -o $@ -c $<
