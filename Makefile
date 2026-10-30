# Makefile for brwatch - MSYS2 mingw64 version
# Compatible with MSYS2 mingw64 toolchain
# Usage: mingw32-make -f Makefile clean all
#    or: make -f Makefile clean all

# MSYS2 mingw64 toolchain prefix
# Set MSYS2_MINGW64 to your MSYS2 installation root, e.g.:
#   make -f Makefile MSYS2_MINGW64=C:/msys64
MSYS2_MINGW64 ?= C:/msys64
TOOLCHAIN     = $(MSYS2_MINGW64)/mingw64/bin/

CC       = $(TOOLCHAIN)gcc
CXX      = $(TOOLCHAIN)g++
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
CFLAGS   = -m64 -Wall -Wextra -I$(INC_DIR)
CXXFLAGS = $(CFLAGS) -std=c++14
LDFLAGS  = -m64 -mwindows -static-libgcc -static-libstdc++
LIBS     = -lws2_32 -liphlpapi -lnetapi32 -lcomdlg32 -lgdi32 -luser32 -lkernel32 -lcomctl32 -lshell32 -L$(LIB_DIR) $(LIB_DIR)/PviCom64.lib

# Resource
RES      = $(BUILD_DIR)/brwatch_private.o

# Source files
CPP_SOURCES = \
	cpusearch.cpp \
	dlg_about.cpp \
	dlg_busy.cpp \
	dlg_editcpu.cpp \
	dlg_edittask.cpp \
	dlg_settings.cpp \
	dlg_showpviobjects.cpp \
	dlg_writepvar.cpp \
	logger.cpp \
	main.cpp \
	mylistview.cpp \
	object_treeview.cpp \
	pvi_interface.cpp \
	resource.cpp \
	settings.cpp \
	stringtools.cpp \
	compressfile.cpp

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
APP_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPP_SOURCES))
ZLIB_OBJS = $(patsubst %.c,$(BUILD_DIR)/zlib/%.o,$(ZLIB_SOURCES))
OBJS     = $(APP_OBJS) $(ZLIB_OBJS)

# Rules
.PHONY: all clean help test FORCE

all: setup $(BIN)

test: setup $(BUILD_DIR)/pvi_object_test.exe
	$(BUILD_DIR)\pvi_object_test.exe

$(BUILD_DIR)/pvi_object_test.exe: tests/pvi_object_test.cpp $(OBJS) $(RES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -mconsole -o $@ $< $(OBJS) $(RES) $(LIBS)

help:
	@echo brwatch Makefile (MSYS2 mingw64)
	@echo.
	@echo Usage:
	@echo   make -f Makefile clean all    - Clean and build
	@echo   make -f Makefile all          - Build only
	@echo   make -f Makefile clean        - Clean build files
	@echo   make -f Makefile help         - Show this help
	@echo   make -f Makefile test         - Run PviObject regression tests
	@echo.

setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(BUILD_DIR)\zlib" mkdir "$(BUILD_DIR)\zlib"

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)" 2>nul || echo.

$(RES): resources/main.rc resources/brwatch.manifest $(INC_DIR)/resource.h
	$(WINDRES) -i resources/main.rc -o $(RES) --output-format=coff

FORCE:

$(BUILD_DIR)/git_info.h: FORCE | setup
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File build_git_info.ps1 -OutputPath "$(BUILD_DIR)/git_info.h"

$(BUILD_DIR)/dlg_about.o: $(SRC_DIR)/dlg_about.cpp $(BUILD_DIR)/git_info.h
	$(CXX) $(CXXFLAGS) -I$(BUILD_DIR) -c $< -o $@

# Main executable
$(BIN): setup $(OBJS) $(RES)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJS) $(RES) $(LIBS)
	@echo.
	@echo Build successful: $(BIN)
	@echo.

# Pattern rule for object files from src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for object files from src/zlib/
$(BUILD_DIR)/zlib/%.o: $(SRC_DIR)/zlib/%.c
	$(CC) $(CFLAGS) -c $< -o $@
