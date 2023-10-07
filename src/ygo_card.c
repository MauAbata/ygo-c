#include "ygo_card.h"
#include "ygo_bin.h"
#include <stdio.h>

size_t ygo_card_serialize(uint8_t *buffer, const ygo_card_t *card) {
    ygo_bin_write_context_t ctx;
    ygo_bin_begin_data_write(&ctx, buffer);
    ygo_bin_write_magic_word(&ctx);

    // Card Record Header
    ygo_bin_record_header_t header = {
        .record_type = BIN_RECORD_CARD_BASIC,
        .data_version = YGO_CARD_DATA_VERSION,
        .record_length = 0x00,
    };

    ygo_bin_write_record_header(&ctx, &header);

    // Write Card Data
    ygo_bin_write_int(&ctx, card->id, 4);
    ygo_bin_write_int(&ctx, card->type, 1);
    ygo_bin_write_int(&ctx, card->frame, 1);
    ygo_bin_write_int(&ctx, card->subtype, 1);
    ygo_bin_write_int(&ctx, card->attribute, 1);
    ygo_bin_write_int(&ctx, card->atk, 2);
    ygo_bin_write_int(&ctx, card->def, 2);
    ygo_bin_write_int(&ctx, card->rank, 1);
    ygo_bin_write_int(&ctx, card->scale, 1);
    ygo_bin_write_int(&ctx, card->link_value, 1);
    ygo_bin_write_int(&ctx, card->link_markers, 1);
    ygo_bin_write_str(&ctx, card->name, YGO_CARD_NAME_MAX_LEN);

    ygo_bin_write_record_end(&ctx);
    return ctx.ptr;
}

size_t ygo_card_deserialize(ygo_card_t *card, const uint8_t *buffer) {
    ygo_bin_read_context_t ctx;
    ygo_bin_begin_data_read(&ctx, buffer);

    ygo_bin_record_header_t header;
    ygo_bin_read_record_header(&ctx, &header);

    // Read card data.
//    card->id = ygo_bin_read_int(&ctx, 4);
//    card->type = ygo_bin_read_int(&ctx, 1);
//    card->frame = ygo_bin_read_int(&ctx, 1);
//    card->subtype = ygo_bin_read_int(&ctx, 1);
//    card->attribute = ygo_bin_read_int(&ctx, 1);
//    card->atk = ygo_bin_read_int(&ctx, 2);
//    card->def = ygo_bin_read_int(&ctx, 2);
//    card->rank = ygo_bin_read_int(&ctx, 1);
//    card->scale = ygo_bin_read_int(&ctx, 1);
//    card->link_value = ygo_bin_read_int(&ctx, 1);
//    card->link_markers = ygo_bin_read_int(&ctx, 1);
//    ygo_bin_read_str(&ctx, card->name, YGO_CARD_NAME_MAX_LEN);

//    ygo_bin_check_record_end(&ctx);
    return ctx.ptr;
}
