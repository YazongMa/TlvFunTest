echo off
set BASE_PATH=D:/TimProject/05_TestProc/HPS
set NDK_PATH=C:/Users/Administrator/AppData/Local/Android/Sdk/ndk-bundle
set CMAKE_PATH=C:/Users/Administrator/AppData/Local/Android/Sdk/cmake/3.10.2.4988404/bin

set WORKING_PATH=%BASE_PATH%/src
set NATIVEBUILD_PATH=%BASE_PATH%/build/saturn1000

%CMAKE_PATH%/cmake.exe ^
-H%WORKING_PATH% ^
-B%NATIVEBUILD_PATH%/debug/armeabi-v7a ^
-DPLATFORM_WORK_DIR=%WORKING_PATH% ^
-DCOMPILE_DIR=%NATIVEBUILD_PATH%/os ^
-DANDROID_ABI=armeabi-v7a ^
-DANDROID_PLATFORM=android-23 ^
-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=%NATIVEBUILD_PATH%/bin/armeabi-v7a ^
-DCMAKE_BUILD_TYPE=Debug ^
-DANDROID_NDK=%NDK_PATH% ^
-DCMAKE_SYSTEM_NAME=Android ^
-DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a ^
-DCMAKE_SYSTEM_VERSION=23 ^
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
-DCMAKE_ANDROID_NDK=%NDK_PATH% ^
-DCMAKE_TOOLCHAIN_FILE=%NDK_PATH%/build/cmake/android.toolchain.cmake ^
-G Ninja ^
-DCMAKE_MAKE_PROGRAM=%CMAKE_PATH%/ninja.exe


cd %NATIVEBUILD_PATH%
ls -l

if "%1" == "clean" (
    rmdir /S /Q "%NATIVEBUILD_PATH%/debug"
    rmdir /S /Q "%NATIVEBUILD_PATH%/bin"
    echo "clean successful"
) else (
    cd %NATIVEBUILD_PATH%/debug/armeabi-v7a
    %CMAKE_PATH%/ninja.exe

    ls -l "%NATIVEBUILD_PATH%/bin/armeabi-v7a"
    echo "build complete"
)

cd %WORKING_PATH%

pause
