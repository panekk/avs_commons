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
    src/buffer.c)

set(PUBLIC_HEADERS
    include_public/avsystem/commons/buffer.h)

set(ALL_SOURCES ${SOURCES} ${PUBLIC_HEADERS})

set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include_public")

set(avs_buffer_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

include_directories(${INCLUDE_DIRS})

add_library(avs_buffer STATIC ${ALL_SOURCES})

avs_install_export(avs_buffer buffer)
install(DIRECTORY include_public/
        COMPONENT buffer
        DESTINATION ${INCLUDE_INSTALL_DIR}
        FILES_MATCHING REGEX "[.]h$")

include_directories(${AVS_TEST_INCLUDE_DIRS})
add_avs_test(avs_buffer ${ALL_SOURCES})
if(WITH_INTERNAL_LOGS)
    target_link_libraries(avs_buffer avs_log)
    if(TARGET avs_buffer_test)
        target_link_libraries(avs_buffer_test avs_log)
    endif()
    include_directories(../log/include_public)
endif()
