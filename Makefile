# Modular Make File
# Dr. Brian C. Ladd
# laddbc@potsdam.edu

# Compiling x86 code on x86 Linux (or similar)
#   CC - the C++ compiler to use
#   ASM - the assembler to use
CC := g++
ASM := nasm

# Directory for compiled object files.
BUILD = build

# Base source directory
SOURCE = src

# CFLAGS - flags for the C compiler
#   -std=c++20 use the C++ 2020 standard
#   -O<n>      set optimization level applied; 0 means no optimization
#              optimization can change behavior of low-level code
#   -Wall      report all possible warnings
#   -Werror    treat any warning as an error and stop the compile
#   -g         include debug information in the .o and executable files
CFLAGS = -std=c++20 -O0 -Wall -Werror -g

# ASMFLAGS - flags for the NASM assembler
#   -fbin    output format flat 16-bit binary (bootloader, DOS-like)
#   -felf64  output Format 64-bit ELF object code
ASMFLAGS = -felf64

# LDFLAGS - flags for the linker
#   -nostdlib  do not link the standard run-time library to the program
#   -lstdc++fs link with the library (-l means this) stdc++-fs
#              (the c++17 standard filesystem implementation)
#   -static    link all libraries statically rather than dynamically
LDFLAGS := -lstdc++fs

# The information defined in the source directories
#   INCLUDES - directories to search for ".h" (and, for 3rd-party libs, ".hpp") include files
#   SRC - additional source directories
#   BUILDDIRS - required $(BUILD) subdirectories+

INCLUDES := -I include
SRC :=
BUILDDIRS :=
EXEC_SRC :=
# Define the important information locally in the source directories
include $(SOURCE)/allModule.mk

# SRC is constructed by the included module make files; modify it by
# replacing .s, .c, and .cpp extensions with .o Notice change to
# manipulated list in second assignment; patsubst passes non-matches
# through unchanged so first map assembly source to object files, then
# map remaining sorce (C source) to object files
OBJ := $(patsubst $(SOURCE)/%.s,$(BUILD)/%.o,$(SRC))
OBJ := $(patsubst $(SOURCE)/%.c,$(BUILD)/%.o,$(OBJ))
OBJ := $(patsubst $(SOURCE)/%.cpp,$(BUILD)/%.o,$(OBJ))

TESTOBJ := $(foreach o, $(OBJ), $(if $(findstring test, $o),$o))

OBJ := $(filter-out $(TESTOBJ), $(OBJ))

EXEC := $(addprefix $(BUILD)/, $(basename $(notdir $(EXEC_SRC))))

# Default rule; builds target and tests
all:  $(BUILD) $(EXEC)

# Stuff from the GNU Make Book to get useful characters for printing
# and building Makefile rules to be evaluated.
, := ,

define \n


endef
blank :=
space := $(blank) $(blank)
\t := $(blank)	$(blank)

define executable_rule =
$(addprefix $(BUILD)/, $(basename $(notdir $1))): $(OBJ) $(if $(findstring test, $1), $(TESTOBJ)) $(patsubst $(SOURCE)/%.cpp,$(BUILD)/%.o,$1)
	$$(CC) $$(CFLAGS) -o $$@ $$^ $$(LDFLAGS)
endef

$(foreach t, $(EXEC_SRC), $(eval $(call executable_rule, $t)))

# Rule to make the object files; two rules since there are different
# commands depending on the type of the source code (C or  assembly).
# The directory structure below $(BUILD) must match that below $(SOURCE)
$(BUILD)/%.o: $(SOURCE)/%.s
	$(ASM) $(ASMFLAGS) $(INCLUDES) $< -o $@

$(BUILD)/%.o: $(SOURCE)/%.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# So: the build directory might not exist (see clean); it, and all
# subordinate directories, must be made. Base dir depends on any subdirs
# and is build after them. $(BUILD) is made because $(BUILDDIRS) might
# be empty.
$(BUILD): $(BUILDDIRS)
	@mkdir -p $@

# mkdir -p permits making a deeper than 1 tree AND remaking an existing
# directory. So the previous rule runs fine even if this one runs first
$(BUILDDIRS):
	@mkdir -p $@

# Rule to clean files.
.PHONY:	clean
clean :
	-rm -rf $(BUILD)
