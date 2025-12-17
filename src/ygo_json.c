/**
 * @file ygo_json.c
 * @brief JSON parsing for ygo_card_t.
 *
 * Converts YGOPRODeck API-style JSON or pre-parsed numeric JSON into ygo_card_t structs.
 * Uses tokenization pattern similar to ygo-card-tool's _assign_card_type().
 */

#include "ygo_json.h"
#include <cJSON.h>
#include <string.h>

// Internal buffer for tokenizing type string (not thread-safe, but fine for embedded use)
#define TYPE_STR_BUF_SIZE 128

/**
 * Parse a single token from the type string and update the output enums accordingly.
 * Returns 1 if the token was recognized, 0 otherwise.
 */
static int _process_type_token(const char *token,
                               ygo_card_type_t *out_type,
                               ygo_monster_flag_t *out_flags,
                               ygo_monster_ability_t *out_ability,
                               ygo_summon_type_t *out_summon) {
    // Card type keywords
    if (strcmp(token, "Monster") == 0) {
        *out_type = YGO_CARD_TYPE_MONSTER;
        return 1;
    }
    if (strcmp(token, "Spell") == 0 || strcmp(token, "Spell Card") == 0) {
        *out_type = YGO_CARD_TYPE_SPELL;
        return 1;
    }
    if (strcmp(token, "Trap") == 0 || strcmp(token, "Trap Card") == 0) {
        *out_type = YGO_CARD_TYPE_TRAP;
        return 1;
    }
    if (strcmp(token, "Token") == 0) {
        *out_type = YGO_CARD_TYPE_TOKEN;
        return 1;
    }
    if (strcmp(token, "Skill") == 0) {
        *out_type = YGO_CARD_TYPE_SKILL;
        return 1;
    }

    // Monster flags
    if (strcmp(token, "Effect") == 0) {
        *out_flags |= YGO_MONSTER_FLAG_EFFECT;
        return 1;
    }
    if (strcmp(token, "Tuner") == 0) {
        *out_flags |= YGO_MONSTER_FLAG_TUNER;
        return 1;
    }
    if (strcmp(token, "Pendulum") == 0) {
        *out_flags |= YGO_MONSTER_FLAG_PENDULUM;
        return 1;
    }

    // Monster abilities
    if (strcmp(token, "Normal") == 0) {
        // "Normal" can mean normal monster (no ability) or Normal Spell/Trap
        // Context determines meaning - if we're already a spell/trap, this is subtype
        *out_ability = YGO_MONSTER_ABILITY_NORMAL;
        return 1;
    }
    if (strcmp(token, "Flip") == 0) {
        *out_ability = YGO_MONSTER_ABILITY_FLIP;
        return 1;
    }
    if (strcmp(token, "Gemini") == 0) {
        *out_ability = YGO_MONSTER_ABILITY_GEMINI;
        return 1;
    }
    if (strcmp(token, "Spirit") == 0) {
        *out_ability = YGO_MONSTER_ABILITY_SPIRIT;
        return 1;
    }
    if (strcmp(token, "Toon") == 0) {
        *out_ability = YGO_MONSTER_ABILITY_TOON;
        return 1;
    }
    if (strcmp(token, "Union") == 0) {
        *out_ability = YGO_MONSTER_ABILITY_UNION;
        return 1;
    }

    // Summon types
    if (strcmp(token, "Fusion") == 0) {
        *out_summon = YGO_SUMMON_TYPE_FUSION;
        return 1;
    }
    if (strcmp(token, "Ritual") == 0) {
        *out_summon = YGO_SUMMON_TYPE_RITUAL;
        return 1;
    }
    if (strcmp(token, "Synchro") == 0) {
        *out_summon = YGO_SUMMON_TYPE_SYNCHRO;
        return 1;
    }
    if (strcmp(token, "Xyz") == 0 || strcmp(token, "XYZ") == 0) {
        *out_summon = YGO_SUMMON_TYPE_XYZ;
        return 1;
    }
    if (strcmp(token, "Link") == 0) {
        *out_summon = YGO_SUMMON_TYPE_LINK;
        return 1;
    }

    // Spell subtypes (set in summon field for Spell cards)
    if (strcmp(token, "Continuous") == 0) {
        // Could be spell or trap - handled by context
        *out_summon = (ygo_summon_type_t)YGO_SPELL_TYPE_CONTINUOUS;
        return 1;
    }
    if (strcmp(token, "Equip") == 0) {
        *out_summon = (ygo_summon_type_t)YGO_SPELL_TYPE_EQUIP;
        return 1;
    }
    if (strcmp(token, "Field") == 0) {
        *out_summon = (ygo_summon_type_t)YGO_SPELL_TYPE_FIELD;
        return 1;
    }
    if (strcmp(token, "Quick-Play") == 0) {
        *out_summon = (ygo_summon_type_t)YGO_SPELL_TYPE_QUICK_PLAY;
        return 1;
    }

    // Trap subtypes
    if (strcmp(token, "Counter") == 0) {
        *out_summon = (ygo_summon_type_t)YGO_TRAP_TYPE_COUNTER;
        return 1;
    }

    // Unknown token - not necessarily an error, just ignore
    return 0;
}

ygo_json_err_t ygo_json_parse_type_string(const char *type_str,
                                          ygo_card_type_t *out_type,
                                          ygo_monster_flag_t *out_flags,
                                          ygo_monster_ability_t *out_ability,
                                          ygo_summon_type_t *out_summon) {
    if (!type_str || !out_type || !out_flags || !out_ability || !out_summon) {
        return YGO_JSON_ERR_NULL_INPUT;
    }

    // Initialize outputs
    *out_type = YGO_CARD_TYPE_TOKEN; // Default, will be overwritten
    *out_flags = 0;
    *out_ability = YGO_MONSTER_ABILITY_NORMAL;
    *out_summon = YGO_SUMMON_TYPE_NORMAL;

    // Copy string for tokenization (strtok_r modifies the string)
    char buf[TYPE_STR_BUF_SIZE];
    size_t len = strlen(type_str);
    if (len >= TYPE_STR_BUF_SIZE) {
        len = TYPE_STR_BUF_SIZE - 1;
    }
    memcpy(buf, type_str, len);
    buf[len] = '\0';

    // Tokenize by space
    char *saveptr = NULL;
    char *token = strtok_r(buf, " ", &saveptr);
    int found_any = 0;

    while (token != NULL) {
        if (_process_type_token(token, out_type, out_flags, out_ability, out_summon)) {
            found_any = 1;
        }
        token = strtok_r(NULL, " ", &saveptr);
    }

    if (!found_any) {
        return YGO_JSON_ERR_INVALID_TYPE;
    }

    return YGO_JSON_OK;
}

ygo_card_link_markers_t ygo_json_parse_link_markers(const cJSON *markers_array) {
    ygo_card_link_markers_t result = 0;

    if (!markers_array || !cJSON_IsArray(markers_array)) {
        return result;
    }

    const cJSON *marker;
    cJSON_ArrayForEach(marker, markers_array) {
        if (!cJSON_IsString(marker)) continue;
        const char *s = marker->valuestring;

        // Match marker strings (API uses both formats)
        if (strcmp(s, "Top") == 0) {
            result |= YGO_CARD_LINK_TOP;
        } else if (strcmp(s, "Top-Right") == 0 || strcmp(s, "Top Right") == 0) {
            result |= YGO_CARD_LINK_TOP_RIGHT;
        } else if (strcmp(s, "Right") == 0) {
            result |= YGO_CARD_LINK_RIGHT;
        } else if (strcmp(s, "Bottom-Right") == 0 || strcmp(s, "Bottom Right") == 0) {
            result |= YGO_CARD_LINK_BOTTOM_RIGHT;
        } else if (strcmp(s, "Bottom") == 0) {
            result |= YGO_CARD_LINK_BOTTOM;
        } else if (strcmp(s, "Bottom-Left") == 0 || strcmp(s, "Bottom Left") == 0) {
            result |= YGO_CARD_LINK_BOTTOM_LEFT;
        } else if (strcmp(s, "Left") == 0) {
            result |= YGO_CARD_LINK_LEFT;
        } else if (strcmp(s, "Top-Left") == 0 || strcmp(s, "Top Left") == 0) {
            result |= YGO_CARD_LINK_TOP_LEFT;
        }
    }

    return result;
}

ygo_json_err_t ygo_json_to_card(const cJSON *json, ygo_card_t *card) {
    if (!json || !card) {
        return YGO_JSON_ERR_NULL_INPUT;
    }

    // Required: id
    const cJSON *id_json = cJSON_GetObjectItemCaseSensitive(json, "id");
    if (!id_json || !cJSON_IsNumber(id_json)) {
        return YGO_JSON_ERR_MISSING_ID;
    }
    card->id = (uint32_t)id_json->valuedouble;

    // Required: name
    const cJSON *name_json = cJSON_GetObjectItemCaseSensitive(json, "name");
    if (!name_json || !cJSON_IsString(name_json)) {
        return YGO_JSON_ERR_MISSING_NAME;
    }
    size_t name_len = strlen(name_json->valuestring);
    if (name_len >= YGO_CARD_NAME_MAX_LEN) {
        name_len = YGO_CARD_NAME_MAX_LEN - 1;
    }
    memcpy(card->name, name_json->valuestring, name_len);
    card->name[name_len] = '\0';

    // Check for numeric format first (takes precedence)
    const cJSON *type_id = cJSON_GetObjectItemCaseSensitive(json, "type_id");
    if (type_id && cJSON_IsNumber(type_id)) {
        // Numeric format: direct enum values
        card->type = (ygo_card_type_t)type_id->valueint;

        const cJSON *flags_id = cJSON_GetObjectItemCaseSensitive(json, "flags_id");
        if (flags_id && cJSON_IsNumber(flags_id)) {
            card->flags = (ygo_monster_flag_t)flags_id->valueint;
        }

        const cJSON *ability_id = cJSON_GetObjectItemCaseSensitive(json, "ability_id");
        if (ability_id && cJSON_IsNumber(ability_id)) {
            card->ability = (ygo_monster_ability_t)ability_id->valueint;
        }

        const cJSON *summon_id = cJSON_GetObjectItemCaseSensitive(json, "summon_id");
        if (summon_id && cJSON_IsNumber(summon_id)) {
            card->summon = (ygo_summon_type_t)summon_id->valueint;
        }

        const cJSON *mtype_id = cJSON_GetObjectItemCaseSensitive(json, "monster_type_id");
        if (mtype_id && cJSON_IsNumber(mtype_id)) {
            card->monster_type = (ygo_monster_type_t)mtype_id->valueint;
        }
    } else {
        // String format: parse "type" field
        const cJSON *type_str = cJSON_GetObjectItemCaseSensitive(json, "type");
        if (!type_str || !cJSON_IsString(type_str)) {
            return YGO_JSON_ERR_MISSING_TYPE;
        }

        ygo_json_err_t err = ygo_json_parse_type_string(
            type_str->valuestring, &card->type, &card->flags, &card->ability, &card->summon);
        if (err != YGO_JSON_OK) {
            return err;
        }

        // Parse race (monster type) from string
        const cJSON *race = cJSON_GetObjectItemCaseSensitive(json, "race");
        if (race && cJSON_IsString(race)) {
            card->monster_type = ygo_monster_type_from_str(race->valuestring);
            // If not found in monster types, might be spell/trap race (ignore)
        }
    }

    // Attribute (string or numeric)
    const cJSON *attr_id = cJSON_GetObjectItemCaseSensitive(json, "attribute_id");
    if (attr_id && cJSON_IsNumber(attr_id)) {
        card->attribute = (ygo_attribute_t)attr_id->valueint;
    } else {
        const cJSON *attr = cJSON_GetObjectItemCaseSensitive(json, "attribute");
        if (attr && cJSON_IsString(attr)) {
            card->attribute = ygo_attribute_from_str(attr->valuestring);
        }
    }

    // ATK (numeric)
    const cJSON *atk = cJSON_GetObjectItemCaseSensitive(json, "atk");
    if (atk && cJSON_IsNumber(atk)) {
        card->atk = (uint16_t)atk->valueint;
    }

    // DEF (numeric)
    const cJSON *def = cJSON_GetObjectItemCaseSensitive(json, "def");
    if (def && cJSON_IsNumber(def)) {
        card->def = (uint16_t)def->valueint;
    }

    // Level/Rank (numeric)
    const cJSON *level = cJSON_GetObjectItemCaseSensitive(json, "level");
    if (level && cJSON_IsNumber(level)) {
        card->level = (uint8_t)level->valueint;
    }

    // Pendulum scale (numeric)
    const cJSON *scale = cJSON_GetObjectItemCaseSensitive(json, "scale");
    if (scale && cJSON_IsNumber(scale)) {
        card->scale = (uint8_t)scale->valueint;
    }

    // Link value (numeric) - uses same union field as scale
    const cJSON *linkval = cJSON_GetObjectItemCaseSensitive(json, "linkval");
    if (linkval && cJSON_IsNumber(linkval)) {
        card->link_value = (uint8_t)linkval->valueint;
    }

    // Link markers (array of strings or numeric)
    const cJSON *markers_id = cJSON_GetObjectItemCaseSensitive(json, "link_markers_id");
    if (markers_id && cJSON_IsNumber(markers_id)) {
        card->link_markers = (ygo_card_link_markers_t)markers_id->valueint;
    } else {
        const cJSON *markers = cJSON_GetObjectItemCaseSensitive(json, "linkmarkers");
        if (markers && cJSON_IsArray(markers)) {
            card->link_markers = ygo_json_parse_link_markers(markers);
        }
    }

    return YGO_JSON_OK;
}

const char *ygo_json_err_str(ygo_json_err_t err) {
    switch (err) {
    case YGO_JSON_OK: return "OK";
    case YGO_JSON_ERR_NULL_INPUT: return "Null input";
    case YGO_JSON_ERR_MISSING_ID: return "Missing 'id' field";
    case YGO_JSON_ERR_MISSING_NAME: return "Missing 'name' field";
    case YGO_JSON_ERR_MISSING_TYPE: return "Missing 'type' field";
    case YGO_JSON_ERR_INVALID_TYPE: return "Invalid type string";
    case YGO_JSON_ERR_NAME_TOO_LONG: return "Name too long";
    case YGO_JSON_ERR_UNKNOWN_ATTRIBUTE: return "Unknown attribute";
    case YGO_JSON_ERR_UNKNOWN_RACE: return "Unknown race";
    default: return "Unknown error";
    }
}
