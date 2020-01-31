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
CC=arm-brcm-linux-gnueabi-gcc
CCC=arm-brcm-linux-gnueabi-g++
CXX=arm-brcm-linux-gnueabi-g++
FC=gfortran.exe
AS=as

# Macros
CND_PLATFORM=Gnueabi-Windows
CND_DLIB_EXT=dll
CND_CONF=Device
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include NbMakefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/cURL.o


# C Compiler Flags
CFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wstrict-prototypes -Wno-trigraphs -Wimplicit -Wformat -fPIC 

# CC Compiler Flags
CCFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat -fPIC 
CXXFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat -fPIC 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread -lcurl -lssl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../build/upt1000/Lib/libcurl_kernel.so

../../build/upt1000/Lib/libcurl_kernel.so: ${OBJECTFILES}
	${MKDIR} -p ../../build/upt1000/Lib
	arm-brcm-linux-gnueabi-g++ -o ../../build/upt1000/Lib/libcurl_kernel.so ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl,--version-script=export.def -L . "-L${SDKUPT1000LIB}" "-L${SDKUPT1000LIBN}" -shared -fPIC

${OBJECTDIR}/cURL.o: cURL.c
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -fPIC  -o ${OBJECTDIR}/cURL.o cURL.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
