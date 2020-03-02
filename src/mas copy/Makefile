## -*- Makefile -*-
##
## User: matthew
## Time: Jun 25, 2015 9:07:06 AM
## Makefile created by Oracle Solaris Studio.
##
## This file is generated automatically.
##
#### Compiler and tool definitions shared by all build targets #####
CCC = g++
CXX = g++


BASICOPTS =   -O3  -s -m64
CCFLAGS = -std=c++14
CXXFLAGS =  -DMAS_COMMAND_TOOL -O3  -pthread -std=c++14 $(EXTRA_CXXFLAGS)
CCADMIN = 
# Define the target directories.
TARGETDIR_mas=bin
all: $(TARGETDIR_mas)/mas
## Target: mas
OBJS_mas =  \
	$(TARGETDIR_mas)/main.o
USERLIBS_mas = $(SYSLIBS_mas) 
DEPLIBS_mas =  
LDLIBS_mas = $(USERLIBS_mas)
# Link or archive
$(TARGETDIR_mas)/mas: $(TARGETDIR_mas) $(OBJS_mas) $(DEPLIBS_mas)
	$(LINK.cc) $(CCFLAGS_mas) $(CPPFLAGS_mas) -o $@ $(OBJS_mas) $(LDLIBS_mas)
       
# Compile source files into .o files
$(TARGETDIR_mas)/main.o: $(TARGETDIR_mas) main.cpp
	$(COMPILE.cc) $(CCFLAGS_mas) $(CPPFLAGS_mas) -o $@ main.cpp
#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_mas)/mas \
		$(TARGETDIR_mas)/mas.o

# Create the target directory (if needed)
$(TARGETDIR_mas):
	mkdir -p $(TARGETDIR_mas)
# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux
