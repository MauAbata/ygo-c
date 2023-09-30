# YGO.c

This is a simple C library that defines the structure of Yu-Gi-Oh! cards and their related data.

## Example:

```c
#include "ygo_card.h"

card_data_t favorite_card = {
    .name = "Dark Magician of Chaos",
    .type = CARD_TYPE_EFFECT_MONSTER,
    .frame = CARD_FRAME_EFFECT,
    .atk = 2800,
    .def = 2600,
    .rank = 8,
    .subtype = CARD_SUBTYPE_MON_SPELLCASTER,
    .attribute = CARD_ATTR_DARK,
};
```