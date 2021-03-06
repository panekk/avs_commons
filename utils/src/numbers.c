/*
 * Copyright 2017 AVSystem <avsystem@avsystem.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <avs_commons_config.h>

#include <avsystem/commons/utils.h>

#include <stdlib.h>

VISIBILITY_SOURCE_BEGIN

int avs_rand_r(unsigned int *seed) {
    return (*seed = *seed * 1103515245u + 12345u)
           % (unsigned int) (AVS_RAND_MAX + 1);
}

#ifdef AVS_COMMONS_BIG_ENDIAN
uint16_t avs_convert_be16(uint16_t value) {
    return value;
}

uint32_t avs_convert_be32(uint32_t value) {
    return value;
}

uint64_t avs_convert_be64(uint64_t value) {
    return value;
}
#else // AVS_COMMONS_BIG_ENDIAN
uint16_t avs_convert_be16(uint16_t value) {
    return (uint16_t) ((value >> 8) | (value << 8));
}

uint32_t avs_convert_be32(uint32_t value) {
    return (uint32_t) ((value >> 24)
            | ((value & 0xFF0000) >> 8)
            | ((value & 0xFF00) << 8)
            | (value << 24));
}

uint64_t avs_convert_be64(uint64_t value) {
    return (uint64_t) ((value >> 56)
            | ((value & UINT64_C(0xFF000000000000)) >> 40)
            | ((value & UINT64_C(0xFF0000000000)) >> 24)
            | ((value & UINT64_C(0xFF00000000)) >> 8)
            | ((value & UINT64_C(0xFF000000)) << 8)
            | ((value & UINT64_C(0xFF0000)) << 24)
            | ((value & UINT64_C(0xFF00)) << 40)
            | (value << 56));
}
#endif // AVS_COMMONS_BIG_ENDIAN
