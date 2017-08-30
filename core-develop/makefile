# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OSTYPE),cygwin)
	# CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
elseifeq ($(OSTYPE),msys)
    # CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.exe
elseifeq ($(OS),Windows_NT)
	CLEANUP = del /F /Q
	MKDIR = mkdir
	TARGET_EXTENSION=.exe
else
	# CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
endif

UNITY_ROOT=~/Unity-master
C_COMPILER=gcc

CFLAGS=-std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Werror
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition

# TODO: none of this is correct, this is copied from elswhere
TARGET_BASE1=test1
TARGET_BASE2=test2
TARGET1 = $(TARGET_BASE1)$(TARGET_EXTENSION)
TARGET2 = $(TARGET_BASE2)$(TARGET_EXTENSION)
SRC_FILES1=$(UNITY_ROOT)/src/unity.c src/main.c  tests/test_main.c
SRC_FILES2=$(UNITY_ROOT)/src/unity.c src/helpers.c tests/test_helpers.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src
SYMBOLS=-DTEST

all: clean test

test:
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb tests/test_main.c
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb tests/test_helpers.c
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES1) -o $(TARGET1)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES2) -o $(TARGET2)
	./$(TARGET1)
	./$(TARGET2)

clean:
	$(CLEANUP)


