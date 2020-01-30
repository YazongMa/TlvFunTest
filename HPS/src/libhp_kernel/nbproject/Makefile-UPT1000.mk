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
	${OBJECTDIR}/ByteStream.o \
	${OBJECTDIR}/CallUserAction.o \
	${OBJECTDIR}/CurlObject.o \
	${OBJECTDIR}/EMVApi.o \
	${OBJECTDIR}/EMVBaseCallback.o \
	${OBJECTDIR}/FileFun.o \
	${OBJECTDIR}/FillTxnData.o \
	${OBJECTDIR}/FunBase.o \
	${OBJECTDIR}/KernelVersion.o \
	${OBJECTDIR}/PackSoap/BaseParam.o \
	${OBJECTDIR}/PackSoap/BaseSoap.o \
	${OBJECTDIR}/PackSoap/TransBatchClose.o \
	${OBJECTDIR}/PackSoap/TransCreditAddToBatch.o \
	${OBJECTDIR}/PackSoap/TransCreditAuth.o \
	${OBJECTDIR}/PackSoap/TransCreditReturn.o \
	${OBJECTDIR}/PackSoap/TransCreditSale.o \
	${OBJECTDIR}/PackSoap/TransCreditVoid.o \
	${OBJECTDIR}/PackSoap/TransDebitReturn.o \
	${OBJECTDIR}/PackSoap/TransDebitSale.o \
	${OBJECTDIR}/PackSoap/TransManualReversal.o \
	${OBJECTDIR}/PackSoap/TransReversal.o \
	${OBJECTDIR}/PackSoap/TransSAF.o \
	${OBJECTDIR}/PinObject.o \
	${OBJECTDIR}/StringFun.o \
	${OBJECTDIR}/TlvFun.o \
	${OBJECTDIR}/TxnBase.o \
	${OBJECTDIR}/TxnContext.o \
	${OBJECTDIR}/TxnEmv.o \
	${OBJECTDIR}/TxnEmvQC.o \
	${OBJECTDIR}/TxnEmvcl.o \
	${OBJECTDIR}/TxnMsr.o \
	${OBJECTDIR}/TxnNone.o \
	${OBJECTDIR}/TxnSAF.o \
	${OBJECTDIR}/VoltageObject.o


# C Compiler Flags
CFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wstrict-prototypes -Wno-trigraphs -Wimplicit -Wformat -fPIC 

# CC Compiler Flags
CCFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat -fPIC -fvisibility=hidden 
CXXFLAGS="-I${SDKUPT1000INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat -fPIC -fvisibility=hidden 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../build/upt1000/Lib/ -Wl,-rpath,'.' -lcaethernet -lcafont -lcafs -lcakms -lcalcd -lcamodem -lcapmodem -lcaprt -lcartc -lcauart -lcauldpm -lcausbh -lcagsm -lcabarcode -lpthread -ldl -lcaclvw -lcatls -lctosapi -lcrypto -lcurl -lssl -lz -lfreetype -lcaclentry -lcaclmdl -lcurl_kernel -lsoap_kernel -lcjson -lvoltage

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../build/upt1000/Lib/libhp_kernel.so

../../build/upt1000/Lib/libhp_kernel.so: ${OBJECTFILES}
	${MKDIR} -p ../../build/upt1000/Lib
	arm-brcm-linux-gnueabi-g++ -o ../../build/upt1000/Lib/libhp_kernel.so ${OBJECTFILES} ${LDLIBSOPTIONS} -Wl -L . "-L${SDKUPT1000LIB}" "-L${SDKUPT1000LIBN}" -shared -fPIC

${OBJECTDIR}/ByteStream.o: ByteStream.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/ByteStream.o ByteStream.cpp

${OBJECTDIR}/CallUserAction.o: CallUserAction.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/CallUserAction.o CallUserAction.cpp

${OBJECTDIR}/CurlObject.o: CurlObject.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/CurlObject.o CurlObject.cpp

${OBJECTDIR}/EMVApi.o: EMVApi.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/EMVApi.o EMVApi.cpp

${OBJECTDIR}/EMVBaseCallback.o: EMVBaseCallback.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/EMVBaseCallback.o EMVBaseCallback.cpp

${OBJECTDIR}/FileFun.o: FileFun.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/FileFun.o FileFun.cpp

${OBJECTDIR}/FillTxnData.o: FillTxnData.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/FillTxnData.o FillTxnData.cpp

${OBJECTDIR}/FunBase.o: FunBase.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/FunBase.o FunBase.cpp

${OBJECTDIR}/KernelVersion.o: KernelVersion.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/KernelVersion.o KernelVersion.cpp

${OBJECTDIR}/PackSoap/BaseParam.o: PackSoap/BaseParam.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/BaseParam.o PackSoap/BaseParam.cpp

${OBJECTDIR}/PackSoap/BaseSoap.o: PackSoap/BaseSoap.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/BaseSoap.o PackSoap/BaseSoap.cpp

${OBJECTDIR}/PackSoap/TransBatchClose.o: PackSoap/TransBatchClose.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransBatchClose.o PackSoap/TransBatchClose.cpp

${OBJECTDIR}/PackSoap/TransCreditAddToBatch.o: PackSoap/TransCreditAddToBatch.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransCreditAddToBatch.o PackSoap/TransCreditAddToBatch.cpp

${OBJECTDIR}/PackSoap/TransCreditAuth.o: PackSoap/TransCreditAuth.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransCreditAuth.o PackSoap/TransCreditAuth.cpp

${OBJECTDIR}/PackSoap/TransCreditReturn.o: PackSoap/TransCreditReturn.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransCreditReturn.o PackSoap/TransCreditReturn.cpp

${OBJECTDIR}/PackSoap/TransCreditSale.o: PackSoap/TransCreditSale.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransCreditSale.o PackSoap/TransCreditSale.cpp

${OBJECTDIR}/PackSoap/TransCreditVoid.o: PackSoap/TransCreditVoid.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransCreditVoid.o PackSoap/TransCreditVoid.cpp

${OBJECTDIR}/PackSoap/TransDebitReturn.o: PackSoap/TransDebitReturn.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransDebitReturn.o PackSoap/TransDebitReturn.cpp

${OBJECTDIR}/PackSoap/TransDebitSale.o: PackSoap/TransDebitSale.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransDebitSale.o PackSoap/TransDebitSale.cpp

${OBJECTDIR}/PackSoap/TransManualReversal.o: PackSoap/TransManualReversal.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransManualReversal.o PackSoap/TransManualReversal.cpp

${OBJECTDIR}/PackSoap/TransReversal.o: PackSoap/TransReversal.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransReversal.o PackSoap/TransReversal.cpp

${OBJECTDIR}/PackSoap/TransSAF.o: PackSoap/TransSAF.cpp
	${MKDIR} -p ${OBJECTDIR}/PackSoap
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PackSoap/TransSAF.o PackSoap/TransSAF.cpp

${OBJECTDIR}/PinObject.o: PinObject.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/PinObject.o PinObject.cpp

${OBJECTDIR}/StringFun.o: StringFun.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/StringFun.o StringFun.cpp

${OBJECTDIR}/TlvFun.o: TlvFun.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TlvFun.o TlvFun.cpp

${OBJECTDIR}/TxnBase.o: TxnBase.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnBase.o TxnBase.cpp

${OBJECTDIR}/TxnContext.o: TxnContext.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnContext.o TxnContext.cpp

${OBJECTDIR}/TxnEmv.o: TxnEmv.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnEmv.o TxnEmv.cpp

${OBJECTDIR}/TxnEmvQC.o: TxnEmvQC.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnEmvQC.o TxnEmvQC.cpp

${OBJECTDIR}/TxnEmvcl.o: TxnEmvcl.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnEmvcl.o TxnEmvcl.cpp

${OBJECTDIR}/TxnMsr.o: TxnMsr.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnMsr.o TxnMsr.cpp

${OBJECTDIR}/TxnNone.o: TxnNone.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnNone.o TxnNone.cpp

${OBJECTDIR}/TxnSAF.o: TxnSAF.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/TxnSAF.o TxnSAF.cpp

${OBJECTDIR}/VoltageObject.o: VoltageObject.cpp
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/UPT1000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/UPT1000/include -I../../build/upt1000/Inc/Heartland -I../../build/upt1000/Inc -fPIC  -o ${OBJECTDIR}/VoltageObject.o VoltageObject.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:
