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

#ifndef AVS_COMMONS_COAP_MSG_H
#define AVS_COMMONS_COAP_MSG_H

#include <assert.h>
#include <stdint.h>

#include <avsystem/commons/defs.h>

#include <avsystem/commons/coap/opt.h>
#include <avsystem/commons/coap/msg_identity.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Magic value defined in RFC7252, used internally when constructing/parsing
 * CoAP packets.
 */
#define AVS_COAP_PAYLOAD_MARKER ((uint8_t) 0xFF)

/** Miminum size, in bytes, of a correct CoAP message. */
#define AVS_COAP_MSG_MIN_SIZE ((unsigned) sizeof(avs_coap_msg_header_t))

/** CoAP message type, as defined in RFC7252. */
typedef enum avs_coap_msg_type {
    AVS_COAP_MSG_CONFIRMABLE,
    AVS_COAP_MSG_NON_CONFIRMABLE,
    AVS_COAP_MSG_ACKNOWLEDGEMENT,
    AVS_COAP_MSG_RESET,

    _AVS_COAP_MSG_FIRST = AVS_COAP_MSG_CONFIRMABLE,
    _AVS_COAP_MSG_LAST = AVS_COAP_MSG_RESET
} avs_coap_msg_type_t;

/** @{
 * Internal macros used for constructing/parsing CoAP codes.
 */
#define AVS_COAP_CODE_CLASS_MASK 0xE0
#define AVS_COAP_CODE_CLASS_SHIFT 5
#define AVS_COAP_CODE_DETAIL_MASK 0x1F
#define AVS_COAP_CODE_DETAIL_SHIFT 0

#define AVS_COAP_CODE(cls, detail) \
    ((((cls) << AVS_COAP_CODE_CLASS_SHIFT) \
      & AVS_COAP_CODE_CLASS_MASK) \
     | (((detail) << AVS_COAP_CODE_DETAIL_SHIFT) \
        & AVS_COAP_CODE_DETAIL_MASK))
/** @} */

/** @{
 * CoAP code constants, as defined in RFC7252/RFC7959.
 *
 * For detailed description of their semantics, refer to appropriate RFCs.
 */
#define AVS_COAP_CODE_EMPTY  AVS_COAP_CODE(0, 0)

#define AVS_COAP_CODE_GET    AVS_COAP_CODE(0, 1)
#define AVS_COAP_CODE_POST   AVS_COAP_CODE(0, 2)
#define AVS_COAP_CODE_PUT    AVS_COAP_CODE(0, 3)
#define AVS_COAP_CODE_DELETE AVS_COAP_CODE(0, 4)

#define AVS_COAP_CODE_CREATED  AVS_COAP_CODE(2, 1)
#define AVS_COAP_CODE_DELETED  AVS_COAP_CODE(2, 2)
#define AVS_COAP_CODE_VALID    AVS_COAP_CODE(2, 3)
#define AVS_COAP_CODE_CHANGED  AVS_COAP_CODE(2, 4)
#define AVS_COAP_CODE_CONTENT  AVS_COAP_CODE(2, 5)
#define AVS_COAP_CODE_CONTINUE AVS_COAP_CODE(2, 31)

#define AVS_COAP_CODE_BAD_REQUEST                AVS_COAP_CODE(4, 0)
#define AVS_COAP_CODE_UNAUTHORIZED               AVS_COAP_CODE(4, 1)
#define AVS_COAP_CODE_BAD_OPTION                 AVS_COAP_CODE(4, 2)
#define AVS_COAP_CODE_FORBIDDEN                  AVS_COAP_CODE(4, 3)
#define AVS_COAP_CODE_NOT_FOUND                  AVS_COAP_CODE(4, 4)
#define AVS_COAP_CODE_METHOD_NOT_ALLOWED         AVS_COAP_CODE(4, 5)
#define AVS_COAP_CODE_NOT_ACCEPTABLE             AVS_COAP_CODE(4, 6)
#define AVS_COAP_CODE_REQUEST_ENTITY_INCOMPLETE  AVS_COAP_CODE(4, 8)
#define AVS_COAP_CODE_PRECONDITION_FAILED        AVS_COAP_CODE(4, 12)
#define AVS_COAP_CODE_REQUEST_ENTITY_TOO_LARGE   AVS_COAP_CODE(4, 13)
#define AVS_COAP_CODE_UNSUPPORTED_CONTENT_FORMAT AVS_COAP_CODE(4, 15)

#define AVS_COAP_CODE_INTERNAL_SERVER_ERROR  AVS_COAP_CODE(5, 0)
#define AVS_COAP_CODE_NOT_IMPLEMENTED        AVS_COAP_CODE(5, 1)
#define AVS_COAP_CODE_BAD_GATEWAY            AVS_COAP_CODE(5, 2)
#define AVS_COAP_CODE_SERVICE_UNAVAILABLE    AVS_COAP_CODE(5, 3)
#define AVS_COAP_CODE_GATEWAY_TIMEOUT        AVS_COAP_CODE(5, 4)
#define AVS_COAP_CODE_PROXYING_NOT_SUPPORTED AVS_COAP_CODE(5, 5)
/** @} */

/**
 * Converts CoAP code to a human-readable form.
 *
 * @param code     CoAP code to convert.
 * @param buf      Buffer to store the code string in.
 * @param buf_size @p buf capacity, in bytes.
 *
 * @returns @p buf on success, a pointer to a implementation-defined constant
 *          string if @p code is unknown or @p buf is too small.
 */
const char *
avs_coap_msg_code_to_string(uint8_t code, char *buf, size_t buf_size);

/**
 * Convenience macro that calls @ref avs_coap_msg_code_to_string with
 * a stack-allocated buffer big enough to store any CoAP code string.
 */
#define AVS_COAP_CODE_STRING(Code) \
        avs_coap_msg_code_to_string((Code), &(char[32]){0}[0], 32)

/** @{
 * CoAP code class/detail accessors. See RFC7252 for details.
 */
uint8_t avs_coap_msg_code_get_class(uint8_t code);
void avs_coap_msg_code_set_class(uint8_t *code, uint8_t cls);
uint8_t avs_coap_msg_code_get_detail(uint8_t code);
void avs_coap_msg_code_set_detail(uint8_t *code, uint8_t detail);
/** @} */

/**
 * @returns true if @p code belongs to a "client error" code class,
 *          false otherwise.
 */
static inline bool avs_coap_msg_code_is_client_error(uint8_t code) {
    return avs_coap_msg_code_get_class(code) == 4;
}

/**
 * @returns true if @p code belongs to a "server error" code class,
 *          false otherwise.
 */
static inline bool avs_coap_msg_code_is_server_error(uint8_t code) {
    return avs_coap_msg_code_get_class(code) == 5;
}

/**
 * @returns true if @p code represents a request, false otherwise.
 *          Note: Empty (0.00) is NOT considered a request. See RFC7252
 *          for details.
 */
static inline bool avs_coap_msg_code_is_request(uint8_t code) {
    return avs_coap_msg_code_get_class(code) == 0
            && avs_coap_msg_code_get_detail(code) > 0;
}

/** @returns true if @p code represents a response, false otherwise. */
static inline bool avs_coap_msg_code_is_response(uint8_t code) {
    return avs_coap_msg_code_get_class(code) > 0;
}

/** Serialized CoAP message header. For internal use only. */
typedef struct avs_coap_msg_header {
    uint8_t version_type_token_length;
    uint8_t code;
    uint8_t message_id[2];
} avs_coap_msg_header_t;

/** @{
 * Sanity checks that ensure no padding is inserted anywhere inside
 * @ref avs_coap_msg_header_t .
 */
AVS_STATIC_ASSERT(offsetof(avs_coap_msg_header_t, version_type_token_length) == 0,
                  vttl_field_is_at_start_of_msg_header_t);
AVS_STATIC_ASSERT(offsetof(avs_coap_msg_header_t, code) == 1,
                  no_padding_before_code_field_of_msg_header_t);
AVS_STATIC_ASSERT(offsetof(avs_coap_msg_header_t, message_id) == 2,
                  no_padding_before_message_id_field_of_msg_header_t);
AVS_STATIC_ASSERT(sizeof(avs_coap_msg_header_t) == 4,
                  no_padding_in_msg_header_t);
/** @} */

/** @{
 * Internal macros used for retrieving CoAP message type from
 * @ref avs_coap_msg_header_t .
 */
#define AVS_COAP_HEADER_TYPE_MASK 0x30
#define AVS_COAP_HEADER_TYPE_SHIFT 4
/** @} */

/** @returns CoAP message type encoded in @p hdr . */
avs_coap_msg_type_t
avs_coap_msg_header_get_type(const avs_coap_msg_header_t *hdr);

/** Sets CoAP message type of @p hdr to @p type . */
void avs_coap_msg_header_set_type(avs_coap_msg_header_t *hdr,
                                  avs_coap_msg_type_t type);

/** @returns Length, in bytes, of the CoAP message token encoded in @p hdr. */
uint8_t avs_coap_msg_header_get_token_length(const avs_coap_msg_header_t *hdr);

/** CoAP message object. */
typedef struct avs_coap_msg {
    /**
     * Length of the whole message (header + content). Does not include the
     * length field itself.
     */
    uint32_t length; // whole message (header + content)

    /** CoAP message header. */
    avs_coap_msg_header_t header;

    /**
     * Not really a single-element array, but a FAM; rest of the CoAP
     * message: token + options + payload.
     */
    uint8_t content[1];
} avs_coap_msg_t;

/** @{
 * Sanity checks ensuring no padding is inserted in @ref avs_coap_msg_t .
 */
AVS_STATIC_ASSERT(offsetof(avs_coap_msg_t, header) == 4,
                  no_padding_before_header_field_of_msg_t);
AVS_STATIC_ASSERT(offsetof(avs_coap_msg_t, content) == 8,
                  no_padding_before_content_field_of_msg_t);
/** @} */

/**
 * Iterator object used to access CoAP message options.
 *
 * Its fields should not be modified directly after initialization.
 * Use @ref avs_coap_opt_begin , @ref avs_coap_opt_next and
 * @ref avs_coap_opt_end instead.
 */
typedef struct {
    const avs_coap_msg_t *const msg;
    const avs_coap_opt_t *curr_opt;
    uint32_t prev_opt_number;
} avs_coap_opt_iterator_t;

/** Empty iterator object initializer. */
#define AVS_COAP_OPT_ITERATOR_EMPTY \
    (avs_coap_opt_iterator_t) { \
        NULL, NULL, 0 \
    }

/**
 * @param msg Message to retrieve ID from.
 * @returns Message ID in the host byte order.
 */
uint16_t avs_coap_msg_get_id(const avs_coap_msg_t *msg);

/**
 * @param msg Message to check
 * @return true if message is a request message (RFC7252 section 5.1),
 *      false otherwise
 */
static inline bool avs_coap_msg_is_request(const avs_coap_msg_t *msg) {
    return avs_coap_msg_code_is_request(msg->header.code);
}

/**
 * @param msg Message to check
 * @return true if message is a response message (RFC7252 section 5.1),
 *      false otherwise
 */
static inline bool avs_coap_msg_is_response(const avs_coap_msg_t *msg) {
    return avs_coap_msg_code_is_response(msg->header.code);
}

/**
 * @param msg       Message to retrieve token from.
 * @param out_token Buffer for the extracted token.
 * @returns Token length in bytes (0 <= length <= sizeof(avs_coap_token_t)).
 */
size_t avs_coap_msg_get_token(const avs_coap_msg_t *msg,
                              avs_coap_token_t *out_token);

/**
 * @param msg Message to retrieve identity from.
 * @returns @ref avs_coap_msg_identity_t object with message ID and token.
 */
static inline avs_coap_msg_identity_t
avs_coap_msg_get_identity(const avs_coap_msg_t *msg) {
    avs_coap_msg_identity_t id;
    memset(&id, 0, sizeof(id));
    id.msg_id = avs_coap_msg_get_id(msg);
    id.token_size = avs_coap_msg_get_token(msg, &id.token);
    return id;
}

/**
 * @returns true if CoAP token of @p msg matches one from @p id ,
 *          false otherwise.
 */
static inline bool
avs_coap_msg_token_matches(const avs_coap_msg_t *msg,
                           const avs_coap_msg_identity_t *id) {
    avs_coap_token_t msg_token;
    size_t msg_token_size = avs_coap_msg_get_token(msg, &msg_token);

    return avs_coap_token_equal(&msg_token, msg_token_size,
                                   &id->token, id->token_size);
}

/**
 * @param msg Message to iterate over.
 * @returns An CoAP Option iterator object.
 */
avs_coap_opt_iterator_t avs_coap_opt_begin(const avs_coap_msg_t *msg);

/**
 * Advances the @p optit iterator to the next CoAP Option.
 *
 * @param optit CoAP Option iterator to operate on.
 * @returns @p optit.
 */
avs_coap_opt_iterator_t *
avs_coap_opt_next(avs_coap_opt_iterator_t *optit);

/**
 * Checks if the @p optit points to the area after CoAP options list.
 *
 * @param optit Iterator to check.
 * @returns true if there are no more Options to iterate over (i.e. the iterator
 *          is invalidated), false if it points to a valid Option.
 */
bool avs_coap_opt_end(const avs_coap_opt_iterator_t *optit);

/**
 * @param optit Iterator to operate on.
 * @returns Number of the option currently pointed to by @p optit
 */
uint32_t avs_coap_opt_number(const avs_coap_opt_iterator_t *optit);

/**
 * Note: this function is NOT SAFE to use on invalid messages.
 *
 * @param msg Message to operate on.
 * @returns Pointer to the start of a message payload, or end-of-message if it
 *          does not contain payload.
 */
const void *avs_coap_msg_payload(const avs_coap_msg_t *msg);

/**
 * Note: this function is NOT SAFE to use on invalid messages.
 *
 * @param msg Message to operate on.
 * @returns Message payload size in bytes.
 */
size_t avs_coap_msg_payload_length(const avs_coap_msg_t *msg);

/**
 * @param msg Message to operate on.
 * @returns true if the message has a valid format, false otherwise.
 */
bool avs_coap_msg_is_valid(const avs_coap_msg_t *msg);

/**
 * Prints the @p msg content to standard output.
 *
 * @param msg Message to print.
 */
void avs_coap_msg_debug_print(const avs_coap_msg_t *msg);

/**
 * Prints a short summary of the @p msg to @p buf and returns a pointer
 * to @p buf.
 *
 * @param msg      Message to summarize.
 * @param buf      Buffer to put summary in.
 * @param buf_size @p buf capacity, in bytes.
 *
 * @returns @p buf.
 */
const char *
avs_coap_msg_summary(const avs_coap_msg_t *msg, char *buf, size_t buf_size);

/**
 * Convenience macro that calls @ref avs_coap_msg_summary with
 * a stack-allocated buffer big enough to store any summary.
 */
#define AVS_COAP_MSG_SUMMARY(Msg) \
        avs_coap_msg_summary((Msg), &(char[256]){0}[0], 256)

#ifdef __cplusplus
}
#endif

#endif // AVS_COMMONS_COAP_MSG_H
