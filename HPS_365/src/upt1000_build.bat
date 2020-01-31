@set CYGWIN=nodosfilewarning
if %PROCESSOR_ARCHITECTURE%==AMD64 (
GOTO PF_86
) else (
GOTO PF_64
)

:PF_86
set PF=%ProgramFiles(x86)%
GOTO END

:PF_64
set PF=%ProgramFiles%
GOTO END

:END
@set NETBEANPATH=%PF%\NetBeans 8.2\bin
@set CYGWINPATH=C:\cygwin\bin
@set CSDKPATH=%PF%\Castles

@set SDKDPSINC=%CSDKPATH%\DPS1000\include
@set SDKDPSLIB=%CSDKPATH%\DPS1000\lib
@set SDKEMV8900INC=%CSDKPATH%\EMV8900\include
@set SDKEMV8900LIB=%CSDKPATH%\EMV8900\lib
@set SDKEPP1000=%CSDKPATH%\epp1000
@set SDKEZPROXINC=%CSDKPATH%\EZProx\include
@set SDKEZPROXLIB=%CSDKPATH%\EZProx\lib
@set SDKPCI3KINC=%CSDKPATH%\PCI3000\include
@set SDKPCI3KLIB=%CSDKPATH%\PCI3000\lib
@set SDKPPC900INC=%CSDKPATH%\PPC900\include
@set SDKPPC900LIB=%CSDKPATH%\PPC900\lib
@set SDKQP1000INC=%CSDKPATH%\QP1000\include
@set SDKQP1000LIB=%CSDKPATH%\QP1000\lib
@set SDKQP2000INC=%CSDKPATH%\QP2000\include
@set SDKQP2000LIB=%CSDKPATH%\QP2000\lib
@set SDKQP3000INC=%CSDKPATH%\QP3000\include
@set SDKQP3000LIB=%CSDKPATH%\QP3000\lib
@set SDKQP3000SINC=%CSDKPATH%\QP3000S\include
@set SDKQP3000SLIB=%CSDKPATH%\QP3000S\lib
@set SDKV5INC=%CSDKPATH%\VEGA5000\include
@set SDKV5LIB=%CSDKPATH%\VEGA5000\lib
@set SDKV5LIBN=%CSDKPATH%\VEGA5000\lib_not_builtin
@set SDKV5SINC=%CSDKPATH%\VEGA5000S\include
@set SDKV5SLIB=%CSDKPATH%\VEGA5000S\lib
@set SDKV5SLIBN=%CSDKPATH%\VEGA5000S\lib_not_builtin
@set SDKV5SIMINC=%CSDKPATH%\VEGA5000\include
@set SDKV5SIMLIB=%CSDKPATH%\VEGA5000\slib
@set SDKV7INC=%CSDKPATH%\VEGA7000\include
@set SDKV7LIB=%CSDKPATH%\VEGA7000\lib
@set SDKV7SIMINC=%CSDKPATH%\VEGA7000\sinclude
@set SDKV7SIMLIB=%CSDKPATH%\VEGA7000\slib
@set SDKV7SIMRES=%CSDKPATH%\VEGA7000\sres
@set SDKV9INC=%CSDKPATH%\VEGA9000\include
@set SDKV9LIB=%CSDKPATH%\VEGA9000\lib
@set SDKV3INC=%CSDKPATH%\VEGA3000\include
@set SDKV3LIB=%CSDKPATH%\VEGA3000\lib
@set SDKV3LIBN=%CSDKPATH%\VEGA3000\lib_not_builtin
@set SDKV3ULINC=%CSDKPATH%\VEGA3000UL\include
@set SDKV3ULLIB=%CSDKPATH%\VEGA3000UL\lib
@set SDKV3ULLIBN=%CSDKPATH%\VEGA3000UL\lib_not_builtin
@set SDKMP10INC=%CSDKPATH%\MPIN10\include
@set SDKMP10LIB=%CSDKPATH%\MPIN10\lib
@set SDKMP10LIBN=%CSDKPATH%\MPIN10\lib_not_builtin
@set SDKMP200INC=%CSDKPATH%\MP200\include
@set SDKMP200LIB=%CSDKPATH%\MP200\lib
@set SDKMP200LIBN=%CSDKPATH%\MP200\lib_not_builtin
@set SDKMARS1000INC=%CSDKPATH%\MARS1000\include
@set SDKMARS1000LIB=%CSDKPATH%\MARS1000\lib
@set SDKMARS1000LIBN=%CSDKPATH%\MARS1000\lib_not_builtin
@set SDKUPT1000INC=%CSDKPATH%\UPT1000\include
@set SDKUPT1000LIB=%CSDKPATH%\UPT1000\lib
@set SDKUPT1000LIBN=%CSDKPATH%\UPT1000\lib_not_builtin
@set SDKUPT2000INC=%CSDKPATH%\UPT2000\include
@set SDKUPT2000LIB=%CSDKPATH%\UPT2000\lib
@set SDKUPT2000LIBN=%CSDKPATH%\UPT2000\lib_not_builtin
@set SDKS1EXTINC=%CSDKPATH%\SATURN1000Ext\include
@set SDKS1EXTLIB=%CSDKPATH%\SATURN1000Ext\lib
@set SDKS1EXTLIBN=%CSDKPATH%\SATURN1000Ext\lib_not_builtin
@set SDKQP8000INC=%CSDKPATH%\QP8000\include
@set SDKQP8000LIB=%CSDKPATH%\QP8000\lib
@set SDKQP3000EINC=%CSDKPATH%\QP3000E\include
@set SDKQP3000ELIB=%CSDKPATH%\QP3000E\lib


echo off
cd ../build/upt1000
mkdir Inc

cd Inc
mkdir Heartland

cd ..
mkdir Lib

cd ../../src
cp ./libsoap_kernel/*.h ../build/upt1000/Inc/Heartland/
cp ./libsoap_kernel/*.hpp ../build/upt1000/Inc/Heartland/
cp ./libcurl_kernel/*.h ../build/upt1000/Inc/Heartland/
cp ./libvoltage/voltage.h ../build/upt1000/Inc/Heartland/
cp ./libcjson/*.h ../build/upt1000/Inc/
cp ./libhp_api/TxnInterface.h ../build/upt1000/Inc/


echo off
cd libcjson
if "%1" == "clean" (
    make -f nbMakefile clean
) else (
    make -f NbMakefile -j
    if %errorlevel% NEQ 0 (
    	cd ..
    	color fc
    	echo cJSON library compile failed.
    	goto Error
    	)
)
cd ..

echo off
cd libvoltage
if "%1" == "clean" (
    make -f nbMakefile clean
) else (
    make -f NbMakefile -j
    if %errorlevel% NEQ 0 (
    	cd ..
    	color fc
    	echo libvoltage library compile failed.
    	goto Error
    	)
)
cd ..

cd libcurl_kernel
if "%1" == "clean" (
    make -f nbMakefile clean
) else (
    make -f NbMakefile -j
    if %errorlevel% NEQ 0 (
    	cd ..
    	color fc
    	echo CurlKernel library compile failed.
    	goto Error
    	)
)
cd ..

cd libsoap_kernel
if "%1" == "clean" (
    make -f nbMakefile clean
) else (
    make -f NbMakefile -j
    if %errorlevel% NEQ 0 (
    	cd ..
    	color fc
    	echo SoapKernel library compile failed.
    	goto Error
    	)
)
cd ..


cd libhp_kernel
if "%1" == "clean" (
    make -f nbMakefile clean
) else (
    make -f NbMakefile -j
    if %errorlevel% NEQ 0 (
    	cd ..
    	color fc
    	echo libhp_kernel library compile failed.
    	goto Error
    	)
)
cd ..


cd libhp_api
if "%1" == "clean" (
    make -f nbMakefile clean
) else (
    make -f NbMakefile -j
    if %errorlevel% NEQ 0 (
    	cd ..
    	color fc
    	echo libhp_api library compile failed.
    	goto Error
    	)
)
cd ..


cd Portico-CAN
if "%1" == "clean" (
    make -f nbMakefile clean
) else (
    make -f NbMakefile -j
    if %errorlevel% NEQ 0 (
    	cd ..
    	color fc
    	echo Portico-CAN application compile failed.
    	goto Error
    	)
)
cd ..

:Error
pause

