# Copyright 2017 AVSystem <avsystem@avsystem.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(SOURCES
    src/numbers.c
    src/strings.c
    src/hexlify.c
    src/time.c
    src/token.c)

set(PRIVATE_HEADERS
    src/x_time_conv.h)

option(WITH_POSIX_AVS_TIME "Enable avs_time_real_now() and avs_time_monotonic_now() implementation based on POSIX clock_gettime()" "${UNIX}")
if(WITH_POSIX_AVS_TIME)
    set(SOURCES ${SOURCES} compat/posix/compat_time.c)
endif()

set(ALL_SOURCES ${SOURCES})

set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include_public")

set(avs_utils_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

include_directories(${INCLUDE_DIRS})

add_library(avs_utils STATIC ${ALL_SOURCES})
avs_emit_deps(avs_utils ${MATH_LIBRARY})

add_avs_test(avs_utils ${ALL_SOURCES})
avs_install_export(avs_utils utils)
avs_propagate_exports()
install(DIRECTORY include_public/
        COMPONENT utils
        DESTINATION ${INCLUDE_INSTALL_DIR}
        FILES_MATCHING REGEX "[.]h$")
