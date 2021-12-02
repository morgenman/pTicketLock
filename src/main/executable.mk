# @file module.mk
#
# The subsystem (module) make include file. Adds all local .[cs] files
# to the source list (SRC) and the current directory to the BUILDDIRS list.

# GNU make appends the name of each make file it processes to the
# MAKEFILE_LIST just before the file is processed. Thus the last word
# in the list is the latest included make file (this file). Get the
# subsystem source directory name from that file name.
LOCALSOURCE := $(dir $(lastword $(MAKEFILE_LIST)))

# echo the name of the folder being processed
q := $(shell echo "$(LOCALSOURCE)" 1>&2)

# append the submodule directory to the list of include directories
# for C compiler
INCLUDES += -I $(LOCALSOURCE)

# append BUILD modified version of directory name to list of build
# directories (so the directories are made if necessary)
MYBUILD := $(patsubst $(SOURCE)/%,$(BUILD)/%,$(LOCALSOURCE))
BUILDDIRS += $(MYBUILD)

# Make a list of the names of executable file names w/folder
EXEC_SRC += $(wildcard $(LOCALSOURCE)*.cpp)
