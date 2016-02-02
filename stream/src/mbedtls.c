/*
 * AVSystem Commons Library
 *
 * Copyright (C) 2014-2016 AVSystem <http://www.avsystem.com/>
 *
 * This code is free and open source software licensed under the MIT License.
 * See the LICENSE file for details.
 */

#include <config.h>

#include <stdlib.h>

#include <mbedtls/md5.h>

#include <avsystem/commons/stream/md5.h>

#include "md5_common.h"

#ifdef HAVE_VISIBILITY
#pragma GCC visibility push(hidden)
#endif

typedef struct {
    avs_stream_md5_common_t common;
    mbedtls_md5_context ctx;
} mbedtls_md5_stream_t;

static int unimplemented() {
    return -1;
}

static int avs_md5_finish(avs_stream_abstract_t *stream) {
    mbedtls_md5_stream_t *str = (mbedtls_md5_stream_t *) stream;

    mbedtls_md5_finish(&str->ctx, str->common.result);
    _avs_stream_md5_common_finalize(&str->common);

    return 0;
}

static int avs_md5_reset(avs_stream_abstract_t *stream) {
    mbedtls_md5_stream_t *str = (mbedtls_md5_stream_t *) stream;

    if (!_avs_stream_md5_common_is_finalized(&str->common)) {
        avs_md5_finish(stream);
    }
    mbedtls_md5_starts(&str->ctx);
    _avs_stream_md5_common_reset(&str->common);
    return 0;
}

static int avs_md5_update(avs_stream_abstract_t *stream,
                          const void *buf,
                          size_t len) {
    mbedtls_md5_stream_t * str = (mbedtls_md5_stream_t *) stream;

    if (_avs_stream_md5_common_is_finalized(&str->common)) {
        return -1;
    }

    mbedtls_md5_update(&str->ctx, (const unsigned char *) buf, len);
    return 0;
}

static const avs_stream_v_table_t md5_vtable = {
    avs_md5_update,
    avs_md5_finish,
    _avs_stream_md5_common_read,
    (avs_stream_peek_t) unimplemented,
    avs_md5_reset,
    avs_md5_finish,
    (avs_stream_errno_t) unimplemented,
    AVS_STREAM_V_TABLE_NO_EXTENSIONS
};

avs_stream_abstract_t *avs_stream_md5_create(void) {
    mbedtls_md5_stream_t *retval =
            (mbedtls_md5_stream_t *) malloc(sizeof (mbedtls_md5_stream_t));
    if (retval) {
        _avs_stream_md5_common_init(&retval->common, &md5_vtable);
        mbedtls_md5_init(&retval->ctx);
        mbedtls_md5_starts(&retval->ctx);
    }
    return (avs_stream_abstract_t *) retval;
}
