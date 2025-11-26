#include "ygo_card.h"
#include "ygo_bin.h"
#include <stdio.h>
#include <ygo_card.h>

ENUM_IMPL(ygo_card_type, YGO_CARD_TYPE_DEFS);
ENUM_IMPL_BITS(ygo_monster_flag, YGO_MONSTER_FLAG_DEFS);
ENUM_IMPL(ygo_monster_ability, YGO_MONSTER_ABILITY_DEFS);
ENUM_IMPL(ygo_summon_type, YGO_SUMMON_TYPE_DEFS);
ENUM_IMPL(ygo_monster_type, YGO_MONSTER_TYPE_DEFS);
ENUM_IMPL(ygo_spell_type, YGO_SPELL_TYPE_DEFS);
ENUM_IMPL(ygo_trap_type, YGO_TRAP_TYPE_DEFS);
ENUM_IMPL(ygo_attribute, YGO_ATTRIBUTE_DEFS);
ENUM_IMPL_BITS(ygo_card_link_markers, YGO_CARD_LINK_MARKERS_DEFS);

size_t ygo_card_serialize(uint8_t *buffer, const ygo_card_t *card) {
    ygo_bin_write_context_t ctx;
    ygo_bin_begin_data_write(&ctx, buffer);
    ygo_bin_write_magic_word(&ctx);

    // Card Record Header
    ygo_bin_record_header_t header = {
        .record_type = BIN_RECORD_CARD_BASIC,
        .data_version = YGO_CARD_DATA_VERSION,
        .record_length = 0x0000,
    };

    ygo_bin_write_record_header(&ctx, &header);

    // Write Card Data
    ygo_bin_write_int32(&ctx, card->id);

    uint8_t type1 = (((unsigned)card->type) | ((unsigned)card->flags) | ((unsigned)card->ability));

    ygo_bin_write_int8(&ctx, type1);

    uint8_t type0 = (((unsigned)card->summon) | ((unsigned)card->monster_type));

    ygo_bin_write_int8(&ctx, type0);

    // Unused, to align Attribute Start
    ygo_bin_write_int16(&ctx, 0x0000);

    ygo_bin_write_int16(&ctx, card->atk);
    ygo_bin_write_int16(&ctx, card->def);
    ygo_bin_write_int8(&ctx, card->level);
    ygo_bin_write_int8(&ctx, card->attribute);
    ygo_bin_write_int8(&ctx, card->scale);
    ygo_bin_write_int8(&ctx, card->link_markers);

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

    uint8_t type1 = 0x00;
    ygo_bin_read_int8(&ctx, &type1);
    card->type = GET_CARD_TYPE(type1);
    card->flags = GET_MONSTER_FLAG(type1);
    card->ability = GET_ABILITY(type1);

    uint8_t type0 = 0x00;
    ygo_bin_read_int8(&ctx, &type0);
    card->summon = GET_SUMMON_TYPE(type0);
    card->monster_type = GET_MONSTER_TYPE(type0);

    ygo_bin_read_enum(&ctx, &card->attribute, 1);
    ygo_bin_read_int16(&ctx, &card->atk);
    ygo_bin_read_int16(&ctx, &card->def);
    ygo_bin_read_int8(&ctx, &card->level);
    ygo_bin_read_int8(&ctx, &card->scale);
    ygo_bin_read_int8(&ctx, &card->link_value);
    ygo_bin_read_enum(&ctx, &card->link_markers, 1);
    ygo_bin_read_str(&ctx, card->name, YGO_CARD_NAME_MAX_LEN);

    ygo_bin_check_record_end(&ctx);
    return ctx.ptr;
}

void ygo_card_print(ygo_card_t *card) {
    printf("ID: %d\n", card->id);
    printf("Name: %s\n", card->name);

    if (card->type == YGO_CARD_TYPE_MONSTER) {
        printf("Type: 0x%x%x - %s ",
               (unsigned)card->type | (unsigned)card->flags | (unsigned)card->ability,
               (unsigned)card->summon | (unsigned)card->monster_type,
               ygo_monster_type_to_str(card->monster_type));

        for (unsigned int i = 0; i < 4; i++) {
            unsigned int mask = 1u << (3u + i);
            if ((unsigned)card->flags & mask) {
                printf("%s ", ygo_monster_flag_to_str((unsigned)card->flags & mask));
            }
        }

        if (card->ability > 0) {
            printf("%s ", ygo_monster_ability_to_str(card->ability));
        }

        if (card->summon > 0) {
            printf("%s ", ygo_summon_type_to_str(card->summon));
        }

        printf("%s\n", ygo_card_type_to_str(card->type));

        printf("Attribute: 0x%x - %s\n", card->attribute, ygo_attribute_to_str(card->attribute));
        printf("ATK: %d\n", card->atk);

        if (card->summon != YGO_SUMMON_TYPE_LINK) {
            printf("DEF: %d\n", card->def);
            printf("Level: %d\n", card->level);
        }

        if ((unsigned)card->flags & (unsigned)YGO_MONSTER_FLAG_PENDULUM) {
            printf("Scale: %d\n", card->scale);
        }

        if (card->summon == YGO_SUMMON_TYPE_LINK) {
            printf("Link Value: %d\n", card->link_value);
            printf("Link Markers: 0x%x - ", card->link_markers);

            for (unsigned int i = 0; i < 8; i++) {
                unsigned int lm = 1u << i;

                if ((unsigned)card->link_markers & lm) {
                    printf("%s, ", ygo_card_link_markers_to_str(lm));
                }
            }

            printf("\n");
        }
    }
}
