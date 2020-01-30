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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include NbMakefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AppMain.o \
	${OBJECTDIR}/CTouch.o \
	${OBJECTDIR}/CtrlThread.o \
	${OBJECTDIR}/EthLink.o \
	${OBJECTDIR}/GetResponseThread.o \
	${OBJECTDIR}/GprsLink.o \
	${OBJECTDIR}/Receipt.o \
	${OBJECTDIR}/Rs232Link.o \
	${OBJECTDIR}/TouchThread.o \
	${OBJECTDIR}/TxnRecord.o \
	${OBJECTDIR}/TxnRequest.o \
	${OBJECTDIR}/Ui.o \
	${OBJECTDIR}/UnzipRes.o


# C Compiler Flags
CFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wstrict-prototypes -Wno-trigraphs -Wimplicit -Wformat 

# CC Compiler Flags
CCFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 
CXXFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lcaethernet -lcafont -lcafs -lcakms -lcalcd -lcamodem -lcapmodem -lcaprt -lcartc -lcauart -lcauldpm -lcausbh -lcagsm -lcabarcode -lpthread -ldl -lcaclvw -lcatls -lctosapi -lcrypto -lcurl -lssl -lz -lfreetype -lcaclentry -lcaclmdl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/upt1000/Debug/TestAP.exe

dist/upt1000/Debug/TestAP.exe: ${OBJECTFILES}
	${MKDIR} -p dist/upt1000/Debug
	arm-brcm-linux-gnueabi-g++ -o dist/upt1000/Debug/TestAP ${OBJECTFILES} ${LDLIBSOPTIONS} -L . "-L${SDKUPT1000LIB}" "-L${SDKUPT1000LIBN}"

${OBJECTDIR}/AppMain.o: AppMain.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/AppMain.o AppMain.cpp

${OBJECTDIR}/CTouch.o: CTouch.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/CTouch.o CTouch.cpp

${OBJECTDIR}/CtrlThread.o: CtrlThread.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/CtrlThread.o CtrlThread.cpp

${OBJECTDIR}/EthLink.o: EthLink.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/EthLink.o EthLink.cpp

${OBJECTDIR}/GetResponseThread.o: GetResponseThread.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/GetResponseThread.o GetResponseThread.cpp

${OBJECTDIR}/GprsLink.o: GprsLink.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/GprsLink.o GprsLink.cpp

${OBJECTDIR}/Receipt.o: Receipt.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/Receipt.o Receipt.cpp

${OBJECTDIR}/Rs232Link.o: Rs232Link.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/Rs232Link.o Rs232Link.cpp

${OBJECTDIR}/TouchThread.o: TouchThread.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/TouchThread.o TouchThread.cpp

${OBJECTDIR}/TxnRecord.o: TxnRecord.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/TxnRecord.o TxnRecord.cpp

${OBJECTDIR}/TxnRequest.o: TxnRequest.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/TxnRequest.o TxnRequest.cpp

${OBJECTDIR}/Ui.o: Ui.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/Ui.o Ui.cpp

${OBJECTDIR}/UnzipRes.o: UnzipRes.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -o ${OBJECTDIR}/UnzipRes.o UnzipRes.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
