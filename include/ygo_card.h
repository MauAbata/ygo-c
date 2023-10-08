#ifndef __ygo_card_h
#define __ygo_card_h

#include <stdint.h>
#include "internals.h"

#define YGO_CARD_TYPE_GENERIC_DEFS(X, V) \
    V(YGO_CARD_TYPE_MONSTER, "Monster", 0x01) \
    V(YGO_CARD_TYPE_SPELL, "Spell", 0x02) \
    V(YGO_CARD_TYPE_TRAP, "Trap", 0x03) \
    V(YGO_CARD_TYPE_EXTRA, "Extra", 0x04) \
    V(YGO_CARD_TYPE_SKILL, "Skill", 0x05) \
    V(YGO_CARD_TYPE_TOKEN, "Token", 0x06)

/**
 * Generic card type, which is bitmasked from either ygo_card_type or ygo_card_subtype structs.
 */
ENUM_DECL(ygo_card_type_generic, YGO_CARD_TYPE_GENERIC_DEFS);

// This bitmask is used in ygo_card_type and ygo_card_subtype to identify the overall generic type
// of the card.
#define YGO_CARD_TYPE_GROUP_BITS (0x07 << 5)

// Infers a generic type from the more detailed ygo_card_type and ygo_card_subtype values.
#define get_generic_type(type) ((d & YGO_CARD_TYPE_GROUP_BITS) >> 5)

// This is madness and would absolutely be better expressed as a bitfield.
#define YGO_CARD_TYPE_DEFS(X, V) \
    /* Main Deck Type */\
    V(YGO_CARD_TYPE_NORMAL_MONSTER, "Normal Monster", YGO_CARD_TYPE_MONSTER << 5u) \
    X(YGO_CARD_TYPE_EFFECT_MONSTER, "Effect Monster") \
    X(YGO_CARD_TYPE_FLIP_EFFECT_MONSTER, "Flip Effect Monster") \
    X(YGO_CARD_TYPE_FLIP_TUNER_EFFECT_MONSTER, "Flip Tuner Effect Monster") \
    X(YGO_CARD_TYPE_GEMINI_MONSTER, "Gemini Monster") \
    X(YGO_CARD_TYPE_NORMAL_TUNER_MONSTER, "Normal Tuner Monster") \
    X(YGO_CARD_TYPE_PENDULUM_EFFECT_MONSTER, "Pendulum Effect Monster") \
    X(YGO_CARD_TYPE_PENDULUM_EFFECT_RITUAL_MONSTER, "Pendulum Effect Ritual Monster") \
    X(YGO_CARD_TYPE_PENDULUM_FLIP_EFFECT_MONSTER, "Pendulum Flip Effect Monster") \
    X(YGO_CARD_TYPE_PENDULUM_NORMAL_MONSTER, "Pendulum Normal Monster") \
    X(YGO_CARD_TYPE_PENDULUM_TUNER_EFFECT_MONSTER, "Pendulum Tuner Effect Monster") \
    X(YGO_CARD_TYPE_RITUAL_EFFECT_MONSTER, "Ritual Effect Monster") \
    X(YGO_CARD_TYPE_RITUAL_MONSTER, "Ritual Monster") \
    X(YGO_CARD_TYPE_SPIRIT_MONSTER, "Spirit Monster") \
    X(YGO_CARD_TYPE_TOON_MONSTER, "Toon Monster") \
    X(YGO_CARD_TYPE_TUNER_MONSTER, "Tuner Monster") \
    X(YGO_CARD_TYPE_UNION_EFFECT_MONSTER, "Union Effect Monster") \
    \
    /* Non-Monster Main */\
    V(YGO_CARD_TYPE_SPELL_CARD, "Spell Card", YGO_CARD_TYPE_SPELL << 5u) \
    V(YGO_CARD_TYPE_TRAP_CARD, "Trap Card",  YGO_CARD_TYPE_TRAP << 5u) \
    \
    /* Extra Deck Types */\
    V(YGO_CARD_TYPE_FUSION_MONSTER, "Fusion Monster", YGO_CARD_TYPE_EXTRA << 5u) \
    X(YGO_CARD_TYPE_LINK_MONSTER, "Link Monster") \
    X(YGO_CARD_TYPE_PENDULUM_EFFECT_FUSION_MONSTER, "Pendulum Effect Fusion Monster") \
    X(YGO_CARD_TYPE_SYNCHRO_MONSTER, "Synchro Monster") \
    X(YGO_CARD_TYPE_SYNCHRO_PENDULUM_EFFECT_MONSTER, "Synchro Pendulum Effect Monster") \
    X(YGO_CARD_TYPE_SYNCHRO_TUNER_MONSTER, "Synchro Tuner Monster") \
    X(YGO_CARD_TYPE_XYZ_MONSTER, "XYZ Monster") \
    X(YGO_CARD_TYPE_XYZ_PENDULUM_EFFECT_MONSTER, "XYZ Pendulum Effect Monster") \
    \
    /* Other Types */\
    V(YGO_CARD_TYPE_SKILL_CARD, "Skill Card", YGO_CARD_TYPE_SKILL << 5u) \
    V(YGO_CARD_TYPE_TOKEN_CARD, "Token Card", YGO_CARD_TYPE_TOKEN << 5u) \

/**
 * Main type of card (Monster, Effect, Etc.)
 * Thought should be put into this data format to more quickly parse the main type based on bitwise
 * operations. For ease in rendering, please look at the ygo_card_frame_t enum.
 *
 * Since there are *so* many monster types, we should actually enumerate
 * Monster/Spell/Trap/Extra/Other. Upper 3 bits = this grouping, lower 3 = monster subtype
 */
ENUM_DECL(ygo_card_type, YGO_CARD_TYPE_DEFS);

#define YGO_CARD_FRAME_DEFS(X, V) \
    X(YGO_CARD_FRAME_NORMAL, "Normal") \
    X(YGO_CARD_FRAME_EFFECT, "Effect") \
    X(YGO_CARD_FRAME_RITUAL, "Ritual") \
    X(YGO_CARD_FRAME_FUSION, "Fusion") \
    X(YGO_CARD_FRAME_SYNCHRO, "Synchro") \
    X(YGO_CARD_FRAME_XYZ, "Xyz") \
    X(YGO_CARD_FRAME_LINK, "Link") \
    X(YGO_CARD_FRAME_NORMAL_PENDULUM, "Normal Pendulum") \
    X(YGO_CARD_FRAME_EFFECT_PENDULUM, "Effect Pendulum") \
    X(YGO_CARD_FRAME_RITUAL_PENDULUM, "Ritual Pendulum") \
    X(YGO_CARD_FRAME_FUSION_PENDULUM, "Fusion Pendulum") \
    X(YGO_CARD_FRAME_SYNCHRO_PENDULUM, "Synchro Pendulum") \
    X(YGO_CARD_FRAME_XYZ_PENDULUM, "Xyz Pendulum") \
    X(YGO_CARD_FRAME_SPELL, "Spell") \
    X(YGO_CARD_FRAME_TRAP, "Trap") \
    X(YGO_CARD_FRAME_TOKEN, "Token") \
    X(YGO_CARD_FRAME_SKILL, "Skill") \

/**
 * A generic frame / color for the card. This can easily be used to render cards (sans images)
 * dynamically. It is more simple than switching on the card type above.
 */
ENUM_DECL(ygo_card_frame, YGO_CARD_FRAME_DEFS);

#define YGO_CARD_SUBTYPE_DEFS(X, V) \
    /* Monster Cards */\
    V(YGO_CARD_STYPE_MONSTER_AQUA, "Aqua", YGO_CARD_TYPE_MONSTER << 5u) \
    X(YGO_CARD_STYPE_MONSTER_BEAST, "Beast") \
    X(YGO_CARD_STYPE_MONSTER_BEAST_WARRIOR, "Beast Warrior") \
    X(YGO_CARD_STYPE_MONSTER_CREATOR_GOD, "Creator God") \
    X(YGO_CARD_STYPE_MONSTER_CYBERSE, "Cyberse") \
    X(YGO_CARD_STYPE_MONSTER_DINOSAUR, "Dinosaur") \
    X(YGO_CARD_STYPE_MONSTER_DIVINE_BEAST, "Divine Beast") \
    X(YGO_CARD_STYPE_MONSTER_DRAGON, "Dragon") \
    X(YGO_CARD_STYPE_MONSTER_FAIRY, "Fairy") \
    X(YGO_CARD_STYPE_MONSTER_FIEND, "Fiend") \
    X(YGO_CARD_STYPE_MONSTER_FISH, "Fish") \
    X(YGO_CARD_STYPE_MONSTER_INSECT, "Insect") \
    X(YGO_CARD_STYPE_MONSTER_MACHINE, "Machine") \
    X(YGO_CARD_STYPE_MONSTER_PLANT, "Plant") \
    X(YGO_CARD_STYPE_MONSTER_PSYCHIC, "Psychic") \
    X(YGO_CARD_STYPE_MONSTER_PYRO, "Pyro") \
    X(YGO_CARD_STYPE_MONSTER_REPTILE, "Reptile") \
    X(YGO_CARD_STYPE_MONSTER_ROCK, "Rock") \
    X(YGO_CARD_STYPE_MONSTER_SEA_SERPENT, "Sea Serpent") \
    X(YGO_CARD_STYPE_MONSTER_SPELLCASTER, "Spellcaster") \
    X(YGO_CARD_STYPE_MONSTER_THUNDER, "Thunder") \
    X(YGO_CARD_STYPE_MONSTER_WARRIOR, "Warrior") \
    X(YGO_CARD_STYPE_MONSTER_WINGED_BEAST, "Winged Beast") \
    X(YGO_CARD_STYPE_MONSTER_WYRM, "Wyrm") \
    X(YGO_CARD_STYPE_MONSTER_ZOMBIE, "Zombie") \
    \
    /* Spell Cards */\
    V(YGO_CARD_STYPE_SPELL_NORMAL, "Normal", YGO_CARD_TYPE_SPELL << 5u) \
    X(YGO_CARD_STYPE_SPELL_FIELD, "Field") \
    X(YGO_CARD_STYPE_SPELL_EQUIP, "Equip") \
    X(YGO_CARD_STYPE_SPELL_CONTINUOUS, "Continuous") \
    X(YGO_CARD_STYPE_SPELL_QUICK_PLAY, "Quick Play") \
    X(YGO_CARD_STYPE_SPELL_RITUAL, "Ritual") \
    \
    /* Trap Cards */\
    V(YGO_CARD_STYPE_TRAP_NORMAL, "Normal", YGO_CARD_TYPE_TRAP << 5u) \
    X(YGO_CARD_STYPE_TRAP_CONTINUOUS, "Continuous") \
    X(YGO_CARD_STYPE_TRAP_COUNTER, "Counter") \

/**
 * This is the type of Monster, Trap, or Spell card. WIth regards to the string representations:
 * They shouldn't be here. There should be a union on the main type for this field, and different
 * Enum values.
 */
ENUM_DECL(ygo_card_subtype, YGO_CARD_SUBTYPE_DEFS);

#define YGO_CARD_ATTRIBUTE_DEFS(X, V) \
    X(YGO_CARD_ATTR_DARK, "DARK") \
    X(YGO_CARD_ATTR_EARTH, "EARTH") \
    X(YGO_CARD_ATTR_FIRE, "FIRE") \
    X(YGO_CARD_ATTR_LIGHT, "LIGHT") \
    X(YGO_CARD_ATTR_WATER, "WATER") \
    X(YGO_CARD_ATTR_WIND, "WIND") \
    X(YGO_CARD_ATTR_DIVINE, "DIVINE") \
    X(YGO_CARD_ATTR_TIME, "TIME") \

/**
 * Card attribute. Nice.
 */
ENUM_DECL(ygo_card_attribute, YGO_CARD_ATTRIBUTE_DEFS);

#define YGO_CARD_LINK_MARKERS_DEFS(B) \
    B(YGO_CARD_LINK_TOP, "Top", 0u) \
    B(YGO_CARD_LINK_TOP_RIGHT, "Top Right", 1u) \
    B(YGO_CARD_LINK_RIGHT, "Right", 2u) \
    B(YGO_CARD_LINK_BOTTOM_RIGHT, "Bottom Right", 3u) \
    B(YGO_CARD_LINK_BOTTOM, "Bottom", 4u) \
    B(YGO_CARD_LINK_BOTTOM_LEFT, "Bottom Left", 5u) \
    B(YGO_CARD_LINK_LEFT, "Left", 6u) \
    B(YGO_CARD_LINK_TOP_LEFT, "Top Left", 7u) \

/**
 * Cardinal positions of potential card link markers. These are flags, use bitwise operations and
 * pack into a single byte of data.
 */
ENUM_DECL_BITS(ygo_card_link_markers, YGO_CARD_LINK_MARKERS_DEFS);

// At the time of writing, the longest known Yu-Gi-Oh! card name is:
//        "Black Luster Soldier - Envoy of the Evening Twilight", 52 characters long (plus
//        null). Therefore, sizing this to 64 bytes as originally planned might not be enough.
//        We'll size this field to 96 bytes, and if a card has a name longer than that I
//        sincerely fear for the printers.
#define YGO_CARD_NAME_MAX_LEN 0x60

/**
 * Complete packed struct of card data, for storing on NFC tags or other tiny bits of data storage.
 * This does not yet include rule resolution metadata, but that can be added in an extension to the
 * protocol.
 *
 * NTAG215 has 504 bytes of usable memory, ending at address 0x1F7
 * NTAG213 has 144 bytes of usable memory, ending at address 0x8F
 *
 * This struct is designed to fit within NTAG213 chips, however the extended rule resolution data
 * can fill an NTAG215 chip, that would require additional programming beyond what is readily
 * available from an API, but allow offline resolution on card tables that support it.
 *
 * Note that the offset here is no longer tied to the physical data size on the actual chip.
 */
struct _packed_ ygo_card {
    // Page 0x00
    // 0x00 - Card ID or Passcode
    uint32_t id;

    // Page 0x01
    // 0x04 - Main type of card (Normal Monster, Effect Monster, Spell, Trap, etc.)
    ygo_card_type_t type;

    // 0x05 - Frame / Color (normal, effect, synchro, spell, trap, etc.)
    ygo_card_frame_t frame;

    // 0x06 - Subtype of card (Spellcaster, Warrior, etc. for Monsters; Field, Equip, Counter, etc.
    // for Trap/Spell)
    ygo_card_subtype_t subtype;

    // 0x07 - Attribute of the card (FIRE/WATER/DARK, etc.)
    ygo_card_attribute_t attribute;

    // Page 0x02
    // 0x08 - ATK value of Monster card
    uint16_t atk;

    // 0x0A - DEF value of Monster card
    uint16_t def;

    // Page 0x03
    // 0x0C - Rank of Monster card
    uint8_t rank;

    // 0x0D - Pendulum Scale value for Pendulum Monster
    uint8_t scale;

    // 0x0E - Link Value of the card // unnecessary, can get from adding bits on link marker field.
    uint8_t link_value;

    // 0x0F - Link Markers
    ygo_card_link_markers_t link_markers;

    // Page 0x04
    // 0x10 - Card Name. Because NTAG devices have 4 byte pages, it would be nice if we can
    //        align this data neatly to that. This is more of a serializing concern, but this
    //        struct is generally packed the same way it is serialized.
    char name[YGO_CARD_NAME_MAX_LEN];
};

typedef struct ygo_card ygo_card_t;

/////

/**
 * Serialize a card data to a buffer. The buffer *must* be large enough to hold the size of the
 * struct.
 */
size_t ygo_card_serialize(uint8_t *buffer, const ygo_card_t *card);

/**
 * Deserialize a card buffer into a card object.
 */
size_t ygo_card_deserialize(ygo_card_t *card, const uint8_t *buffer);

void ygo_card_print(ygo_card_t *card);

#endif
