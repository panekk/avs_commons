#
#  AVSystem Commons Library
#
#  Copyright (C) 2014 AVSystem <http://www.avsystem.com/>
#
#  This code is free and open source software licensed under the MIT License.
#  See the LICENSE file for details.
#

cmake_minimum_required(VERSION 2.8.1)
project(avs_commons C)
set(AVS_COMMONS_VERSION SNAPSHOT)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AVSystem Commons Library")
set(CPACK_PACKAGE_VENDOR "AVSystem")
set(CPACK_PACKAGE_VERSION "${AVS_COMMONS_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")

set(CPACK_SOURCE_IGNORE_FILES
    \\\\.a\\$
    \\\\.so\\$
    \\\\.so\\\\.
    /list/avs_list_test\\$
    /buffer/avs_buffer_test\\$
    /nbproject
    \\\\.log\\$
    /CMakeFiles/
    /CMakeTmp/
    /Makefile\\$
    /CMakeCache\\\\.txt\\$
    \\\\.cmake\\$
    /compile_commands\\\\.json\\$
    /install_manifest\\\\.txt\\$
    /_CPack_Packages/
    /Testing/
    \\\\.tar\\\\.gz\\$
    \\\\.tgz\\$
    \\\\.deb\\$
    /\\\\.git)

include(CPack)

add_custom_target(dist COMMAND ${CMAKE_CPACK_COMMAND} --config ${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

option(WITH_EXTRA_WARNINGS "Enable extra compilation warnings" OFF)
if(WITH_EXTRA_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -Wall -Wextra -Winit-self -Wmissing-declarations -Wc++-compat -Wsign-conversion -Wconversion -Wcast-qual -Wno-variadic-macros -Wno-long-long")
endif()

if(CMAKE_C_COMPILE_OPTIONS_PIC)
    option(WITH_PIC "Generate position-independent code" ON)
    if(WITH_PIC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_COMPILE_OPTIONS_PIC}")
    endif()
endif()

include_directories(include_public)

find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    option(WITH_VALGRIND "Enable usage of valgrind during unit tests" ON)
endif()
if(WITH_VALGRIND)
    execute_process(COMMAND ${VALGRIND_EXECUTABLE} --tool=helgrind --version
                    RESULT_VARIABLE HELGRIND_TEST_RESULT
                    OUTPUT_QUIET ERROR_QUIET)
    if(HELGRIND_TEST_RESULT EQUAL 0)
        option(WITH_HELGRIND "Put valgrind in helgrind mode (test for race conditions instead of memory errors)" OFF)
    endif()
    if(WITH_HELGRIND)
        set(VALGRIND_ARGS --tool=helgrind)
    else()
        set(VALGRIND_ARGS --leak-check=full --track-origins=yes)
    endif()
    set(VALGRIND ${VALGRIND_EXECUTABLE} ${VALGRIND_ARGS} -q --error-exitcode=63)
endif()

enable_testing()
add_custom_target(check)

if(NOT LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR lib)
endif()

if(NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR include)
endif()

set(DOXYGEN_SKIP_DOT TRUE)
find_package(Doxygen)

if(DOXYGEN_FOUND)
    configure_file(doc/Doxyfile.in doc/Doxyfile @ONLY)
    add_custom_target(doc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile)
endif()

add_subdirectory(unit)
add_subdirectory(buffer)
add_subdirectory(list)

install(EXPORT avs_commons-targets
        DESTINATION ${LIB_INSTALL_DIR}/avs_commons)
configure_file(avs_commons-config.cmake.in avs_commons-config.cmake @ONLY)
configure_file(avs_commons-version.cmake.in avs_commons-version.cmake @ONLY)
install(EXPORT avs_commons-targets DESTINATION ${LIB_INSTALL_DIR}/avs_commons)
install(FILES
        ${CMAKE_BINARY_DIR}/avs_commons-config.cmake
        ${CMAKE_BINARY_DIR}/avs_commons-version.cmake
        DESTINATION ${LIB_INSTALL_DIR}/avs_commons)
install(DIRECTORY include_public/
        DESTINATION ${INCLUDE_INSTALL_DIR}
        FILES_MATCHING REGEX "[.]h$")