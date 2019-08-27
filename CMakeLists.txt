cmake_minimum_required(VERSION 3.12)
project(stropt
		VERSION 0.0.1
		DESCRIPTION "Parse options from a string (it supports quotation, option arguments)"
		HOMEPAGE_URL "https://github.com/rd235/libstropt"
		LANGUAGES C)

include(GNUInstallDirs)
add_definitions(-D_GNU_SOURCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if (PROJECT_INCLUDEDIR)
  include_directories(${PROJECT_INCLUDEDIR})
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/stropt.h ${PROJECT_INCLUDEDIR}/.)
endif (PROJECT_INCLUDEDIR)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(STROPT_SOURCES stropt.c)
add_library(stropt SHARED ${STROPT_SOURCES})

set_target_properties(stropt PROPERTIES VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR})

configure_file(stropt.pc.in stropt.pc @ONLY)

install(TARGETS stropt LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES stropt.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stropt.pc
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

add_subdirectory(man)
