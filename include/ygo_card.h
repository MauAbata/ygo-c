#ifndef __ygo_card_h
#define __ygo_card_h

#include <stdint.h>

/**
 * Main type of card (Monster, Effect, Etc.)
 * Thought should be put into this data format to more quickly parse the main type based on bitwise
 * operations. For ease in rendering, please look at the card_frame_t enum.
 */
enum card_type {
    // Main Deck Type
    CARD_TYPE_EFFECT_MONSTER,
    CARD_TYPE_FLIP_EFFECT_MONSTER,
    CARD_TYPE_FLIP_TUNER_EFFECT_MONSTER,
    CARD_TYPE_GEMINI_MONSTER,
    CARD_TYPE_NORMAL_MONSTER,
    CARD_TYPE_NORMAL_TUNER_MONSTER,
    CARD_TYPE_PENDULUM_EFFECT_MONSTER,
    CARD_TYPE_PENDULUM_EFFECT_RITUAL_MONSTER,
    CARD_TYPE_PENDULUM_FLIP_EFFECT_MONSTER,
    CARD_TYPE_PENDULUM_NORMAL_MONSTER,
    CARD_TYPE_PENDULUM_TUNER_EFFECT_MONSTER,
    CARD_TYPE_RITUAL_EFFECT_MONSTER,
    CARD_TYPE_RITUAL_MONSTER,
    CARD_TYPE_SPELL_CARD,
    CARD_TYPE_SPIRIT_MONSTER,
    CARD_TYPE_TOON_MONSTER,
    CARD_TYPE_TRAP_CARD,
    CARD_TYPE_TUNER_MONSTER,
    CARD_TYPE_UNION_EFFECT_MONSTER,

    // Extra Deck Types
    CARD_TYPE_FUSION_MONSTER,
    CARD_TYPE_LINK_MONSTER,
    CARD_TYPE_PENDULUM_EFFECT_FUSION_MONSTER,
    CARD_TYPE_SYNCHRO_MONSTER,
    CARD_TYPE_SYNCHRO_PENDULUM_EFFECT_MONSTER,
    CARD_TYPE_SYNCHRO_TUNER_MONSTER,
    CARD_TYPE_XYZ_MONSTER,
    CARD_TYPE_XYZ_PENDULUM_EFFECT_MONSTER,

    // Other Types
    CARD_TYPE_SKILL_CARD,
    CARD_TYPE_TOKEN,
};

typedef enum card_type card_type_t;

/**
 * A generic frame / color for the card. This can easily be used to render cards (sans images)
 * dynamically. It is more simple than switching on the card type above.
 */
enum card_frame {
    CARD_FRAME_NORMAL,
    CARD_FRAME_EFFECT,
    CARD_FRAME_RITUAL,
    CARD_FRAME_FUSION,
    CARD_FRAME_SYNCHRO,
    CARD_FRAME_XYZ,
    CARD_FRAME_LINK,
    CARD_FRAME_NORMAL_PENDULUM,
    CARD_FRAME_EFFECT_PENDULUM,
    CARD_FRAME_RITUAL_PENDULUM,
    CARD_FRAME_FUSION_PENDULUM,
    CARD_FRAME_SYNCHRO_PENDULUM,
    CARD_FRAME_XYZ_PENDULUM,
    CARD_FRAME_SPELL,
    CARD_FRAME_TRAP,
    CARD_FRAME_TOKEN,
    CARD_FRAME_SKILL,
};

typedef enum card_frame card_frame_t;

/**
 * This is the type of Monster, Trap, or Spell card.
 */
enum card_subtype {
    CARD_STYPE_MONSTER_AQUA,
    CARD_STYPE_MONSTER_BEAST,
    CARD_STYPE_MONSTER_BEAST_WARRIOR,
    CARD_STYPE_MONSTER_CREATOR_GOD,
    CARD_STYPE_MONSTER_CYBERSE,
    CARD_STYPE_MONSTER_DINOSAUR,
    CARD_STYPE_MONSTER_DIVINE_BEAST,
    CARD_STYPE_MONSTER_DRAGON,
    CARD_STYPE_MONSTER_FAIRY,
    CARD_STYPE_MONSTER_FIEND,
    CARD_STYPE_MONSTER_FISH,
    CARD_STYPE_MONSTER_INSECT,
    CARD_STYPE_MONSTER_MACHINE,
    CARD_STYPE_MONSTER_PLANT,
    CARD_STYPE_MONSTER_PSYCHIC,
    CARD_STYPE_MONSTER_PYRO,
    CARD_STYPE_MONSTER_REPTILE,
    CARD_STYPE_MONSTER_ROCK,
    CARD_STYPE_MONSTER_SEA_SERPENT,
    CARD_STYPE_MONSTER_SPELLCASTER,
    CARD_STYPE_MONSTER_THUNDER,
    CARD_STYPE_MONSTER_WARRIOR,
    CARD_STYPE_MONSTER_WINGED_BEAST,
    CARD_STYPE_MONSTER_WYRM,
    CARD_STYPE_MONSTER_ZOMBIE,

    CARD_STYPE_SPELL_NORMAL,
    CARD_STYPE_SPELL_FIELD,
    CARD_STYPE_SPELL_EQUIP,
    CARD_STYPE_SPELL_CONTINUOUS,
    CARD_STYPE_SPELL_QUICK_PLAY,
    CARD_STYPE_SPELL_RITUAL,

    CARD_STYPE_TRAP_NORMAL,
    CARD_STYPE_TRAP_CONTINUOUS,
    CARD_STYPE_TRAP_COUNTER,
};

typedef enum card_subtype card_subtype_t;

/**
 * Card attribute. Nice.
 */
enum card_attribute {
    CARD_ATTR_DARK,
    CARD_ATTR_EARTH,
    CARD_ATTR_FIRE,
    CARD_ATTR_LIGHT,
    CARD_ATTR_WATER,
    CARD_ATTR_WIND,
    CARD_ATTR_DIVINE,
    CARD_ATTR_TIME,
};

typedef enum card_attribute card_attribute_t;

/**
 * Cardinal positions of potential card link markers. These are flags, use bitwise operations and
 * pack into a single byte of data.
 */
enum card_link_markers {
    CARD_LINK_TOP = (1 << 0),
    CARD_LINK_TOP_RIGHT = (1 << 1),
    CARD_LINK_RIGHT = (1 << 2),
    CARD_LINK_BOTTOM_RIGHT = (1 << 3),
    CARD_LINK_BOTTOM = (1 << 4),
    CARD_LINK_BOTTOM_LEFT = (1 << 5),
    CARD_LINK_LEFT = (1 << 6),
    CARD_LINK_TOP_LEFT = (1 << 7),
};

typedef enum card_link_markers card_link_markers_t;

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
 */
struct __attribute__((__packed__)) card_data {
    // 0x00 - Card ID or Passcode
    uint32_t id : 4;

    // 0x04 - Main type of card (Normal Monster, Effect Monster, Spell, Trap, etc.)
    card_type_t type : 1;

    // 0x05 - Frame / Color (normal, effect, synchro, spell, trap, etc.)
    card_frame_t frame : 1;

    // 0x06 - Subtype of card (Spellcaster, Warrior, etc. for Monsters; Field, Equip, Counter, etc.
    // for Trap/Spell)
    card_subtype_t subtype : 1;

    // 0x07 - Attribute of the card (FIRE/WATER/DARK, etc.)
    card_attribute_t attribute : 1;

    // 0x08 - ATK value of Monster card
    uint16_t atk : 2;

    // 0x0A - DEF value of Monster card
    uint16_t def : 2;

    // 0x0C - Rank of Monster card
    uint8_t rank : 1;

    // 0x0D - Pandulum Scale value for Pendulum Monster
    uint8_t scale : 1;

    // 0x0E - Link Value of the card // unnecessary, can get from adding bits on link marker field.
    uint8_t link_value : 1;

    // 0x0F - Link Markers
    card_link_markers_t link_markers : 1;

    // 0x10 - Card Name -- At the time of writing, the longest known Yu-Gi-Oh! card name is:
    //        "Black Luster Soldier - Envoy of the Evening Twilight", 52 characters long (plus
    //        null). Therefore, sizing this to 64 bytes as originally planned might not be enough.
    //        We'll size this field to 96 bytes, and if a card has a name longer than that I
    //        sincerely fear for the printers.
    char name[0x60];

    // 0x70 - Reserved for future use.
    uint8_t __reserved[0x20];
};

typedef struct card_data card_data_t;

#endif