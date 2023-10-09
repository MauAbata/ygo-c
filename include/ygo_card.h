#ifndef __ygo_card_h
#define __ygo_card_h

#include <stdint.h>
#include <assert.h>
#include "internals.h"

#define YGO_CARD_TYPE_DEFS(X, V) \
    V(YGO_CARD_TYPE_TOKEN, "Token", 0x00u) \
    V(YGO_CARD_TYPE_SKILL, "Skill", 0x10u) \
    V(YGO_CARD_TYPE_SPELL, "Spell", 0x20u) \
    V(YGO_CARD_TYPE_TRAP, "Trap", 0x30u) \
    V(YGO_CARD_TYPE_TRAP_MONSTER, "Trap-Monster", 0x40u) \
    V(YGO_CARD_TYPE_MONSTER, "Monster", 0x80u) \

/**
 * Card Type corresponds to the 4 MSB of the CTYPE1 data byte. Note that for Trap-Monster and
 * Monster types, only the most significant 1 is used. That is, for Trap-Monster, the 2 LSB
 * of this nibble are ignored, and for Monster, the 3 LSB of this nibble is ignored.
 *
 * For convenience in checking this, two macros are defined as IS_MONSTER() and IS_TRAP_MONSTER().
 */
ENUM_DECL(ygo_card_type, YGO_CARD_TYPE_DEFS);

#define IS_MONSTER(type) (type & 0x80)
#define IS_TRAP_MONSTER(type) ((!(type & 0x80u)) && (type & 0x40u))
#define GET_CARD_TYPE(type) ( (ygo_card_type_t) \
    ((type & 0x80u) || (type & 0x40u) || (type & 0x30u)) )

#define YGO_MONSTER_FLAG_DEFS(B) \
    B(YGO_MONSTER_FLAG_TUNER, "Tuner", 3u) \
    B(YGO_MONSTER_FLAG_PENDULUM, "Pendulum", 4u) \
    B(YGO_MONSTER_FLAG_EFFECT, "Effect", 5u) \

/**
 * Tuner, Pendulum, and Effect monsters can apply in any combination on top of the other Abilities
 * and Summon Types of this monster. Therefore, these items are represented as flags. The
 * definitions in this enum are shifted to where they align in the CTYPE1 data byte for ease of
 * checking monster type via bitwise-and.
 */
ENUM_DECL_BITS(ygo_monster_flag, YGO_MONSTER_FLAG_DEFS);
static_assert(YGO_MONSTER_FLAG_EFFECT == 0b00100000u, "YGO_MONSTER_FLAG_EFFECT value incorrect!");

#define GET_MONSTER_FLAG(ctype1) ((ygo_monster_flag_t)(ctype1 & 0x38u))


#define YGO_MONSTER_ABILITY_DEFS(X, V) \
    X(YGO_MONSTER_ABILITY_NORMAL, "") \
    X(YGO_MONSTER_ABILITY_FLIP, "Flip") \
    X(YGO_MONSTER_ABILITY_GEMINI, "Gemini") \
    X(YGO_MONSTER_ABILITY_SPIRIT, "Spirit") \
    X(YGO_MONSTER_ABILITY_TOON, "Toon") \
    X(YGO_MONSTER_ABILITY_UNION, "Union") \

/**
 * Ability attached to the monster. This includes things like Flip, Union, Toon, etc. This data
 * normally occupies the 3 LSBs of the CTYPE1 data byte, therefore the values are not shifted.
 */
ENUM_DECL(ygo_monster_ability, YGO_MONSTER_ABILITY_DEFS);

// Extract the ability from a card type byte.
#define GET_ABILITY(ctype1) ((ygo_monster_ability_t)(ctype1 & 0x07u))


#define YGO_SUMMON_TYPE_SHIFT (5u)
#define YGO_SUMMON_TYPE_DEFS(X, V) \
    V(YGO_SUMMON_TYPE_NORMAL, "", 0) \
    V(YGO_SUMMON_TYPE_SPECIAL, "Special", 1u << YGO_SUMMON_TYPE_SHIFT) \
    X(YGO_SUMMON_TYPE_RITUAL, "Ritual") \
    X(YGO_SUMMON_TYPE_FUSION, "Fusion") \
    X(YGO_SUMMON_TYPE_LINK, "Link") \
    X(YGO_SUMMON_TYPE_SYNCHRO, "Synchro") \
    X(YGO_SUMMON_TYPE_XYZ, "Xyz") \

/**
 * Monster summoning type, if any. This includes things like Fusion and Link monsters. This data
 * normally lives in the 3 MSBs of the CTYPE0 data byte, therefore these values are bit shifted.
 * (Except "NORMAL", which has a value of 0, and is impossible to bit shift.)
 *
 * Use (value >> YGO_SUMMON_TYPE_SHIFT) to return values to a 0-index.
 */
ENUM_DECL(ygo_summon_type, YGO_SUMMON_TYPE_DEFS);

#define GET_SUMMON_TYPE(ctype0) ((ygo_summon_type_t)(ctype0 & 0xE0u))

#define YGO_MONSTER_TYPE_DEFS(X, V) \
    X(YGO_MONSTER_TYPE_AQUA, "Aqua") \
    X(YGO_MONSTER_TYPE_BEAST, "Beast") \
    X(YGO_MONSTER_TYPE_BEAST_WARRIOR, "Beast Warrior") \
    X(YGO_MONSTER_TYPE_CREATOR_GOD, "Creator God") \
    X(YGO_MONSTER_TYPE_CYBERSE, "Cyberse") \
    X(YGO_MONSTER_TYPE_DINOSAUR, "Dinosaur") \
    X(YGO_MONSTER_TYPE_DIVINE_BEAST, "Divine Beast") \
    X(YGO_MONSTER_TYPE_DRAGON, "Dragon") \
    X(YGO_MONSTER_TYPE_FAIRY, "Fairy") \
    X(YGO_MONSTER_TYPE_FIEND, "Fiend") \
    X(YGO_MONSTER_TYPE_FISH, "Fish") \
    X(YGO_MONSTER_TYPE_ILLUSION, "Illusion") \
    X(YGO_MONSTER_TYPE_INSECT, "Insect") \
    X(YGO_MONSTER_TYPE_MACHINE, "Machine") \
    X(YGO_MONSTER_TYPE_PLANT, "Plant") \
    X(YGO_MONSTER_TYPE_PSYCHIC, "Psychic") \
    X(YGO_MONSTER_TYPE_PYRO, "Pyro") \
    X(YGO_MONSTER_TYPE_REPTILE, "Reptile") \
    X(YGO_MONSTER_TYPE_ROCK, "Rock") \
    X(YGO_MONSTER_TYPE_SEA_SERPENT, "Sea Serpent") \
    X(YGO_MONSTER_TYPE_SPELLCASTER, "Spellcaster") \
    X(YGO_MONSTER_TYPE_THUNDER, "Thunder") \
    X(YGO_MONSTER_TYPE_WARRIOR, "Warrior") \
    X(YGO_MONSTER_TYPE_WINGED_BEAST, "Winged Beast") \
    X(YGO_MONSTER_TYPE_WYRM, "Wyrm") \
    X(YGO_MONSTER_TYPE_ZOMBIE, "Zombie") \

/**
 * This is the main monster type (Cybers, Dragon, Winged-Beast, Cyberse, etc.) Some items have a
 * string representation which may or may not include hyphens. The related from_string functions
 * will handle this, however the associated to_string functions will return a version with a space.
 *
 * This data normally lives in the 5 LSBs of the CTYPE0 data byte, therefore there is no bit
 * shifting performed.
 */
ENUM_DECL(ygo_monster_type, YGO_MONSTER_TYPE_DEFS);

// This has no reason to exist, I just love Cyberse type monsters <3.
#define IS_AMAZING(mtype) (mtype == YGO_MONSTER_TYPE_CYBERSE)
#define GET_MONSTER_TYPE(ctype0) ((ygo_monster_type_t)(ctype0 & 0x1Fu))


#define YGO_SPELL_TYPE_DEFS(X, V) \
    X(YGO_SPELL_TYPE_NORMAL, "Normal") \
    X(YGO_SPELL_TYPE_CONTINUOUS, "Continuous") \
    X(YGO_SPELL_TYPE_EQUIP, "Equip") \
    X(YGO_SPELL_TYPE_FIELD, "Field") \
    X(YGO_SPELL_TYPE_QUICK_PLAY, "Quick Play") \
    X(YGO_SPELL_TYPE_RITUAL, "Ritual") \

ENUM_DECL(ygo_spell_type, YGO_SPELL_TYPE_DEFS);

#define YGO_TRAP_TYPE_DEFS(X, V) \
    X(YGO_TRAP_TYPE_NORMAL, "Normal") \
    X(YGO_TRAP_TYPE_CONTINUOUS, "Continuous") \
    X(YGO_TRAP_TYPE_COUNTER, "Counter") \

ENUM_DECL(ygo_trap_type, YGO_TRAP_TYPE_DEFS);

#define YGO_ATTRIBUTE_DEFS(X, V) \
    X(YGO_ATTRIBUTE_DARK, "DARK") \
    X(YGO_ATTRIBUTE_DIVINE, "DIVINE") \
    X(YGO_ATTRIBUTE_EARTH, "EARTH") \
    X(YGO_ATTRIBUTE_FIRE, "FIRE") \
    X(YGO_ATTRIBUTE_LIGHT, "LIGHT") \
    X(YGO_ATTRIBUTE_WATER, "WATER") \
    X(YGO_ATTRIBUTE_WIND, "WIND") \
    X(YGO_ATTRIBUTE_TIME, "TIME") \

ENUM_DECL(ygo_attribute, YGO_ATTRIBUTE_DEFS);

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
//        null). Therefore, I will size this to 64 bytes.
#define YGO_CARD_NAME_MAX_LEN 0x40


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
    uint32_t id;

    ygo_card_type_t type;
    ygo_monster_flag_t flags;
    ygo_monster_type_t monster_type;
    ygo_monster_ability_t ability;

    union {
        ygo_summon_type_t summon;
        ygo_spell_type_t spell_type;
        ygo_trap_type_t trap_type;
    };

    ygo_attribute_t attribute;
    uint16_t atk;
    uint16_t def;
    uint8_t level;

    union {
        uint8_t scale;
        uint8_t link_value;
    };
    ygo_card_link_markers_t link_markers;

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
