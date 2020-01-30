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
CND_CONF=UPT1000
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include NbMakefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/DetectCard.o \
	${OBJECTDIR}/HPApiContext.o \
	${OBJECTDIR}/HPApiKernel.o \
	${OBJECTDIR}/HP_Api.o \
	${OBJECTDIR}/JsonReqCmd.o \
	${OBJECTDIR}/JsonRspCmd.o \
	${OBJECTDIR}/KernelLayerObjs.o \
	${OBJECTDIR}/TlvFun.o \
	${OBJECTDIR}/TxnInterface.o


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
LDLIBSOPTIONS=-L../../build/upt1000/Lib/ -Wl,-rpath,'.' -lcaethernet -lcafont -lcafs -lcakms -lcalcd -lcamodem -lcapmodem -lcaprt -lcartc -lcauart -lcauldpm -lcausbh -lcagsm -lcabarcode -lpthread -ldl -lcaclvw -lcatls -lctosapi -lcrypto -lcurl -lssl -lz -lfreetype -lcaclentry -lcaclmdl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../build/upt1000/Lib/libhp_api.so

../../build/upt1000/Lib/libhp_api.so: ${OBJECTFILES}
	${MKDIR} -p ../../build/upt1000/Lib
	arm-brcm-linux-gnueabi-g++ -o ../../build/upt1000/Lib/libhp_api.so ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl,--version-script=export.def -L . "-L${SDKUPT1000LIB}" "-L${SDKUPT1000LIBN}" -shared -fPIC

${OBJECTDIR}/DetectCard.o: DetectCard.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/DetectCard.o DetectCard.cpp

${OBJECTDIR}/HPApiContext.o: HPApiContext.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/HPApiContext.o HPApiContext.cpp

${OBJECTDIR}/HPApiKernel.o: HPApiKernel.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/HPApiKernel.o HPApiKernel.cpp

${OBJECTDIR}/HP_Api.o: HP_Api.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/HP_Api.o HP_Api.cpp

${OBJECTDIR}/JsonReqCmd.o: JsonReqCmd.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/JsonReqCmd.o JsonReqCmd.cpp

${OBJECTDIR}/JsonRspCmd.o: JsonRspCmd.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/JsonRspCmd.o JsonRspCmd.cpp

${OBJECTDIR}/KernelLayerObjs.o: KernelLayerObjs.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/KernelLayerObjs.o KernelLayerObjs.cpp

${OBJECTDIR}/TlvFun.o: TlvFun.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TlvFun.o TlvFun.cpp

${OBJECTDIR}/TxnInterface.o: TxnInterface.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnInterface.o TxnInterface.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
