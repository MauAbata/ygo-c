#ifndef __ygo_json_h
#define __ygo_json_h

#include "ygo_card.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ygo_json.h
 * @brief JSON parsing for ygo_card_t using cJSON or compatible API.
 *
 * This module provides conversion between JSON objects (as returned by YGOPRODeck API or similar)
 * and the ygo_card_t binary structure. It handles the complex type string parsing needed to map
 * API responses like "Effect Synchro Tuner Monster" into the appropriate enum values.
 *
 * Expected JSON schema (matches YGOPRODeck API format):
 * {
 *   "id": 12345678,              // Required: Card ID (uint32)
 *   "name": "Dark Magician",     // Required: Card name (max 63 chars)
 *   "type": "Normal Monster",    // Required: Compound type string
 *   "race": "Spellcaster",       // Monster type/race (for monsters)
 *   "attribute": "DARK",         // Attribute (for monsters)
 *   "atk": 2500,                 // ATK (for monsters)
 *   "def": 2100,                 // DEF (for monsters, not Link)
 *   "level": 7,                  // Level/Rank (for monsters, not Link)
 *   "scale": 4,                  // Pendulum scale (if Pendulum)
 *   "linkval": 3,                // Link value (if Link monster)
 *   "linkmarkers": ["Top", "Bottom-Left", "Bottom-Right"]  // Link markers array
 * }
 *
 * Alternatively, numeric enums can be passed directly for internal/pre-parsed data:
 * {
 *   "id": 12345678,
 *   "name": "Dark Magician",
 *   "type_id": 128,              // ygo_card_type_t value
 *   "flags_id": 0,               // ygo_monster_flag_t value
 *   "ability_id": 0,             // ygo_monster_ability_t value
 *   "summon_id": 0,              // ygo_summon_type_t value
 *   "monster_type_id": 20,       // ygo_monster_type_t value
 *   ...
 * }
 */

// Forward declaration - actual cJSON type provided by platform (ESP-IDF, host, etc.)
struct cJSON;

/**
 * Error codes for JSON parsing operations.
 */
typedef enum {
    YGO_JSON_OK = 0,
    YGO_JSON_ERR_NULL_INPUT,
    YGO_JSON_ERR_MISSING_ID,
    YGO_JSON_ERR_MISSING_NAME,
    YGO_JSON_ERR_MISSING_TYPE,
    YGO_JSON_ERR_INVALID_TYPE,
    YGO_JSON_ERR_NAME_TOO_LONG,
    YGO_JSON_ERR_UNKNOWN_ATTRIBUTE,
    YGO_JSON_ERR_UNKNOWN_RACE,
} ygo_json_err_t;

/**
 * Convert a cJSON object to a ygo_card_t structure.
 *
 * This function supports two input formats:
 * 1. API format: Uses string fields ("type", "race", "attribute", "linkmarkers")
 *    that are parsed and converted to enum values.
 * 2. Numeric format: Uses direct enum values ("type_id", "flags_id", etc.)
 *
 * If both formats are present, numeric format takes precedence.
 *
 * @param json  Pointer to cJSON object containing card data
 * @param card  Pointer to ygo_card_t to populate (must be pre-zeroed by caller)
 * @return YGO_JSON_OK on success, or an error code
 */
ygo_json_err_t ygo_json_to_card(const struct cJSON *json, ygo_card_t *card);

/**
 * Parse the compound "type" string from YGOPRODeck API into component enums.
 *
 * Examples:
 *   "Normal Monster"              -> type=MONSTER, summon=NORMAL, flags=0
 *   "Effect Synchro Tuner Monster" -> type=MONSTER, summon=SYNCHRO, flags=TUNER|EFFECT
 *   "Continuous Spell"            -> type=SPELL, spell_type=CONTINUOUS
 *   "Counter Trap"                -> type=TRAP, trap_type=COUNTER
 *
 * @param type_str   The type string to parse (will be copied internally, not modified)
 * @param out_type   Output: card type enum
 * @param out_flags  Output: monster flags (bitfield)
 * @param out_ability Output: monster ability enum
 * @param out_summon  Output: summon type enum (also used for spell/trap type)
 * @return YGO_JSON_OK on success, YGO_JSON_ERR_INVALID_TYPE if unparseable
 */
ygo_json_err_t ygo_json_parse_type_string(const char *type_str,
                                          ygo_card_type_t *out_type,
                                          ygo_monster_flag_t *out_flags,
                                          ygo_monster_ability_t *out_ability,
                                          ygo_summon_type_t *out_summon);

/**
 * Parse link markers array from cJSON array.
 *
 * @param markers_array  cJSON array of marker strings ("Top", "Bottom-Left", etc.)
 * @return Combined bitfield of ygo_card_link_markers_t
 */
ygo_card_link_markers_t ygo_json_parse_link_markers(const struct cJSON *markers_array);

/**
 * Return a human-readable error string for a ygo_json_err_t.
 */
const char *ygo_json_err_str(ygo_json_err_t err);

#ifdef __cplusplus
}
#endif

#endif /* __ygo_json_h */
