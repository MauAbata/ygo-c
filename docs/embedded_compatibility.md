# Embedded System Compatibility Guide

This document describes the compatibility considerations for using `ygo-c` on AVR (ATmega328PB) and ESP32 microcontrollers.

## Platform Support

✅ **Fully Compatible:**
- AVR (ATmega328PB) - 8-bit, 32KB flash, 2KB SRAM
- ESP32 (ESP-IDF) - 32-bit, 4MB flash, 520KB SRAM
- Other embedded platforms with C11 support

## Memory Requirements

### Minimal Configuration (AVR-friendly)
- **Code size:** ~3-4KB (without printf/debug features)
- **RAM (stack):** 
  - Card data structure: 64 bytes
  - Serialization buffer: ~80 bytes
  - Binary write/read contexts: ~24 bytes each
  - CRC table: 0 bytes (using `YGO_USE_SLOW_CRC`)

### Standard Configuration (ESP32)
- **Code size:** ~5-6KB (with all features)
- **RAM:**
  - Same as minimal + CRC table: 512 bytes
  - Debug strings: varies

## Compilation Flags

### Required for AVR (constrained systems)

```ini
# platformio.ini
build_flags = 
    -DYGO_USE_SLOW_CRC          # Bit-by-bit CRC, no 512-byte lookup table
```

### Optional Features

```ini
build_flags = 
    -DYGO_ENABLE_PRINT_DEBUG    # Enable printf-based debug (ygo_card_print, LOGD macro)
                                # NOT recommended for AVR due to flash/RAM overhead
```

### ESP32 Recommended

```ini
# Full features enabled
build_flags = 
    -DYGO_ENABLE_PRINT_DEBUG    # Optional, useful for development
    # YGO_USE_SLOW_CRC not needed (plenty of RAM for table)
```

## Feature Matrix

| Feature | AVR (constrained) | ESP32 | Notes |
|---------|-------------------|-------|-------|
| Card serialization/deserialization | ✅ | ✅ | Core functionality |
| Binary record framing | ✅ | ✅ | Magic word, CRC, headers |
| Signature data structures | ✅ | ✅ | Read/write only |
| CRC-16 (table-based) | ⚠️ | ✅ | 512 bytes RAM |
| CRC-16 (bit-by-bit) | ✅ | ✅ | Slower, no RAM overhead |
| `ygo_card_print()` | ❌ | ⚠️ | Needs `YGO_ENABLE_PRINT_DEBUG` + stdio |
| `LOGD()` debug macro | ❌ | ⚠️ | Same as above |
| Signature verification | ❌ | ❌ | Not implemented (stub only) |

## Standard Library Dependencies

### Always Required (Available on AVR & ESP32)
- `<stdint.h>` - Fixed-width integer types
- `<string.h>` - `memcpy()`, `memset()`, `strcmp()` for enum parsing

### Optional (Only if `YGO_ENABLE_PRINT_DEBUG` defined)
- `<stdio.h>` - `printf()` for debug output
- **Warning:** On AVR, printf adds ~1.5KB flash overhead

### Not Used (Good for embedded)
- ✅ No `<stdlib.h>` - No malloc/free
- ✅ No `<assert.h>` runtime checks - Static assertions only
- ✅ No floating point - Integer arithmetic only

## Usage Examples

### AVR (Minimal Flash/RAM)

```c
// platformio.ini: -DYGO_USE_SLOW_CRC

#include "ygo_card.h"
#include "ygo_bin.h"

ygo_card_t card = {
    .id = 46986414,
    .type = YGO_CARD_TYPE_MONSTER,
    .flags = YGO_MONSTER_FLAG_EFFECT,
    .monster_type = YGO_MONSTER_TYPE_SPELLCASTER,
    .attribute = YGO_ATTRIBUTE_DARK,
    .atk = 2500,
    .def = 2100,
    .level = 7,
};
strcpy(card.name, "Dark Magician");

uint8_t buffer[80];
size_t written = ygo_card_serialize(buffer, &card);

// Later: read from NFC tag
ygo_card_t read_card;
size_t read_len = ygo_card_deserialize(&read_card, buffer);

// No ygo_card_print() available (saves ~2KB flash)
```

### ESP32 (Full Features)

```c
// platformio.ini: -DYGO_ENABLE_PRINT_DEBUG

#include "ygo_card.h"
#include "ygo_bin.h"

ygo_card_t card = { /* ... same as above ... */ };

uint8_t buffer[256];
size_t written = ygo_card_serialize(buffer, &card);

ygo_card_t read_card;
ygo_card_deserialize(&read_card, buffer);

// Debug print available
ygo_card_print(&read_card);
// Outputs:
// ID: 46986414
// Name: Dark Magician
// Type: 0x80 - Spellcaster Effect Monster
// ...
```

## Binary Format Stability

The serialized binary format is **platform-independent**:
- Big-endian for multi-byte fields
- Packed structures (no padding)
- CRC-16-ANSI for integrity checking

Cards serialized on AVR can be read on ESP32 and vice versa.

## Known Limitations

### AVR-specific
- ⚠️ **No signature verification** - Ed25519 requires 32KB+ flash (exceeds ATmega328PB)
  - Verification must happen on cybermat-core (ESP32) or connected host
- ⚠️ **String operations limited** - Card names truncated to `YGO_CARD_NAME_MAX_LEN` (32 bytes)
- ⚠️ **No enum parsing from strings** - Enum string functions (`*_from_str()`) available but add flash overhead

### Both Platforms
- ❌ **No SHA-256** - `ygo_sig_build_payload()` uses placeholder zeros for card hash
  - Real implementation needed for production signature verification

## Cross-Repository Integration

### cybermat-core (ESP32 host)
- Uses full feature set: serialization, debug prints, signature structures
- Can verify signatures (when Ed25519 implemented)
- Acts as programmer for pad firmwares

### duel-pad-sensor-control (ATmega328PB pads)
- Uses minimal feature set: serialization only
- Stores serialized card data received from MFRC522 NFC tags
- Forwards data to cybermat-core over SPI for processing

## Build System Integration

### PlatformIO (Recommended)

```ini
# cybermat-core (ESP32)
[env:default]
platform = espressif32
lib_deps = 
    ygo-c
build_flags = 
    -DYGO_ENABLE_PRINT_DEBUG

# duel-pad-sensor-control (AVR)
[env:pad0]
platform = atmelavr
board = ATmega328PB
lib_deps = 
    ygo-c
build_flags = 
    -DYGO_USE_SLOW_CRC
```

### CMake (ESP-IDF)

Already registered automatically via `CMakeLists.txt` in `lib/ygo-c/`.

## Testing

Unity-based tests in `test/test_unit/` are **native-only** (x86/ARM host):
- Serialization round-trip tests
- Binary format validation
- Enum encoding verification

Run with: `pio test -e native_tests`

Tests do NOT run on actual AVR/ESP32 hardware (not needed for deterministic serialization).

## Future Enhancements

Planned features for embedded compatibility:
- [ ] SHA-256 implementation for ESP32 (hardware-accelerated)
- [ ] Ed25519 signature verification for ESP32 (using mbedTLS or Monocypher)
- [ ] Flash-based string storage for AVR (`PROGMEM` for enum strings)
- [ ] Optional fixed-point arithmetic if stats/formulas added

## Support

For issues specific to:
- **AVR integration:** See `duel-pad-sensor-control` repository
- **ESP32 integration:** See `cybermat-core` repository
- **Library bugs:** File issue in the repository containing `lib/ygo-c`
