# Makefile for brwatch - MSYS2 mingw32 version
# Compatible with MSYS2 mingw32 toolchain
# Usage: mingw32-make -f Makefile clean all
#    or: make -f Makefile clean all

# MSYS2 mingw32 toolchain prefix
# Set MSYS2_MINGW32 to your MSYS2 installation root, e.g.:
#   make -f Makefile MSYS2_MINGW32=C:/msys64
MSYS2_MINGW32 ?= C:/msys64
TOOLCHAIN     = $(MSYS2_MINGW32)/mingw32/bin/

CC       = $(TOOLCHAIN)gcc
CPP      = $(TOOLCHAIN)g++
WINDRES  = $(TOOLCHAIN)windres
RM       = del /Q
RMDIR    = rmdir /S /Q
MKDIR_P  = mkdir

# Output
BIN      = build/brwatch.exe

# Directories
SRC_DIR  = src
INC_DIR  = include
LIB_DIR  = libraries
BUILD_DIR = build

# Compiler flags
CFLAGS   = -m32 -Wall -Wextra -I$(INC_DIR)
LDFLAGS  = -m32 -mwindows -static-libgcc
LIBS     = -lws2_32 -liphlpapi -lnetapi32 -lcomdlg32 -lgdi32 -luser32 -lkernel32 -lcomctl32 -L$(LIB_DIR) $(LIB_DIR)/PviCom.lib

# Resource
RES      = $(BUILD_DIR)/brwatch_private.o

# Source files
C_SOURCES = \
	cpusearch.c \
	dlg_about.c \
	dlg_busy.c \
	dlg_editcpu.c \
	dlg_edittask.c \
	dlg_settings.c \
	dlg_showpviobjects.c \
	dlg_writepvar.c \
	logger.c \
	main.c \
	mylistview.c \
	mytreeview.c \
	pvi_interface.c \
	resource.c \
	settings.c \
	stringtools.c \
	compressfile.c

# zlib sources
ZLIB_SOURCES = \
	adler32.c \
	compress.c \
	crc32.c \
	deflate.c \
	gzclose.c \
	gzlib.c \
	gzread.c \
	gzwrite.c \
	infback.c \
	inffast.c \
	inflate.c \
	inftrees.c \
	trees.c \
	uncompr.c \
	zutil.c

# Object files
C_OBJS   = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ZLIB_OBJS = $(patsubst %.c,$(BUILD_DIR)/zlib/%.o,$(ZLIB_SOURCES))
OBJS     = $(C_OBJS) $(ZLIB_OBJS)

# Rules
.PHONY: all clean help

all: setup $(BIN)

help:
	@echo brwatch Makefile (MSYS2 mingw32)
	@echo.
	@echo Usage:
	@echo   make -f Makefile clean all    - Clean and build
	@echo   make -f Makefile all          - Build only
	@echo   make -f Makefile clean        - Clean build files
	@echo   make -f Makefile help         - Show this help
	@echo.

setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(BUILD_DIR)\zlib" mkdir "$(BUILD_DIR)\zlib"

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)" 2>nul || echo.

$(RES): resources/main.rc
	$(WINDRES) -i resources/main.rc -o $(RES) --output-format=coff

# Main executable
$(BIN): setup $(OBJS) $(RES)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(RES) $(LIBS)
	@echo.
	@echo Build successful: $(BIN)
	@echo.

# Pattern rule for object files from src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Pattern rule for object files from src/zlib/
$(BUILD_DIR)/zlib/%.o: $(SRC_DIR)/zlib/%.c
	$(CC) $(CFLAGS) -c $< -o $@
