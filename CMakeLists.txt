# ########## Project setup ##########
PROJECT(ttylog)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(TTYLOG_VERSION_MAJOR "0")
SET(TTYLOG_VERSION_MINOR "30")
SET(TTYLOG_VERSION_PATCH "0")
SET(TTYLOG_VERSION ${TTYLOG_VERSION_MAJOR}.${TTYLOG_VERSION_MINOR}.${TTYLOG_VERSION_PATCH})

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

# ######### General setup ##########
INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

# ######### Build defaults ##########
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

# ########## ttylog executable ##########
# Sources:
SET(ttylog_executable_SRCS
    ttylog.c
)

# Headers:
SET(ttylog_executable_HDRS
)

# ######### Package creation #########
SET(CPACK_PACKAGE_VERSION_MAJOR "${TTYLOG_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${TTYLOG_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${TTYLOG_VERSION_PATCH}")
SET(CPACK_PACKAGE_VERSION "${TTYLOG_VERSION}")
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_SOURCE_IGNORE_FILES "/build/;/.git/;/.svn/;")
INCLUDE(CPack)

# actual target:
ADD_EXECUTABLE(ttylog ${ttylog_executable_SRCS})

# link against librt:
if(UNIX AND NOT APPLE)
    target_link_libraries(ttylog rt)
endif()

# add install targets:
INSTALL(TARGETS ttylog DESTINATION sbin)
# add install man page:
INSTALL(FILES ttylog.8 DESTINATION share/man/man8)
