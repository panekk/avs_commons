/*
 * AVSystem Commons Library
 *
 * Copyright (C) 2014 AVSystem <http://www.avsystem.com/>
 *
 * This code is free and open source software licensed under the MIT License.
 * See the LICENSE file for details.
 */

#ifndef AVS_COMMONS_LIST_H
#define AVS_COMMONS_LIST_H

#include <stddef.h>
#include <stdint.h>

#include <avsystem/commons/defs.h>

/**
 * @file list.h
 *
 * A generic singly linked list implementation.
 *
 * The testing code (<c>test_list.c</c>) may be a good starting point for usage
 * examples.
 */

/**
 * @def AVS_LIST_CONFIG_ALLOC(size)
 *
 * Reference to the memory allocation function used by the generic linked list.
 *
 * By default, this is defined as <c>calloc(1, size)</c>. It may be defined
 * prior to including <c>list.h</c> to use some custom memory allocation
 * function.
 *
 * The function used must be symmetric to the definition of
 * @ref AVS_LIST_CONFIG_FREE and must set at least the first
 * <c>sizeof(void *)</c> bytes of the allocated memory to zeroes.
 *
 * @param size Number of bytes to allocate
 *
 * @return Pointer to newly allocated memory, or <c>NULL</c> in case of error.
 */
#ifndef AVS_LIST_CONFIG_ALLOC
#include <stdlib.h>
#define AVS_LIST_CONFIG_ALLOC(size) calloc(1, size)
#endif

/**
 * @def AVS_LIST_CONFIG_FREE(ptr)
 *
 * Reference to the memory deallocation function used by the generic linked
 * list.
 *
 * By default, this is defined as <c>free(ptr)</c>. It may be defined prior to
 * including <c>list.h</c> to use some custom memory deallocation function.
 *
 * The function used must be symmetric to the definition of
 * @ref AVS_LIST_CONFIG_ALLOC.
 *
 * @param ptr Pointer to previously allocated memory that shall be freed.
 */
#ifndef AVS_LIST_CONFIG_FREE
#include <stdlib.h>
#define AVS_LIST_CONFIG_FREE free
#endif

#if !defined(AVS_LIST_CONFIG_TYPEOF) && !defined(AVS_LIST_CONFIG_NO_TYPEOF) \
        && __GNUC__
/**
 * Alias to the <c>typeof</c> keyword, if available.
 *
 * It will be automatically defined as <c>__typeof__(symbol)</c> if compiling on
 * a GNU compiler or compatible.
 *
 * <c>typeof</c> is not necessary for the library to function, but it increases
 * type safety and in some cases allows for cleaner code.
 *
 * It can be defined prior to including <c>list.h</c> to use the <c>typeof</c>
 * keyword available in the target compiler.
 *
 * Alternatively, <c>AVS_LIST_CONFIG_NO_TYPEOF</c> can be defined to suppress
 * using <c>typeof</c> even on GNU compilers (e.g. for testing).
 */
#define AVS_LIST_CONFIG_TYPEOF __typeof__
#endif

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Padding helper macro.
 *
 * The in-memory representation format of the list is as follows:
 *
 * <pre>
 *                                all pointers point here
 *                                |
 *                                v
 * +=================+===========+============================================+
 * | pointer to next | (padding) | element data                               |
 * +=================+===========+============================================+
 *  { AVS_LIST_SPACE_FOR_NEXT__ } {------------- arbitrary size -------------}
 *  { sizeof(void*) }
 * </pre>
 *
 * The <c>AVS_LIST_SPACE_FOR_NEXT__</c> macro calculates the space necessary
 * for the next element pointer with regard to the architecture's alignment
 * requirements. The padding shall be large enough so that the element data lay
 * on an address complying to strictest alignment requirements suitable for any
 * data type on the target architecture.
 *
 * The concept for calculating this size is based on the structure:
 *
 * <code>
 * struct {
 *     void *next;
 *     avs_max_align_t value;
 * };
 * </code>
 */
#define AVS_LIST_SPACE_FOR_NEXT__ \
offsetof(struct { \
    void *next; \
    avs_max_align_t value; \
}, value)

/**
 * List type for a given element type.
 *
 * This is simply an alias for a pointer type. It is purely syntactic sugar,
 * allowing for semantic marking of list types in the code.
 *
 * Please also note that the value of <c>NULL</c> is a valid, empty list.
 *
 * @param element_type Type of the list element.
 */
#define AVS_LIST(element_type) element_type*

/**
 * @def AVS_LIST_TYPEOF__(symbol)
 *
 * This macro is used to avoid having to specify list type where possible.
 * It uses <c>typeof</c> if possible. Otherwise, it just produces casts to
 * <c>void *</c>, which is unsafe, but permitted by the C standard
 * (will produce warnings, though).
 */
#ifdef AVS_LIST_CONFIG_TYPEOF
#define AVS_LIST_TYPEOF__(symbol) AVS_LIST_CONFIG_TYPEOF(symbol)
#else
#define AVS_LIST_TYPEOF__(symbol) void
#endif

/**
 * Returns a pointer to the next element.
 *
 * The entity returned is syntactically an lvalue.
 *
 * @param element Pointer to a list element.
 *
 * @return Pointer to the next list element.
 */
#define AVS_LIST_NEXT(element) \
(*AVS_APPLY_OFFSET(AVS_LIST_TYPEOF__(*(element)) *, element, \
        -AVS_LIST_SPACE_FOR_NEXT__))

/**
 * Returns a pointer to the variable holding the pointer to the next element.
 *
 * It is semantically equivalent to <c>(&AVS_LIST_NEXT(*(element_ptr)))</c>
 * but yields a <c>void *</c> instead of <c>void **</c> if <c>typeof</c> is not
 * available.
 *
 * @param element_ptr Pointer to a variable holding a pointer to a list element.
 *
 * @return Pointer to a variable in the list element holding a pointer to the
 *         next element.
 */
#define AVS_LIST_NEXT_PTR(element_ptr) \
((AVS_LIST_TYPEOF__(*(element_ptr)) *) &AVS_LIST_NEXT(*(element_ptr)))

/**
 * A shorthand notation for a for-each loop.
 *
 * It is a wrapper around a standard <c>for</c> clause, so all standard features
 * like <c>break</c> and <c>continue</c> will work as expected.
 *
 * <example>
 * The following code prints the contents of a list of <c>int</c>s.
 *
 * @code
 * AVS_LIST(int) list;
 * // ...
 * int *element;
 * AVS_LIST_FOREACH(element, list) {
 *     printf("%d\n", *element);
 * }
 * @endcode
 * </example>
 *
 * @param element Iterator variable. Will be assigned pointers to consecutive
 *                list elements with each iteration.
 *
 * @param list    Pointer to a first element in a list.
 */
#define AVS_LIST_FOREACH(element, list) \
for ((element) = (list); (element); (element) = AVS_LIST_NEXT(element))

/**
 * Iterates over a list, starting with the current element.
 *
 * It is semantically equivalent to <c>AVS_LIST_FOREACH(element, element)</c>.
 *
 * @param element Pointer to an element in a list. Will be assigned consecutive
 *                elements during iteration.
 */
#define AVS_LIST_ITERATE(element) \
for (; (element); (element) = AVS_LIST_NEXT(element))

/**
 * A for-each loop over pointers to element pointers.
 *
 * This is similar to @ref AVS_LIST_FOREACH, but the iterator is assigned
 * pointers to variables holding pointers to elements. This may be useful for
 * e.g. inserting elements during iteration.
 *
 * <example>
 * The following code inserts a 4 element before each 5 element in a list of
 * <c>int</c>s.
 *
 * @code
 * AVS_LIST(int) list;
 * // ...
 * AVS_LIST(int) *element_ptr;
 * AVS_LIST_FOREACH_PTR(element_ptr, &list) {
 *     if (**element_ptr == 5) {
 *         AVS_LIST_INSERT_NEW(int, element_ptr);
 *         **element_ptr = 4;
 *         // skip the new element to avoid infinite loop
 *         element_ptr = AVS_LIST_NEXT_PTR(element_ptr);
 *     }
 * }
 * @endcode
 * </example>
 *
 * @param element_ptr Iterator variable. Will be assigned pointers to variables
 *                    holding pointers to consecutive list elements with each
 *                    iteration.
 *
 * @param list_ptr    Pointer to a list variable.
 */
#define AVS_LIST_FOREACH_PTR(element_ptr, list_ptr) \
for ((element_ptr) = (list_ptr); \
     *(element_ptr); \
     (element_ptr) = AVS_LIST_NEXT_PTR(element_ptr))

/**
 * Iterates over a list as pointers to element pointers, starting with the
 * current element.
 *
 * It is semantically equivalent to
 * <c>AVS_LIST_FOREACH_PTR(element_ptr, element_ptr)</c>.
 *
 * @param element_ptr Pointer to a variable holding a pointer to an element in a
 *                    list. Will be assigned pointers to variables holding
 *                    pointers to consecutive list elements during iteration.
 */
#define AVS_LIST_ITERATE_PTR(element_ptr) \
for (; *(element_ptr); (element_ptr) = AVS_LIST_NEXT_PTR(element_ptr))

/**
 * Comparator type for @ref AVS_LIST_FIND_BY_VALUE_PTR and
 * @ref AVS_LIST_SORT.
 *
 * Standard library <c>memcmp</c> satisfies this type.
 *
 * @param a            The left element of comparison.
 *
 * @param b            The right element of comparison.
 *
 * @param element_size Size in bytes of compared elements.
 *
 * @return A negative value if <c>a &lt; b</c>, zero if <c>a == b</c> or
 *         a positive value if <c>a &gt; b</c>.
 */
typedef int (*avs_list_comparator_func_t)(const void *a,
                                          const void *b,
                                          size_t element_size);

/**
 * @name Internal functions
 *
 * These functions contain actual implementation of some of the list
 * functionality.
 *
 * They are wrapped in macros that offer additional layer of type safety if
 * <c>typeof</c> is available, so it is preferable to use them instead of these
 * functions directly.
 */
/**@{*/
void *avs_list_nth__(void *list, size_t n);
void **avs_list_nth_ptr__(void **list_ptr, size_t n);
void **avs_list_find_ptr__(void **list_ptr, void *element);
void **avs_list_find_by_value_ptr__(void **list_ptr,
                                    void *value_ptr,
                                    avs_list_comparator_func_t comparator,
                                    size_t value_size);
void *avs_list_tail__(void *list);
void *avs_list_detach__(void **to_detach_ptr);
size_t avs_list_size__(const void *list);
void avs_list_sort__(void **list_ptr,
                      avs_list_comparator_func_t comparator,
                      size_t element_size);
/**@}*/

/**
 * Returns a pointer to <i>n</i>-th element in a list.
 *
 * @param list Pointer to a first element in a list.
 *
 * @param n    Index of a desired element to return, with 0 being the first.
 *
 * @return Pointer to the desired element, or <c>NULL</c> if not found.
 */
#define AVS_LIST_NTH(list, n) \
((AVS_LIST_TYPEOF__(*(list)) *) avs_list_nth__((list), (n)))

/**
 * Returns a pointer to a variable holding the <i>n</i>-th element in a list.
 *
 * @param list_ptr Pointer to a list variable.
 *
 * @param n        Index of a desired element to return, with 0 being the first.
 *
 * @return Pointer to a variable holding a pointer to the desired element, or
 *         <c>NULL</c> if not found.
 */
#define AVS_LIST_NTH_PTR(list_ptr, n) \
((AVS_LIST_TYPEOF__(*(list_ptr)) *) \
        avs_list_nth_ptr__((void **) (list_ptr), (n)))

/**
 * Looks for a given element in the list and returns a pointer to the variable
 * holding it.
 *
 * <example>
 * The following function removes an element given by single pointer from a
 * list.
 *
 * @code
 * void remove_element(AVS_LIST(int) *list_ptr, int *element) {
 *     AVS_LIST(int) *element_ptr = AVS_LIST_FIND_PTR(list_ptr, element);
 *     if (element_ptr) {
 *         AVS_LIST_DELETE(element_ptr);
 *     }
 * }
 * @endcode
 * </example>
 *
 * @param list_ptr Pointer to a list variable.
 *
 * @param element  Pointer to the element to find.
 *
 * @return Pointer to a variable holding a pointer to the desired element, or
 *         <c>NULL</c> if not found.
 */
#define AVS_LIST_FIND_PTR(list_ptr, element) \
((AVS_LIST_TYPEOF__(*(list_ptr)) *)(intptr_t) \
        avs_list_find_ptr__((void **)(intptr_t)(list_ptr), \
                            (char *)(intptr_t)(element)))

/**
 * Looks for an element in the list, given its literal value, and returns a
 * pointer to the variable holding it.
 *
 * <example>
 * The following code finds the first element with a value of 5 in a list of
 * <c>int</c>s.
 *
 * @code
 * AVS_LIST(int) list;
 * // ...
 * int search_term = 5;
 * AVS_LIST(int) *first_found = AVS_LIST_FIND_BY_VALUE_PTR(&list, &search_term,
 *                                                         memcmp);
 * @endcode
 * </example>
 *
 * @param list_ptr   Pointer to a list variable.
 *
 * @param value_ptr  Pointer to the search term value.
 *
 * @param comparator Comparator function of type
 *                   @ref avs_list_comparator_func_t. <c>sizeof(*value_ptr)</c>
 *                   will be used as the <c>element_size</c> argument.
 *
 * @return Pointer to a variable holding a pointer to the first matching element
 *         found, or <c>NULL</c> if not found.
 */
#define AVS_LIST_FIND_BY_VALUE_PTR(list_ptr, value_ptr, comparator) \
((AVS_LIST_TYPEOF__(*(list_ptr)) *) \
        avs_list_find_by_value_ptr__((void **)(intptr_t)(list_ptr),\
                                     (void *)(intptr_t)(value_ptr),\
                                     (comparator),\
                                     sizeof(*(value_ptr))))

/**
 * Returns the last element in a list.
 *
 * @param list Pointer to the first element in a list.
 *
 * @return Pointer to the last element in a list, or <c>NULL</c> if the list is
 *         empty.
 */
#define AVS_LIST_TAIL(list) \
((AVS_LIST_TYPEOF__(*(list)) *) avs_list_tail__((list)))

/**
 * Allocates a new list element with an arbitrary size.
 *
 * Invokes @ref AVS_LIST_CONFIG_ALLOC with the desired size increased by the
 * space necessary for the next pointer, and returns the pointer to user data.
 *
 * @param size Number of bytes to allocate for user data.
 *
 * @return Newly allocated list element, as <c>void *</c>.
 */
#define AVS_LIST_NEW_BUFFER(size) \
((void *) (((char *) AVS_LIST_CONFIG_ALLOC(AVS_LIST_SPACE_FOR_NEXT__ + (size)))\
        + AVS_LIST_SPACE_FOR_NEXT__))

/**
 * Allocates a new list element of a given type.
 *
 * It is semantically equivalent to <c>AVS_LIST_NEW_BUFFER(sizeof(type))</c>.
 *
 * @param type Type of user data to allocate.
 *
 * @return Newly allocated list element, as <c>type *</c>.
 */
#define AVS_LIST_NEW_ELEMENT(type) \
((type *) AVS_LIST_NEW_BUFFER(sizeof(type)))

/**
 * Inserts an element into the list.
 *
 * @param destination_element_ptr Pointer to a variable holding a pointer to the
 *                                element (which may be null) before which to
 *                                insert the new element. The variable value
 *                                will be updated with the newly added element.
 *
 * @param new_element             The element to insert. Note that its current
 *                                next pointer value will be discarded, and as
 *                                such, should be <c>NULL</c>.
 */
#define AVS_LIST_INSERT(destination_element_ptr, new_element) \
do { \
    AVS_LIST_TYPEOF__(**(destination_element_ptr)) \
            **avs_list_insert_dest_ptr__ = \
            (AVS_LIST_TYPEOF__(**(destination_element_ptr)) **) \
            (destination_element_ptr); \
    AVS_LIST_TYPEOF__(*(new_element)) *avs_list_insert_new_element__ = \
            (new_element); \
    AVS_LIST_NEXT(avs_list_insert_new_element__) = \
            *avs_list_insert_dest_ptr__; \
    *avs_list_insert_dest_ptr__ = avs_list_insert_new_element__; \
} while (0)

/**
 * Allocates a new element and inserts it into the list.
 *
 * It is semantically equivalent to
 * <c>AVS_LIST_INSERT(destination_element_ptr, AVS_LIST_NEW_ELEMENT(type))</c>.
 *
 * @warning Note that this macro gives no immediate feedback about whether the
 *          memory allocation succeeded. It is thus adviced not to use it in any
 *          code in which memory allocations fails are anticipated.
 *
 * @param type                    Type of user data to allocate.
 *
 * @param destination_element_ptr Pointer to a variable holding a pointer to the
 *                                element (which may be null) before which to
 *                                insert the new element. The variable value
 *                                will be updated with the newly added element.
 */
#define AVS_LIST_INSERT_NEW(type, destination_element_ptr) \
do { \
    type *avs_list_insert_new_value__ = AVS_LIST_NEW_ELEMENT(type); \
    if (avs_list_insert_new_value__) { \
        AVS_LIST_INSERT(destination_element_ptr, \
                        avs_list_insert_new_value__); \
    } \
} while (0)

/**
 * Appends an element or a list at the end of a list.
 *
 * @param list_ptr    Pointer to a list variable.
 *
 * @param new_element An element to append. If it has subsequent elements (i.e.
 *                    is already a list), they will be preserved, actually
 *                    concatenating two lists.
 */
#define AVS_LIST_APPEND(list_ptr, new_element) \
do { \
    AVS_LIST_TYPEOF__(**(list_ptr)) **avs_list_insert_ptr__; \
    AVS_LIST_FOREACH_PTR(avs_list_insert_ptr__, \
                         (AVS_LIST_TYPEOF__(**(list_ptr)) **) list_ptr); \
    *avs_list_insert_ptr__ = new_element; \
} while (0)

/**
 * Detaches an element from a list.
 *
 * @param element_to_detach_ptr Pointer to a variable on a list holding a
 *                              pointer to the element to detach.
 *
 * @return Pointer to the detached element, that has been removed from the list
 *         originally containing it, and is now a self-contained element with
 *         the <i>next</i> pointer guaranteed to be <c>NULL</c>.
 */
/* additional casts through char * work around aliasing rules */
#define AVS_LIST_DETACH(element_to_detach_ptr) \
((AVS_LIST_TYPEOF__(**(element_to_detach_ptr)) *) (char *) \
        avs_list_detach__((void **) (char *) (element_to_detach_ptr)))

/**
 * Deallocates memory claimed by a list element, detaching it beforehand.
 *
 * @param element_to_delete_ptr Pointer to a variable on a list holding a
 *                              pointer to the element to destroy.
 */
#define AVS_LIST_DELETE(element_to_delete_ptr) \
AVS_LIST_CONFIG_FREE(((char *) (AVS_LIST_DETACH(element_to_delete_ptr))) \
        - AVS_LIST_SPACE_FOR_NEXT__)

/**
 * Checks whether the element has not been deleted during
 * @ref AVS_LIST_DELETABLE_FOREACH_PTR.
 *
 * @param element_ptr    Variable passed as <c>element_ptr</c> to
 *                       @ref AVS_LIST_DELETABLE_FOREACH_PTR.
 *
 * @param helper_element Variable passed as <c>helper_element</c> to
 *                       @ref AVS_LIST_DELETABLE_FOREACH_PTR.
 *
 * @return True, unless <c>element_ptr</c> has been deleted.
 */
#define AVS_LIST_DELETABLE_FOREACH_PTR_VALID(element_ptr, helper_element) \
((helper_element) == *(element_ptr))

/**
 * A for-each loop that allows deleting elements during iteration.
 *
 * This is similar to @ref AVS_LIST_FOREACH_PTR, but elements may be detached
 * or deleted during iteration.
 *
 * <example>
 * The following code deletes all elements with a value of 5 from a list of
 * <c>int</c>s.
 *
 * @code
 * AVS_LIST(int) list;
 * // ...
 * AVS_LIST(int) *element_ptr;
 * AVS_LIST(int) helper;
 * AVS_LIST_DELETABLE_FOREACH_PTR(element_ptr, helper, &list) {
 *     if (**element_ptr == 5) {
 *         AVS_LIST_DELETE(element_ptr);
 *     }
 * }
 * @endcode
 * </example>
 *
 * @param element_ptr    Iterator variable. Will be assigned pointers to
 *                       variables holding pointers to consecutive list elements
 *                       with each iteration.
 *
 * @param helper_element Helper variable (of element pointer type - not pointer
 *                       to variable holding pointer to element, as in the case
 *                       of <c>element_ptr</c>), used internally by the
 *                       iteration algorithm. <strong>It shall not be modified
 *                       by user code.</strong>
 *
 * @param list_ptr       Pointer to a list variable.
 */
#define AVS_LIST_DELETABLE_FOREACH_PTR(element_ptr, helper_element, list_ptr) \
for ((element_ptr) = (list_ptr), (helper_element) = *(element_ptr); \
     *(element_ptr); \
     (element_ptr) =  \
             AVS_LIST_DELETABLE_FOREACH_PTR_VALID(element_ptr, helper_element) \
             ? AVS_LIST_NEXT_PTR(element_ptr) : (element_ptr), \
             (helper_element) = *(element_ptr))

/**
 * Deallocates all list elements.
 *
 * It can be used as a normal statement if no additional freeing code is
 * necessary.
 *
 * Alternatively a block of code can follow it, that can do additional cleanup -
 * the first element of the list at the moment of execution is to be considered.
 *
 * <example>
 * Example usage of the long form:
 *
 * @code
 * typedef struct {
 *     int *some_data;
 *     double *some_other_data;
 * } complicated_structure_t;
 * AVS_LIST(complicated_structure_t) list;
 * // ...
 * AVS_LIST_CLEAR(&list) {
 *     free(list->some_data);
 *     free(list->some_other_data);
 * }
 * @endcode
 * </example>
 *
 * @param first_element_ptr Pointer to a list variable.
 */
#define AVS_LIST_CLEAR(first_element_ptr) \
for (; *(first_element_ptr); AVS_LIST_DELETE(first_element_ptr))

/**
 * @def AVS_LIST_SIZE(list)
 *
 * Returns the number of elements on the list.
 *
 * @param list Pointer to the first element of a list.
 *
 * @return Number of elements on the list.
 */
#define AVS_LIST_SIZE avs_list_size__

/**
 * Sorts the list elements, ascending by the ordering enforced by the specified
 * comparator.
 *
 * The sorting is performed using the recursive merge sort algorithm.
 *
 * @param list_ptr   Pointer to a list variable.
 *
 * @param comparator Comparator function of type
 *                   @ref avs_list_comparator_func_t. <c>sizeof(**list_ptr)</c>
 *                   will be used as the <c>element_size</c> argument.
 */
#define AVS_LIST_SORT(list_ptr, comparator) \
avs_list_sort__((void **)(intptr_t)(list_ptr), (comparator), \
                sizeof(**(list_ptr)))

#ifdef	__cplusplus
}
#endif

#endif /* AVS_COMMONS_LIST_H */