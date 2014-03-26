/*
 * AVSystem Commons Library
 *
 * Copyright (C) 2014 AVSystem <http://www.avsystem.com/>
 *
 * This code is free and open source software licensed under the MIT License.
 * See the LICENSE file for details.
 */

#ifndef AVS_COMMONS_DEFS_H
#define AVS_COMMONS_DEFS_H

#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @file defs.h
 *
 * Global common definitions.
*/

/**
 * Returns a pointer to a structure member given by offset. Can be thought of as
 * an inverse operation to standard library <c>offsetof</c>
 *
 * @param type       Type of the data member.
 *
 * @param struct_ptr Pointer to a data structure.
 *
 * @param offset     Offset in bytes from <c>struct_ptr</c>.
 */
#define AVS_APPLY_OFFSET(type, struct_ptr, offset) \
        ((type *) (((char *) (intptr_t) (struct_ptr)) + (offset)))

/**
 * C89-compliant replacement for <c>max_align_t</c>.
 *
 * <c>max_align_t</c> is a type defined in C11 and C++11 standards, that has
 * alignment requirements suitable for any primitive data type.
 *
 * This type simulates it with an union of types that are considered candidates
 * for the largest type available - a pointer, a function pointer,
 * <c>long double</c> and <c>intmax_t</c>.
 */
typedef union {
    /** @cond Doxygen_Suppress */
    /* candidates for "largest type"
     * add offending type if getting alignment errors */
    void *ptr;
    void (*fptr)();
    long double ld;
    intmax_t i;
    /** @endcond */
} avs_max_align_t;

#ifdef	__cplusplus
}
#endif

#endif	/* AVS_COMMONS_DEFS_H */
