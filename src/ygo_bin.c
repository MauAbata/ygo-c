#include "ygo_bin.h"
#include <stdio.h>

static uint8_t _ygo_magic_word[] = YGO_CARD_DATA_MAGIC_WORD;

#ifndef YGO_USE_SLOW_CRC
static uint16_t crc16_tab[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741,
    0x0500, 0xc5c1, 0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941,
    0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 0xd201, 0x12c0, 0x1380, 0xd341,
    0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40,
    0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41,
    0x2d00, 0xedc1, 0xec81, 0x2c40, 0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141,
    0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0, 0x6b80, 0xab41,
    0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541,
    0x7700, 0xb7c1, 0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741,
    0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841, 0x8801, 0x48c0, 0x4980, 0x8941,
    0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341,
    0x4100, 0x81c1, 0x8081, 0x4040};
#endif

uint16_t ygo_bin_calculate_crc(const uint8_t *buffer, size_t size) {
    if (buffer == NULL) return 0x0000;
    const uint8_t *ptr = buffer;
    uint16_t crc = 0x01;

    while (size--) {
#ifndef YGO_USE_SLOW_CRC
        // So, the explicit casts to unsigned in here are because introspection looks into the
        // integer promotion of these uint16_t types, which is signed, and then complains that
        // there is a signed operand in binary operations. That's also why all the numeric constant
        // literals are declared (superfluously) as unsigned, because apparently without that,
        // clang-tidy assumes it's possible for a positive integer constant to be negative.
        //
        // I love my job, I love my job, I love my job...
        //
        crc = crc16_tab[((unsigned)(crc ^ (*ptr++))) & 0xFFu] ^ (unsigned)(crc >> 8u);
#else
        // Slow CRC-16 calculation (bit-by-bit, no lookup table)
        // Saves 512 bytes of RAM on constrained microcontrollers
        uint8_t byte = *ptr++;
        crc ^= byte;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x0001u) {
                crc = (crc >> 1u) ^ 0xA001u; // CRC-16-ANSI polynomial
            } else {
                crc = crc >> 1u;
            }
        }
#endif
    }

    return crc;
}

ygo_bin_errno_t ygo_bin_read_int8(ygo_bin_read_context_t *ctx, uint8_t *dest) {
    if (ctx == NULL || ctx->buffer == NULL) return YGO_BIN_ERR_BAD_ARGS;
    *dest = ctx->buffer[ctx->ptr++];
    return YGO_BIN_OK;
}

ygo_bin_errno_t ygo_bin_read_int16(ygo_bin_read_context_t *ctx, uint16_t *dest) {
    if (ctx == NULL || ctx->buffer == NULL) return YGO_BIN_ERR_BAD_ARGS;
    uint16_t val = (uint16_t)ctx->buffer[ctx->ptr++] << 8;
    val |= ctx->buffer[ctx->ptr++];
    *dest = val;
    return YGO_BIN_OK;
}

ygo_bin_errno_t ygo_bin_read_int32(ygo_bin_read_context_t *ctx, uint32_t *dest) {
    if (ctx == NULL || ctx->buffer == NULL) return YGO_BIN_ERR_BAD_ARGS;
    uint32_t val = (uint32_t)ctx->buffer[ctx->ptr++] << 24;
    val |= (uint32_t)ctx->buffer[ctx->ptr++] << 16;
    val |= (uint32_t)ctx->buffer[ctx->ptr++] << 8;
    val |= ctx->buffer[ctx->ptr++];
    *dest = val;
    return YGO_BIN_OK;
}

ygo_bin_errno_t ygo_bin_read_enum(ygo_bin_read_context_t *ctx, int *dest, size_t n) {
    if (n == 1) {
        uint8_t val;
        ygo_bin_errno_t err = ygo_bin_read_int8(ctx, &val);
        *dest = (int)val;
        return err;
    } else if (n == 2) {
        uint16_t val;
        ygo_bin_errno_t err = ygo_bin_read_int16(ctx, &val);
        *dest = (int)val;
        return err;
    }
    return YGO_BIN_ERR_BAD_ARGS;
}

ygo_bin_errno_t ygo_bin_read_str(ygo_bin_read_context_t *ctx, char *str, size_t len) {
    if (ctx == NULL || ctx->buffer == NULL || str == NULL) return YGO_BIN_ERR_BAD_ARGS;

    // Read len bytes from buffer into string
    for (size_t i = 0; i < len; i++) {
        str[i] = ctx->buffer[ctx->ptr++];
    }

    return YGO_BIN_OK;
}

void ygo_bin_write_bytes(ygo_bin_write_context_t *ctx, const uint8_t *data, size_t n) {
    if (ctx == NULL || data == NULL) return;
    for (size_t i = 0; i < n; i++) {
        if (ctx->buffer != NULL) ctx->buffer[ctx->ptr] = data[i];
        ctx->ptr++;
    }
}

void ygo_bin_write_int8(ygo_bin_write_context_t *ctx, uint8_t value) {
    if (ctx == NULL) return;
    if (ctx->buffer != NULL) ctx->buffer[ctx->ptr] = value;
    ctx->ptr++;
}

void ygo_bin_write_int16(ygo_bin_write_context_t *ctx, uint16_t value) {
    if (ctx == NULL) return;
    if (ctx->buffer != NULL) {
        ctx->buffer[ctx->ptr] = (value >> 8);
        ctx->buffer[ctx->ptr + 1] = value;
    }
    ctx->ptr += 2;
}

void ygo_bin_write_int32(ygo_bin_write_context_t *ctx, uint32_t value) {
    if (ctx == NULL) return;
    if (ctx->buffer != NULL) {
        ctx->buffer[ctx->ptr] = (value >> 24);
        ctx->buffer[ctx->ptr + 1] = (value >> 16);
        ctx->buffer[ctx->ptr + 2] = (value >> 8);
        ctx->buffer[ctx->ptr + 3] = value;
    }
    ctx->ptr += 4;
}

void ygo_bin_write_str(ygo_bin_write_context_t *ctx, const char *str, size_t len) {
    if (ctx == NULL) return;
    size_t i = 0;

    // Copy the string and break on first null character.
    for (; i < len; i++) {
        if (ctx->buffer != NULL) ctx->buffer[ctx->ptr] = str[i];
        ctx->ptr++;
        if (str[i] == '\0') break;
    }

    // Fill the rest (if any) with null.
    for (; i < len; i++) {
        if (ctx->buffer != NULL) ctx->buffer[ctx->ptr] = 0x00;
        ctx->ptr++;
    }
}

void ygo_bin_begin_data_write(ygo_bin_write_context_t *ctx, uint8_t *buffer) {
    if (ctx == NULL) return;
    ctx->buffer = buffer;
    ctx->ptr = 0;
    ctx->header = NULL;
    ctx->header_start = 0;
}

ygo_bin_errno_t ygo_bin_begin_data_read(ygo_bin_read_context_t *ctx, const uint8_t *buffer) {
    if (ctx == NULL) return YGO_BIN_ERR_BAD_ARGS;
    ctx->buffer = buffer;
    ctx->ptr = 0;
    ctx->header = NULL;
    ctx->header_start = 0;
    return YGO_BIN_OK;
}

void ygo_bin_write_magic_word(ygo_bin_write_context_t *ctx) {
    if (ctx == NULL) return;
    ygo_bin_write_bytes(ctx, _ygo_magic_word, 4);
}

void ygo_bin_write_record_header(ygo_bin_write_context_t *ctx, ygo_bin_record_header_t *header) {
    if (ctx == NULL) return;
    if (ctx->header != NULL) ygo_bin_write_record_end(ctx);

    ctx->header_start = ctx->ptr;
    ygo_bin_write_int8(ctx, header->record_type);
    ygo_bin_write_int8(ctx, header->data_version);
    ygo_bin_write_int16(ctx, 0x0000);
}

ygo_bin_errno_t ygo_bin_read_record_header(ygo_bin_read_context_t *ctx,
                                           ygo_bin_record_header_t *header) {
    if (ctx == NULL) return YGO_BIN_ERR_BAD_ARGS;
    if (ctx->header != NULL) ygo_bin_check_record_end(ctx);

    ctx->header_start = ctx->ptr;
    ygo_bin_read_enum(ctx, &header->record_type, 1);
    ygo_bin_read_int8(ctx, &header->data_version);
    ygo_bin_read_int16(ctx, &header->record_length);
    return YGO_BIN_OK;
}

void ygo_bin_write_record_end(ygo_bin_write_context_t *ctx) {
    if (ctx == NULL) return;
    uint16_t crc16 = 0x0000;

    // Pad the data up to 4 byte blocks:
    while ((ctx->ptr - ctx->header_start) % 4 != 0) {
        ygo_bin_write_int8(ctx, 0x00);
    }

    // We can actually skip the calculations if we're not writing to the buffer.
    if (ctx->buffer != NULL) {
        // Alter record header with current length:
        uint16_t block_len = (ctx->ptr - ctx->header_start);
        ctx->buffer[ctx->header_start + 2] = (block_len >> 8u);
        ctx->buffer[ctx->header_start + 3] = (block_len >> 0u);

        // Append Checksum Bytes:
        crc16 = ygo_bin_calculate_crc(ctx->buffer + ctx->header_start, block_len);
    }

    ygo_bin_write_int16(ctx, crc16);
    ygo_bin_write_int16(ctx, 0x0000); // unused data.
}

ygo_bin_errno_t ygo_bin_check_record_end(ygo_bin_read_context_t *ctx) {
    if (ctx == NULL || ctx->buffer == NULL) return YGO_BIN_ERR_BAD_ARGS;

    // Pad to 4-byte alignment
    while ((ctx->ptr - ctx->header_start) % 4 != 0) {
        ctx->ptr++;
    }

    // Read checksum (2 bytes)
    uint16_t stored_crc;
    ygo_bin_errno_t err = ygo_bin_read_int16(ctx, &stored_crc);
    if (err != YGO_BIN_OK) return err;

    // Skip unused data (2 bytes)
    ctx->ptr += 2;

    // Calculate and verify checksum
    uint16_t block_len = (ctx->ptr - ctx->header_start) - 4; // Exclude checksum+unused
    uint16_t calc_crc = ygo_bin_calculate_crc(ctx->buffer + ctx->header_start, block_len);

    if (calc_crc != stored_crc) {
        return YGO_BIN_ERR_BAD_CHECKSUM;
    }

    return YGO_BIN_OK;
}
