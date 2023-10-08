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

Monster types are wild. I'm gonna try and list everything:
 001        010    011    100     101     110      111
MONSTER | SPELL | TRAP | SKILL | TOKEN | GEMINI | SPIRIT
EFFECT
FLIP
TUNER
GEMINI
PENDULUM
RITUAL
SPIRIT
TOON
UNION
LINK
SYNCHRO
XYZ
