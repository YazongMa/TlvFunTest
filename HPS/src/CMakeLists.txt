# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

######################################################################################
#
# if you want to complie castlespay platform, the CMAKE_COMPLIE_OPTIONAL
# should be set to "PLATFORM", otherwise the miniapp would be complie.
# if "COMPLIEALL" is set, both the castlespay platform and miniapp would be complied
#
######################################################################################



SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-trigraphs")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")


SET(COMPILE_INC             ${COMPILE_DIR}/CTOSInc)
SET(COMPILE_LIB             ${COMPILE_DIR}/CTOSLib)

SET(CJSON_WORK_DIR          ${PLATFORM_WORK_DIR}/libcjson)
SET(CURL_KERNEL_DIR         ${PLATFORM_WORK_DIR}/libcurl_kernel)
SET(SOAP_KERNEL_DIR         ${PLATFORM_WORK_DIR}/libsoap_kernel)
SET(VOLTAGE_WORK_DIR        ${PLATFORM_WORK_DIR}/libvoltage)
SET(ICC_SETTING_WORK_DIR    ${PLATFORM_WORK_DIR}/libicc_setting)
SET(HP_KERNEL_DIR           ${PLATFORM_WORK_DIR}/libhp_kernel)
SET(HP_API_DIR              ${PLATFORM_WORK_DIR}/libhp_api)

ADD_DEFINITIONS(-DCTOS_API)
ADD_DEFINITIONS(-DPLATFORM_ANDROID)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
INCLUDE_DIRECTORIES(
        ${CJSON_WORK_DIR}
        ${ICC_SETTING_WORK_DIR}
        ${SOAP_KERNEL_DIR}
        ${CURL_KERNEL_DIR}
        ${COMPILE_INC})

# link directories
LINK_DIRECTORIES(${COMPILE_LIB}/${ANDROID_ABI})


SET(SUBDIRECTORYS
        ${CJSON_WORK_DIR}
        ${CURL_KERNEL_DIR}
        ${SOAP_KERNEL_DIR}
        ${VOLTAGE_WORK_DIR}
        ${ICC_SETTING_WORK_DIR}
        ${HP_KERNEL_DIR}
        ${HP_API_DIR}
    )

#complie all the specified module
FOREACH(_var ${SUBDIRECTORYS})
     MESSAGE("complie subdirectory: ${_var}")
     ADD_SUBDIRECTORY(${_var})
ENDFOREACH()