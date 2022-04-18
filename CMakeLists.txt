cmake_minimum_required (VERSION 3.8)

project ("SipServer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

set(HELPERS_SRC_FILES "${SRC_DIR}/Helpers")
file(GLOB_RECURSE HELPERS_SRC
    "${HELPERS_SRC_FILES}/*.cpp"
    "${HELPERS_SRC_FILES}/*.h"
    "${HELPERS_SRC_FILES}/*.hpp"
)

set(SIP_SRC_FILES "${SRC_DIR}/SIP")
file(GLOB_RECURSE SIP_SRC
    "${SIP_SRC_FILES}/*.cpp"
    "${SIP_SRC_FILES}/*.h"
    "${SIP_SRC_FILES}/*.hpp"
)

include_directories(${HELPERS_SRC_FILES} ${SIP_SRC_FILES})
add_executable (SipServer "main.cpp" ${SIP_SRC} ${HELPERS_SRC} )
if (WIN32)
    target_link_libraries(SipServer wsock32 ws2_32)
elseif (UNIX)
    target_link_libraries(SipServer -pthread)
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

source_group("Helpers"  FILES ${HELPERS_SRC_FILES})
source_group("SIP"      FILES ${SIP_SRC_FILES})