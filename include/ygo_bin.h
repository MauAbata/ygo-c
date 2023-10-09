#ifndef __ygo_bin_h
#define __ygo_bin_h

#ifdef __cplusplus
extern "C" {
#endif

#include "../src/internals.h"
#include <stdint.h>

#define YGO_CARD_DATA_MAGIC_WORD { '\x0E', 'Y', 'G', 'O' }
#define YGO_CARD_DATA_VERSION 0x00

enum ygo_bin_errno {
    YGO_BIN_OK = 0x00,
    YGO_BIN_ERR_BAD_ARGS,
    YGO_BIN_ERR_BAD_MAGIC_WORD,
    YGO_BIN_ERR_BAD_CHECKSUM,
};

typedef enum ygo_bin_errno ygo_bin_errno_t;

/**
 * Selection of various record types we will handle when parsing Card data from binary
 * data files and streams.
 */
enum ygo_bin_record_type {
    BIN_RECORD_CARD_BASIC = 0x00,
    BIN_RECORD_CARD_DESCRIPTION = 0x01,
    BIN_RECORD_CARD_RULE_INDEX = 0x02,
    BIN_RECORD_CARD_IMAGE_CROPPED = 0x03,
};

typedef enum ygo_bin_record_type ygo_bin_record_type_t;

/**
 * Used internally when interfacing with raw binary data records. This header defines the data
 * that follows it. Serialized, it should be 4-bytes long, in the order shown below.
 */
struct _packed_ ygo_bin_record_header {
    ygo_bin_record_type_t record_type;
    uint8_t data_version;

    // Length of the data which follows. This excludes the header, but includes the checksum.
    uint16_t record_length;

    // Just a note on the comment above, the calculation to retroactively populate the
    // record_length field actually counts from the start of header to the start of checksum,
    // but because the header and checksum both occupy 4 bytes, we treat it as if it's the actual
    // payload size excluding header, including checksum.
};

typedef struct ygo_bin_record_header ygo_bin_record_header_t;

/**
 * Internal used context pointers to handle easier writing to packed binfiles.
 */
typedef struct {
    uint8_t *buffer;
    size_t ptr;
    size_t header_start;
    ygo_bin_record_header_t *header;
} ygo_bin_write_context_t;

/**
 * Internal used context pointers to handle easier writing to packed binfiles.
 */
typedef struct {
    const uint8_t *buffer;
    size_t ptr;
    size_t header_start;
    ygo_bin_record_header_t *header;
    ygo_bin_errno_t err;
} ygo_bin_read_context_t;

void ygo_bin_begin_data_write(ygo_bin_write_context_t *ctx, uint8_t *buffer);

ygo_bin_errno_t ygo_bin_begin_data_read(ygo_bin_read_context_t *ctx, const uint8_t *buffer);

void ygo_bin_write_magic_word(ygo_bin_write_context_t *ctx);

ygo_bin_errno_t ygo_bin_check_magic_word(ygo_bin_read_context_t *ctx);

void ygo_bin_write_record_header(ygo_bin_write_context_t *ctx, ygo_bin_record_header_t *header);

ygo_bin_errno_t ygo_bin_read_record_header(ygo_bin_read_context_t *ctx, ygo_bin_record_header_t *header);

void ygo_bin_write_record_end(ygo_bin_write_context_t *ctx);

ygo_bin_errno_t ygo_bin_check_record_end(ygo_bin_read_context_t *ctx);

void ygo_bin_write_int(ygo_bin_write_context_t *ctx, uint32_t value, size_t n);

// TODO: This should actually be a pointer, so we can check errno on return. Context also holds
//       errno in an attempt to allow this bad type conversion to continue, but that makes checking
//       more complicated. Boo.
ygo_bin_errno_t ygo_bin_read_int(ygo_bin_read_context_t *ctx, uintmax_t *dest, size_t n);
ygo_bin_errno_t ygo_bin_read_enum(ygo_bin_read_context_t *ctx, int *dest, size_t n);
ygo_bin_errno_t ygo_bin_read_int8(ygo_bin_read_context_t *ctx, uint8_t *dest);
ygo_bin_errno_t ygo_bin_read_int16(ygo_bin_read_context_t *ctx, uint16_t *dest);
ygo_bin_errno_t ygo_bin_read_int32(ygo_bin_read_context_t *ctx, uint32_t *dest);

void ygo_bin_write_bytes(ygo_bin_write_context_t *ctx, const uint8_t *data, size_t n);

/**
 * This writes a string into the packed buffer. Length is provided and will ensure the string
 * occupies this many bytes of storage. The target data block will be null-padded past the first
 * null character in the incoming string.
 * @warning This will not ensure null termination in the resulting blob.
 * @param ctx
 * @param str
 * @param len
 */
void ygo_bin_write_str(ygo_bin_write_context_t *ctx, const char *str, size_t len);

ygo_bin_errno_t ygo_bin_read_str(ygo_bin_read_context_t *ctx, const char *str, size_t len);



#ifdef __cplusplus
}
#endif

#endif
