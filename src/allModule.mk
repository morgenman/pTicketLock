# @file allModule.mk

# Executable modules will have each .cpp file linked against all objcect files
# EXCEPT for other object files generated from code in an executable module.
#   One "tool" or executable built for each .cpp file in a listed module
EXECUTABLE_MODULES := main
# To add a new executable:
#   Just put the .cpp file in one of the listed subfolders.
#   The executable.mk file will pick up the new file on the
#   next make run.
# To add a new executable MODULE:
#   Make a recursive copy of an existing executable module (to get
#   exeuctable.mk). Add the name of the subfolder created to the
#   above list.

# Modules will have every .cpp file compiled and added to the link list
# for any executable built. All header files in any module are seen by
# every compile unit.
MODULES := locks os_dependent
# To add a new file to existing module:
#   Put a .cpp (and, if necessary, a .h) file in the subfolder
#   with the module name. module.mk will pick up the new .cpp file
#   on the next run of make. The object file will be added to the
#   link list for executables.
# To add a new MODULE:
#   Recursively copy an existing module to make sure a copy of
#   module.mk is included. Add the name of the new subfolder to
#   the list of MODULES above.

# Read the module.mk and executable.mk files in modules
include $(patsubst %,$(SOURCE)/%/module.mk,$(MODULES))
include $(patsubst %,$(SOURCE)/%/executable.mk,$(EXECUTABLE_MODULES))
