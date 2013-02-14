# ########## Project setup ##########
PROJECT(ttylog)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# ######### General setup ##########
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

# ######### Build defaults ##########
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O2")

# ########## ttylog executable ##########
# Sources:
SET(ttylog_executable_SRCS
    ttylog.c
)

# Headers:
SET(ttylog_executable_HDRS
)

# actual target:
ADD_EXECUTABLE(ttylog ${ttylog_executable_SRCS})

# add install targets:
INSTALL(TARGETS ttylog DESTINATION sbin)
# add install man page:
INSTALL(FILES ttylog.8 DESTINATION share/man/man8)
