#include "ygo_card.h"
#include <string.h>
#include <stddef.h>

uint32_t _read_int(uint8_t **buf, size_t n) {
    uint32_t ret = 0;

    for (size_t i = 0; i < n; i++) {
        ret <<= 8;
        uint8_t byte = **buf;
        ret |= byte;
        *buf += 1;
    }

    return ret;
}

#define read(mbr, sz) mbr = _read_int(&ptr, sz)

void _write_int(uint8_t **buf, size_t n, const uint32_t value) {
    for (size_t i = n; i > 0; i--) {
        uint8_t byte = value >> ((i - 1) * 8);
        **buf = byte;
        *buf += 1;
    }
}

#define write(mbr, sz) _write_int(&ptr, sz, mbr)

int ygo_card_serialize(uint8_t *buffer, const ygo_card_t *card) {
    uint8_t *ptr = buffer;

    write(card->id, 4);
    write(card->type, 1);
    write(card->frame, 1);
    write(card->subtype, 1);
    write(card->attribute, 1);
    write(card->atk, 2);
    write(card->def, 2);
    write(card->rank, 1);
    write(card->scale, 1);
    write(card->link_value, 1);
    write(card->link_markers, 1);

    memcpy(ptr, card->name, YGO_CARD_NAME_MAX_LEN);
    ptr += YGO_CARD_NAME_MAX_LEN;

    // Fill the reserved section
    memset(ptr, 0xFF, sizeof(card->_reserved));

    return 0;
}

int ygo_card_deserialize(ygo_card_t *card, uint8_t *const buffer) {
    uint8_t *ptr = buffer;

    read(card->id, 4);
    read(card->type, 1);
    read(card->frame, 1);
    read(card->subtype, 1);
    read(card->attribute, 1);
    read(card->atk, 2);
    read(card->def, 2);
    read(card->rank, 1);
    read(card->scale, 1);
    read(card->link_value, 1);
    read(card->link_markers, 1);

    memcpy(card->name, ptr, YGO_CARD_NAME_MAX_LEN);
    ptr += YGO_CARD_NAME_MAX_LEN;

    return 0;
}
