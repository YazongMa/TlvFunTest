# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

CMAKE_MINIMUM_REQUIRED(VERSION 3.4.1)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--version-script=${TXNLIB_WORK_DIR}/export.def")

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

ADD_DEFINITIONS(-DPLATFORM_ANDROID)

ADD_DEFINITIONS(-DVOLTAGE_ENABLE)

ADD_DEFINITIONS(-DEMVDBG_ENABLE)

ADD_DEFINITIONS(-DEMVCLDBG_ENABLE)

AUX_SOURCE_DIRECTORY("${HP_API_DIR}" HP_API_SRC_DIR)

INCLUDE_DIRECTORIES(
        ${HP_API_DIR}/Inc)

ADD_LIBRARY(hp_api SHARED ${HP_API_SRC_DIR})

TARGET_LINK_LIBRARIES(
        hp_api
        icc_setting
        hp_kernel
        soap_kernel
        curl_kernel
        voltage
        vibepos
        cjson
        curl
        ssl
        crypto
        xml2
        cainterface
        sbkms
        caemvl2ap
        cacl
        caclmdl
        cafs
        camsr
        caprt
        cartc
        casc
        casystem
        cabarcode
        casound
        calcd
        util1
        getbufferif
        android
        log)
