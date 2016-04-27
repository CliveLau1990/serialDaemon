###############################################################################
#
# Copyright (C) 2016 CliveLiu
# Subject to the GNU Public License, version 2.
#
# Created By: Clive Liu <ftdstudio1990@gmail.com>
# Created Date:	2016-03-07
#
# ChangeList:
# Created in 2016-03-07 by Clive;
#
# Generic Makefile for C/C++ Program
#
# Description:
# The makefile searches in <SRCDIRS> directories for the source files
# with extensions specified in <SOURCE_EXT>, then compiles the sources
# and finally produces the <PROGRAM>, the executable file, by linking
# the objectives.
# Usage:
#   $ make           compile and link the program.
#   $ make objs      compile only (no linking. Rarely used).
#   $ make clean     clean the objectives and dependencies.
#   $ make distclean clean the objectives, dependencies and executable.
#   $ make rebuild   rebuild the program. The same as make clean && make all.
#==============================================================================
## Customizing Section: adjust the following if necessary.
##=============================================================================
# The executable file name.
# It must be specified.
# PROGRAM   := a.out    # the executable name
PROGRAM   := serialDaemon
# The directories in which source files reside.
# At least one path should be specified.
# SRCDIRS   := .        # current directory
SRCDIRS   := .
# The source file types (headers excluded).
# At least one type should be specified.
# The valid suffixes are among of .c, .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx.
# SRCEXTS   := .c      # C program
# SRCEXTS   := .cpp    # C++ program
# SRCEXTS   := .c .cpp # C/C++ program
SRCEXTS   := .c .cpp
# The include directry
INCDIRS    := -I$(SRCDIRS)/arm-opencv-2.4.10/include
# The flags used by the cpp (man cpp for more).
# CPPFLAGS  := -Wall -Werror # show all warnings and take them as errors
CPPFLAGS  :=
CPPFLAGS  +=
# The compiling flags used only for C.
# If it is a C++ program, no need to set these flags.
# If it is a C and C++ merging program, set these flags for the C parts.
CFLAGS    :=
CFLAGS    +=
# The compiling flags used only for C++.
# If it is a C program, no need to set these flags.
# If it is a C and C++ merging program, set these flags for the C++ parts.
CXXFLAGS  :=
CXXFLAGS  +=
# The library and the link options ( C and C++ common).
LDDIRS    := -L$(SRCDIRS)/arm-opencv-2.4.10/lib
LDFLAGS   := -lpthread -lm
LDFLAGS   += -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab
## Implict Section: change the following only when necessary.
##=============================================================================
# The Cross-compiler
CROSS_COMPILE ?=
# The C program compiler. Uncomment it to specify yours explicitly.
CC      = $(CROSS_COMPILE)gcc
# The C++ program compiler. Uncomment it to specify yours explicitly.
CXX     = $(CROSS_COMPILE)g++
# Uncomment the 2 lines to compile C programs as C++ ones.
#CC      = $(CXX)
#CFLAGS  = $(CXXFLAGS)
# The command used to delete file.
#RM        = rm -f
## Stable Section: usually no need to be changed. But you can add more.
##=============================================================================
SHELL   = /bin/bash
SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS    = $(foreach x,$(SRCEXTS), $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))
DEPS    = $(patsubst %.o,%.d,$(OBJS))
.PHONY : all objs clean distclean rebuild
all : $(PROGRAM)
# Rules for creating the dependency files (.d).
#---------------------------------------------------
%.d : %.c
	@$(CC) -MM -MD $(CFLAGS) $(INCDIRS) $<
%.d : %.C
	@$(CC) -MM -MD $(CXXFLAGS) $(INCDIRS) $<
%.d : %.cc
	@$(CC) -MM -MD $(CXXFLAGS) $(INCDIRS) $<
%.d : %.cpp
	@$(CC) -MM -MD $(CXXFLAGS) $(INCDIRS) $<
%.d : %.CPP
	@$(CC) -MM -MD $(CXXFLAGS) $(INCDIRS) $<
%.d : %.c++
	@$(CC) -MM -MD $(CXXFLAGS) $(INCDIRS) $<
%.d : %.cp
	@$(CC) -MM -MD $(CXXFLAGS) $(INCDIRS) $<
%.d : %.cxx
	@$(CC) -MM -MD $(CXXFLAGS) $(INCDIRS) $<
# Rules for producing the objects.
#---------------------------------------------------
objs : $(OBJS)
%.o : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(INCDIRS) $<
%.o : %.C
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIRS) $<
%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIRS) $<
%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIRS) $<
%.o : %.CPP
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIRS) $<
%.o : %.c++
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIRS) $<
%.o : %.cp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIRS) $<
%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIRS) $<
# Rules for producing the executable.
#----------------------------------------------
$(PROGRAM) : $(OBJS)
ifeq ($(strip $(SRCEXTS)), .c)  # C file
	$(CC) -o $(PROGRAM) $(LDDIRS) $(OBJS) $(LDFLAGS)
else                            # C++ file
	$(CXX) -o $(PROGRAM) $(LDDIRS) $(OBJS) $(LDFLAGS)
endif
-include $(DEPS)
rebuild: clean all
clean :
	@$(RM) *.o *.d
distclean: clean
	@$(RM) $(PROGRAM) $(PROGRAM).exe
