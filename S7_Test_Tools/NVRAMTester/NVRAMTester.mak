# SlickEdit generated file.  Do not edit this file except in designated areas.

# Make command to use for dependencies
MAKE=make
RM=rm
MKDIR=mkdir

# -----Begin user-editable area-----

# -----End user-editable area-----

# If no configuration is specified, "Debug" will be used
ifndef CFG
CFG=Debug
endif

#
# Configuration: Debug
#
ifeq "$(CFG)" "Debug"
OUTDIR=Debug
OUTFILE=$(OUTDIR)/NVRAMTester
CFG_INC=-I/usr/include/sys -I../../Sentinel -I../../SDK-Include \
	-I../include 
CFG_LIB=-ls7lite -l:libphidget21.a -lusb-1.0 
CFG_OBJ=
COMMON_OBJ=$(OUTDIR)/NVRAMTester.o $(OUTDIR)/NVRAMTesterParts.o \
	$(OUTDIR)/TimerThread.o 
OBJ=$(COMMON_OBJ) $(CFG_OBJ)
ALL_OBJ=$(OUTDIR)/NVRAMTester.o $(OUTDIR)/NVRAMTesterParts.o \
	$(OUTDIR)/TimerThread.o -ls7lite -l:libphidget21.a -lusb-1.0 

COMPILE="$(CXX)" -c  "-DPOWER_DOWN=1" "-D_SENTINEL" -g ${CXXFLAGS} -O0 -std=c++11 -lstdc++ -Wall -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wextra -Wno-sign-compare -Wno-type-limits  -o "$(OUTDIR)/$(*F).o" $(CFG_INC) $<
LINK="$(CXX)"  -g ${CXXFLAGS} -O0 -std=c++11 -lstdc++ -rdynamic -ldl -pthread -o "$(OUTFILE)" $(ALL_OBJ)
COMPILE_ADA=gnat -g -c -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_ADB=gnat -g -c -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_F=gfortran -c -g -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_F90=gfortran -c -g -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_D=gdc -c -g -o "$(OUTDIR)/$(*F).o" "$<"

# Pattern rules
$(OUTDIR)/%.o : %.cpp
	$(COMPILE)

$(OUTDIR)/%.o : %.ada
	$(COMPILE_ADA)

$(OUTDIR)/%.o : %.d
	$(COMPILE_D)

$(OUTDIR)/%.o : %.adb
	$(COMPILE_ADB)

$(OUTDIR)/%.o : %.f90
	$(COMPILE_F90)

$(OUTDIR)/%.o : %.f
	$(COMPILE_F)

# Build rules
all: $(OUTFILE)

$(OUTFILE): $(OUTDIR)  $(OBJ)
	$(LINK)

$(OUTDIR):
	$(MKDIR) -p "$(OUTDIR)"

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	$(RM) -f $(OUTFILE)
	$(RM) -f $(OBJ)

# Clean this project and all dependencies
cleanall: clean
endif

#
# Configuration: Release
#
ifeq "$(CFG)" "Release"
OUTDIR=Release
OUTFILE=$(OUTDIR)/NVRAMTester
CFG_INC=-I/usr/include/sys -I../../Sentinel -I../../SDK-Include \
	-I../include 
CFG_LIB=-ls7lite -l:libphidget21.a -lusb-1.0 
CFG_OBJ=
COMMON_OBJ=$(OUTDIR)/NVRAMTester.o $(OUTDIR)/NVRAMTesterParts.o \
	$(OUTDIR)/TimerThread.o 
OBJ=$(COMMON_OBJ) $(CFG_OBJ)
ALL_OBJ=$(OUTDIR)/NVRAMTester.o $(OUTDIR)/NVRAMTesterParts.o \
	$(OUTDIR)/TimerThread.o -ls7lite -l:libphidget21.a -lusb-1.0 

COMPILE="$(CXX)" -c  "-DPOWER_DOWN=1" "-D_SENTINEL" ${CXXFLAGS} -O0 -std=c++11 -lstdc++ -Wall -Wno-unused-but-set-variable -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wextra -Wno-sign-compare -Wno-type-limits  -o "$(OUTDIR)/$(*F).o" $(CFG_INC) $<
LINK="$(CXX)"  ${CXXFLAGS} -O0 -std=c++11 -lstdc++ -rdynamic -ldl -pthread -o "$(OUTFILE)" $(ALL_OBJ)
COMPILE_ADA=gnat -O -c -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_ADB=gnat -O -c -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_F=gfortran -O -g -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_F90=gfortran -O -g -o "$(OUTDIR)/$(*F).o" "$<"
COMPILE_D=gdc -c -g -o "$(OUTDIR)/$(*F).o" "$<"

# Pattern rules
$(OUTDIR)/%.o : %.cpp
	$(COMPILE)

$(OUTDIR)/%.o : %.ada
	$(COMPILE_ADA)

$(OUTDIR)/%.o : %.d
	$(COMPILE_D)

$(OUTDIR)/%.o : %.adb
	$(COMPILE_ADB)

$(OUTDIR)/%.o : %.f90
	$(COMPILE_F90)

$(OUTDIR)/%.o : %.f
	$(COMPILE_F)

# Build rules
all: $(OUTFILE)

$(OUTFILE): $(OUTDIR)  $(OBJ)
	$(LINK)

$(OUTDIR):
	$(MKDIR) -p "$(OUTDIR)"

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	$(RM) -f $(OUTFILE)
	$(RM) -f $(OBJ)

# Clean this project and all dependencies
cleanall: clean
endif
