#ifndef __ygo_card_h
#define __ygo_card_h

#include <stdint.h>

#ifdef __GNUC__
#define _packed_ __attribute__((__packed__))
#else
#define _packed_ /* nothing */
#endif

/**
 * Generic card type, which is bitmasked from either ygo_card_type or ygo_card_subtype structs.
 */
enum _packed_ ygo_card_type_generic {
    YGO_CARD_TYPE_MONSTER = 0x01,
    YGO_CARD_TYPE_SPELL = 0x02,
    YGO_CARD_TYPE_TRAP = 0x03,
    YGO_CARD_TYPE_EXTRA = 0x04,
    YGO_CARD_TYPE_SKILL = 0x05,
    YGO_CARD_TYPE_TOKEN = 0x06,
};

typedef enum ygo_card_type_generic ygo_card_type_generic_t;

// This bitmask is used in ygo_card_type and ygo_card_subtype to identify the overall generic type
// of the card.
#define YGO_CARD_TYPE_GROUP_BITS (0x07 << 5)

// Infers a generic type from the more detailed ygo_card_type and ygo_card_subtype values.
#define get_generic_type(type) ((d & YGO_CARD_TYPE_GROUP_BITS) >> 5)

/**
 * Main type of card (Monster, Effect, Etc.)
 * Thought should be put into this data format to more quickly parse the main type based on bitwise
 * operations. For ease in rendering, please look at the ygo_card_frame_t enum.
 *
 * Since there are *so* many monster types, we should actually enumerate
 * Monster/Spell/Trap/Extra/Other. Upper 3 bits = this grouping, lower 3 = monster subtype
 */
enum _packed_ ygo_card_type {
    // Main Deck Type
    YGO_CARD_TYPE_NORMAL_MONSTER = (YGO_CARD_TYPE_MONSTER << 5),
    YGO_CARD_TYPE_EFFECT_MONSTER,
    YGO_CARD_TYPE_FLIP_EFFECT_MONSTER,
    YGO_CARD_TYPE_FLIP_TUNER_EFFECT_MONSTER,
    YGO_CARD_TYPE_GEMINI_MONSTER,
    YGO_CARD_TYPE_NORMAL_TUNER_MONSTER,
    YGO_CARD_TYPE_PENDULUM_EFFECT_MONSTER,
    YGO_CARD_TYPE_PENDULUM_EFFECT_RITUAL_MONSTER,
    YGO_CARD_TYPE_PENDULUM_FLIP_EFFECT_MONSTER,
    YGO_CARD_TYPE_PENDULUM_NORMAL_MONSTER,
    YGO_CARD_TYPE_PENDULUM_TUNER_EFFECT_MONSTER,
    YGO_CARD_TYPE_RITUAL_EFFECT_MONSTER,
    YGO_CARD_TYPE_RITUAL_MONSTER,
    YGO_CARD_TYPE_SPIRIT_MONSTER,
    YGO_CARD_TYPE_TOON_MONSTER,
    YGO_CARD_TYPE_TUNER_MONSTER,
    YGO_CARD_TYPE_UNION_EFFECT_MONSTER,

    // Non-Monster Main
    YGO_CARD_TYPE_SPELL_CARD = (YGO_CARD_TYPE_SPELL << 5),
    YGO_CARD_TYPE_TRAP_CARD = (YGO_CARD_TYPE_TRAP << 5),

    // Extra Deck Types
    YGO_CARD_TYPE_FUSION_MONSTER = (YGO_CARD_TYPE_EXTRA << 5),
    YGO_CARD_TYPE_LINK_MONSTER,
    YGO_CARD_TYPE_PENDULUM_EFFECT_FUSION_MONSTER,
    YGO_CARD_TYPE_SYNCHRO_MONSTER,
    YGO_CARD_TYPE_SYNCHRO_PENDULUM_EFFECT_MONSTER,
    YGO_CARD_TYPE_SYNCHRO_TUNER_MONSTER,
    YGO_CARD_TYPE_XYZ_MONSTER,
    YGO_CARD_TYPE_XYZ_PENDULUM_EFFECT_MONSTER,

    // Other Types
    YGO_CARD_TYPE_SKILL_CARD = (YGO_CARD_TYPE_SKILL << 5),
    YGO_CARD_TYPE_TOKEN_CARD = (YGO_CARD_TYPE_TOKEN << 5),
};

typedef enum ygo_card_type ygo_card_type_t;

/**
 * A generic frame / color for the card. This can easily be used to render cards (sans images)
 * dynamically. It is more simple than switching on the card type above.
 */
enum _packed_ ygo_card_frame {
    YGO_CARD_FRAME_NORMAL,
    YGO_CARD_FRAME_EFFECT,
    YGO_CARD_FRAME_RITUAL,
    YGO_CARD_FRAME_FUSION,
    YGO_CARD_FRAME_SYNCHRO,
    YGO_CARD_FRAME_XYZ,
    YGO_CARD_FRAME_LINK,
    YGO_CARD_FRAME_NORMAL_PENDULUM,
    YGO_CARD_FRAME_EFFECT_PENDULUM,
    YGO_CARD_FRAME_RITUAL_PENDULUM,
    YGO_CARD_FRAME_FUSION_PENDULUM,
    YGO_CARD_FRAME_SYNCHRO_PENDULUM,
    YGO_CARD_FRAME_XYZ_PENDULUM,
    YGO_CARD_FRAME_SPELL,
    YGO_CARD_FRAME_TRAP,
    YGO_CARD_FRAME_TOKEN,
    YGO_CARD_FRAME_SKILL,
};

typedef enum ygo_card_frame ygo_card_frame_t;

/**
 * This is the type of Monster, Trap, or Spell card.
 */
enum _packed_ ygo_card_subtype {
    // Monster Cards
    YGO_CARD_STYPE_MONSTER_AQUA = (YGO_CARD_TYPE_MONSTER << 5),
    YGO_CARD_STYPE_MONSTER_BEAST,
    YGO_CARD_STYPE_MONSTER_BEAST_WARRIOR,
    YGO_CARD_STYPE_MONSTER_CREATOR_GOD,
    YGO_CARD_STYPE_MONSTER_CYBERSE,
    YGO_CARD_STYPE_MONSTER_DINOSAUR,
    YGO_CARD_STYPE_MONSTER_DIVINE_BEAST,
    YGO_CARD_STYPE_MONSTER_DRAGON,
    YGO_CARD_STYPE_MONSTER_FAIRY,
    YGO_CARD_STYPE_MONSTER_FIEND,
    YGO_CARD_STYPE_MONSTER_FISH,
    YGO_CARD_STYPE_MONSTER_INSECT,
    YGO_CARD_STYPE_MONSTER_MACHINE,
    YGO_CARD_STYPE_MONSTER_PLANT,
    YGO_CARD_STYPE_MONSTER_PSYCHIC,
    YGO_CARD_STYPE_MONSTER_PYRO,
    YGO_CARD_STYPE_MONSTER_REPTILE,
    YGO_CARD_STYPE_MONSTER_ROCK,
    YGO_CARD_STYPE_MONSTER_SEA_SERPENT,
    YGO_CARD_STYPE_MONSTER_SPELLCASTER,
    YGO_CARD_STYPE_MONSTER_THUNDER,
    YGO_CARD_STYPE_MONSTER_WARRIOR,
    YGO_CARD_STYPE_MONSTER_WINGED_BEAST,
    YGO_CARD_STYPE_MONSTER_WYRM,
    YGO_CARD_STYPE_MONSTER_ZOMBIE,

    // Spell Cards
    YGO_CARD_STYPE_SPELL_NORMAL = (YGO_CARD_TYPE_SPELL << 5),
    YGO_CARD_STYPE_SPELL_FIELD,
    YGO_CARD_STYPE_SPELL_EQUIP,
    YGO_CARD_STYPE_SPELL_CONTINUOUS,
    YGO_CARD_STYPE_SPELL_QUICK_PLAY,
    YGO_CARD_STYPE_SPELL_RITUAL,

    // Trap Cards
    YGO_CARD_STYPE_TRAP_NORMAL = (YGO_CARD_TYPE_TRAP << 5),
    YGO_CARD_STYPE_TRAP_CONTINUOUS,
    YGO_CARD_STYPE_TRAP_COUNTER,
};

typedef enum ygo_card_subtype ygo_card_subtype_t;

/**
 * Card attribute. Nice.
 */
enum _packed_ ygo_card_attribute {
    YGO_CARD_ATTR_DARK,
    YGO_CARD_ATTR_EARTH,
    YGO_CARD_ATTR_FIRE,
    YGO_CARD_ATTR_LIGHT,
    YGO_CARD_ATTR_WATER,
    YGO_CARD_ATTR_WIND,
    YGO_CARD_ATTR_DIVINE,
    YGO_CARD_ATTR_TIME,
};

typedef enum ygo_card_attribute ygo_card_attribute_t;

/**
 * Cardinal positions of potential card link markers. These are flags, use bitwise operations and
 * pack into a single byte of data.
 */
enum _packed_ ygo_card_link_markers {
    YGO_CARD_LINK_TOP = (1 << 0),
    YGO_CARD_LINK_TOP_RIGHT = (1 << 1),
    YGO_CARD_LINK_RIGHT = (1 << 2),
    YGO_CARD_LINK_BOTTOM_RIGHT = (1 << 3),
    YGO_CARD_LINK_BOTTOM = (1 << 4),
    YGO_CARD_LINK_BOTTOM_LEFT = (1 << 5),
    YGO_CARD_LINK_LEFT = (1 << 6),
    YGO_CARD_LINK_TOP_LEFT = (1 << 7),
};

typedef enum ygo_card_link_markers ygo_card_link_markers_t;

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
 */
struct _packed_ ygo_card {
    // Page 0x00
    // 0x00 - Card ID or Passcode
    uint32_t id;

    // Page 0x01
    // 0x04 - Main type of card (Normal Monster, Effect Monster, Spell, Trap, etc.)
    ygo_card_type_t type : 8;

    // 0x05 - Frame / Color (normal, effect, synchro, spell, trap, etc.)
    ygo_card_frame_t frame : 8;

    // 0x06 - Subtype of card (Spellcaster, Warrior, etc. for Monsters; Field, Equip, Counter, etc.
    // for Trap/Spell)
    ygo_card_subtype_t subtype : 8;

    // 0x07 - Attribute of the card (FIRE/WATER/DARK, etc.)
    ygo_card_attribute_t attribute : 8;

    // Page 0x02
    // 0x08 - ATK value of Monster card
    uint16_t atk;

    // 0x0A - DEF value of Monster card
    uint16_t def;

    // Page 0x03
    // 0x0C - Rank of Monster card
    uint8_t rank;

    // 0x0D - Pandulum Scale value for Pendulum Monster
    uint8_t scale;

    // 0x0E - Link Value of the card // unnecessary, can get from adding bits on link marker field.
    uint8_t link_value;

    // 0x0F - Link Markers
    ygo_card_link_markers_t link_markers : 8;

    // Page 0x04
    // 0x10 - Card Name. Because NTAG devices have 4 byte pages, it would be nice if we can
    //        align this data neatly to that. This is more of a serializing concern, but this
    //        struct is generally packed the same way it is serialized.
    char name[YGO_CARD_NAME_MAX_LEN];

    // 0x70 - Reserved for future use.
    uint8_t __reserved[0x20];
};

typedef struct ygo_card ygo_card_t;

/////

/**
 * Serialize a card data to a buffer. The buffer *must* be large enough to hold the size of the
 * struct.
 */
int ygo_card_serialize(uint8_t *buffer, const ygo_card_t *card);

/**
 * Deserialize a card buffer into a card object.
 */
int ygo_card_deserialize(ygo_card_t *card, uint8_t *const buffer);

#endif