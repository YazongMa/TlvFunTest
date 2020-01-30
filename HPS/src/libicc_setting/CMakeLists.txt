# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

CMAKE_MINIMUM_REQUIRED(VERSION 3.4.1)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--version-script=${SOAP_WORK_DIR}/export.def")

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

AUX_SOURCE_DIRECTORY("${ICC_SETTING_WORK_DIR}" ICC_SETTING_SRC_DIR)

ADD_LIBRARY(icc_setting SHARED ${ICC_SETTING_SRC_DIR})

TARGET_LINK_LIBRARIES(
        icc_setting)


