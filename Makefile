EXE	= TermBoard

CC	= kos32-gcc
LD	= kos32-ld
STRIP	= kos32-strip
OBJCOPY	= kos32-objcopy
FASM	= fasm
STD	= -std=gnu11
CFLAGS	= $(SYSCFLAGS) $(STD) -O2 -Wall -Wextra $(MYCFLAGS)
LDFLAGS	= $(SYSLDFLAGS) $(MYLDFLAGS)
LIBS	= $(SYSLIBS) $(MYLIBS)
MYCFLAGS	= 
MYLDFLAGS	=
MYLIBS	=

ifeq ($(OS), Windows_NT)
	TOOLCHAIN_PATH	= C:/MinGW/msys/1.0/home/autobuild/tools/win32
else
	TOOLCHAIN_PATH	= /home/autobuild/tools/win32
endif

KOLIBRIOS_REPO	= $(abspath ../kolibrios)

SDK_DIR	= $(KOLIBRIOS_REPO)/contrib/sdk
NewLib_DIR	= $(SDK_DIR)/sources/newlib
C_LAYER_DIR	= $(KOLIBRIOS_REPO)/contrib/C_Layer
SYSCFLAGS	= -fno-ident -fomit-frame-pointer -U__WIN32__ -U_Win32 -U_WIN32 -U__MINGW32__ -UWIN32 -I $(NewLib_DIR)/libc/include -I $(TOOLCHAIN_PATH)/include -I $(C_LAYER_DIR)/INCLUDE
SYSLDFLAGS	= --image-base 0 -Tapp-dynamic.lds
SYSLIBS	= -nostdlib -L $(SDK_DIR)/lib -L $(TOOLCHAIN_PATH)/lib -L $(TOOLCHAIN_PATH)/mingw32/lib -lgcc -lc.dll -ldll
MYCFLAGS	=
MYLDFLAGS	=
MYLIBS	=
MYOBJS	=

C_LAYER_OBJ	= $(C_LAYER_DIR)/ASM/loadlibini.obj
ALL_O	= src/main.o $(C_LAYER_OBJ) $(MYOBJS)

$(EXE): $(ALL_O)
	$(LD) $(LDFLAGS) -o $@ $(ALL_O) $(LIBS)
	$(STRIP) -S $@
	$(OBJCOPY) $@ -O binary

src/main.o: src/main.c
$(C_LAYER_DIR)/ASM/loadlibini.obj: $(C_LAYER_DIR)/ASM/loadlibini.asm

%.obj: %.asm
	$(FASM) $< $%

clean:
	rm -f $(ALL_O) $(EXE)
