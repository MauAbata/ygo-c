#include "ygo_card.h"
#include "ygo_bin.h"
#include <stdio.h>

ENUM_IMPL(ygo_card_type_generic, YGO_CARD_TYPE_GENERIC_DEFS);
ENUM_IMPL(ygo_card_type, YGO_CARD_TYPE_DEFS);
ENUM_IMPL(ygo_card_frame, YGO_CARD_FRAME_DEFS);
ENUM_IMPL(ygo_card_subtype, YGO_CARD_SUBTYPE_DEFS);
ENUM_IMPL(ygo_card_attribute, YGO_CARD_ATTRIBUTE_DEFS);
ENUM_IMPL_BITS(ygo_card_link_markers, YGO_CARD_LINK_MARKERS_DEFS);

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
    ygo_bin_read_int32(&ctx, &card->id);
    ygo_bin_read_enum(&ctx, &card->type, 1);
    ygo_bin_read_enum(&ctx, &card->frame, 1);
    ygo_bin_read_enum(&ctx, &card->subtype, 1);
    ygo_bin_read_enum(&ctx, &card->attribute, 1);
    ygo_bin_read_int16(&ctx, &card->atk);
    ygo_bin_read_int16(&ctx, &card->def);
    ygo_bin_read_int8(&ctx, &card->rank);
    ygo_bin_read_int8(&ctx, &card->scale);
    ygo_bin_read_int8(&ctx, &card->link_value);
    ygo_bin_read_enum(&ctx, &card->link_markers, 1);
//    ygo_bin_read_str(&ctx, card->name, YGO_CARD_NAME_MAX_LEN);

//    ygo_bin_check_record_end(&ctx);
    return ctx.ptr;
}

void ygo_card_print(ygo_card_t *card) {
    printf("ID: %d\n", card->id);
    printf("Name: %s\n", card->name);
    printf("Type: %x - %s\n", card->type, ygo_card_type_to_str(card->type));
    printf("Frame: %x - %s\n", card->frame, ygo_card_frame_to_str(card->frame));
    printf("Subtype: %x - %s\n", card->subtype, ygo_card_subtype_to_str(card->subtype));
    printf("Attribute: %x - %s\n", card->attribute, ygo_card_attribute_to_str(card->attribute));
    printf("ATK: %d\n", card->atk);
    printf("DEF: %d\n", card->def);
    printf("Rank: %d\n", card->rank);
    printf("Scale: %d\n", card->scale);
    printf("Link Value: %d\n", card->link_value);
    printf("Link Markers: %x\n", card->link_markers);
}
