#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=g77
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Tests/ASAP/admb2r.o \
	${OBJECTDIR}/Tests/ASAP/asap2mas.o \
	${OBJECTDIR}/Tests/NLL/main.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-DEIGEN_DONT_VECTORIZE -Wall -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
CXXFLAGS=-DEIGEN_DONT_VECTORIZE -Wall -DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mas

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mas: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mas ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/Tests/ASAP/admb2r.o: Tests/ASAP/admb2r.cpp
	${MKDIR} -p ${OBJECTDIR}/Tests/ASAP
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ithird_party/ATL/lib/third_party/EASTL/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Tests/ASAP/admb2r.o Tests/ASAP/admb2r.cpp

${OBJECTDIR}/Tests/ASAP/asap2mas.o: Tests/ASAP/asap2mas.cpp
	${MKDIR} -p ${OBJECTDIR}/Tests/ASAP
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ithird_party/ATL/lib/third_party/EASTL/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Tests/ASAP/asap2mas.o Tests/ASAP/asap2mas.cpp

${OBJECTDIR}/Tests/NLL/main.o: Tests/NLL/main.cpp
	${MKDIR} -p ${OBJECTDIR}/Tests/NLL
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ithird_party/ATL/lib/third_party/EASTL/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Tests/NLL/main.o Tests/NLL/main.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Ithird_party/ATL/lib/third_party/EASTL/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
