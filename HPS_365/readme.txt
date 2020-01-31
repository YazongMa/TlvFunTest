At 16:49, JAN 8th, 2020
1. upt1000 -- build
    a) enter ./src directory
    b) double click "upt1000_build.bat" to build
    c) enter ./build/upt1000 directory, TxnInterface.h from Inc is the final header file, and .so files from Lib are the final libraries
2. upt1000 -- clean
    a) open commannd line, enter ./src directory, perform the below command:
        upt1000_build.bat clean


3. saturn1000 -- build
    user must modify the BASE_PATH in the "./src/saturn1000_build.bat" file, and check the path for both NDK_PATH and CMAKE_PATH, then perform the following command
    a) enter ./src directory
    b) double click "saturn1000_build.bat" to build
    c) enter ./build/saturn1000 directory, the .so files from bin directory are the final libraries
4. saturn1000 -- clean
    a) open commannd line, enter ./src directory, perform the below command:
        saturn1000_build.bat clean