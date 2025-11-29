# Card Data Formats

This document describes the binary card data formats supported by the ygo-c library and related firmware. The format is designed for storage on NFC tags (NTAG213/215) and transmission over SPI between sensor pads and host controllers.

## Magic Word Detection

All card data begins with a 4-byte magic word that identifies the card game format. The magic word is used to dispatch to the appropriate deserializer.

### Registered Magic Words

| Bytes (Hex)          | ASCII       | Format    | Status      | Description                     |
|---------------------|-------------|-----------|-------------|---------------------------------|
| `0x0E 0x59 0x47 0x4F` | `\x0EY G O` | YGO       | Implemented | Yu-Gi-Oh! Trading Card Game     |
| `0x0E 0x50 0x54 0x43` | `\x0EP T C` | PTC       | Reserved    | Pokémon Trading Card Game       |
| `0x0E 0x4D 0x54 0x47` | `\x0EM T G` | MTG       | Reserved    | Magic: The Gathering            |

**Note**: The leading `0x0E` byte is a format identifier that distinguishes card data from other NFC tag content. Future formats should preserve this prefix.

## YGO Binary Format (ygo_bin)

The YGO format stores Yu-Gi-Oh! card data in a compact binary representation optimized for NTAG213 tags (144 bytes user memory).

### Structure Overview

```
| Magic Word | Record Header | Card Data | CRC16 |
|   4 bytes  |    4 bytes    | N bytes   | 2 bytes |
```

### Magic Word
- Bytes: `{0x0E, 'Y', 'G', 'O'}` = `{0x0E, 0x59, 0x47, 0x4F}`

### Record Header (`ygo_bin_record_header_t`)
```c
struct ygo_bin_record_header {
    uint8_t record_type;    // BIN_RECORD_CARD_BASIC = 0x00
    uint8_t data_version;   // YGO_CARD_DATA_VERSION = 0x00
    uint16_t record_length; // Payload length (big-endian)
};
```

### Card Data Fields (`ygo_card_t`)

| Offset | Size | Field          | Description                              |
|--------|------|----------------|------------------------------------------|
| 0      | 4    | id             | Card database ID (big-endian u32)        |
| 4      | 1    | type/flags     | Card type + monster flags (packed)       |
| 5      | 1    | summon/monster | Summon type + monster type (packed)      |
| 6      | 2    | (reserved)     | Alignment padding                        |
| 8      | 2    | atk            | Attack points (big-endian u16)           |
| 10     | 2    | def            | Defense points (big-endian u16)          |
| 12     | 1    | level          | Monster level/rank                       |
| 13     | 1    | attribute      | Card attribute (DARK, LIGHT, etc.)       |
| 14     | 1    | scale          | Pendulum scale / Link value              |
| 15     | 1    | link_markers   | Link arrow directions (bitfield)         |
| 16     | 64   | name           | Card name (null-terminated string)       |

**Total serialized size**: ~80 bytes (variable based on name length)
**Maximum size**: 144 bytes (NTAG213 user memory)

### Type Byte Encoding (offset 4)

```
Bit 7: IS_MONSTER flag (0x80)
Bit 6: IS_TRAP_MONSTER flag (0x40)
Bits 5-4: Card type (Token=0x00, Skill=0x10, Spell=0x20, Trap=0x30)
Bit 5: EFFECT flag
Bit 4: PENDULUM flag
Bit 3: TUNER flag
Bits 2-0: Monster ability (Normal=0, Flip=1, Gemini=2, Spirit=3, Toon=4, Union=5)
```

### CRC16 Checksum

- Algorithm: CRC-16/CCITT (polynomial 0x1021)
- Covers: All bytes from record header through end of payload
- Location: Last 2 bytes of record

## Chunked Transfer Protocol

When transmitting card data over SPI, the 144-byte buffer is split into chunks due to the 128-byte SPI payload limit.

### Request: Q_CARD_DATA (0x26)

```c
struct q_card_data_request {
    uint16_t offset;   // LE: Starting byte offset
    uint8_t max_len;   // Maximum bytes to return (default: 96)
};
```

### Response: RP_CARD_DATA (0x36)

```c
struct rp_card_data_response {
    uint16_t total_len;  // LE: Total buffer size (144 for NTAG213)
    uint16_t offset;     // LE: Offset of this chunk
    uint8_t chunk_len;   // Bytes in this chunk
    uint8_t data[];      // Chunk data (up to 96 bytes)
};
```

### Transfer Example

To read full 144-byte buffer with 96-byte chunks:

1. **Request 1**: `offset=0, max_len=96`
   - Response: `total=144, offset=0, chunk=96, data[0..95]`
2. **Request 2**: `offset=96, max_len=96`
   - Response: `total=144, offset=96, chunk=48, data[96..143]`

Host detects transfer complete when `offset + chunk_len >= total_len`.

## Adding New Card Formats

To add support for a new trading card game:

1. **Register magic word** in this document
2. **Create deserializer** following `ygo_card_deserialize()` pattern
3. **Update tag.c** magic word check to dispatch to new deserializer
4. **Update host firmware** to recognize new format after deserialization

### Magic Word Guidelines

- First byte: Always `0x0E` (format identifier)
- Bytes 2-4: 3-letter game abbreviation in ASCII
- Avoid collisions with existing registrations
- Document in this file before implementation

## References

- `lib/ygo-c/include/ygo_card.h` - Card structure and type definitions
- `lib/ygo-c/include/ygo_bin.h` - Binary format helpers
- `lib/ygo-c/src/ygo_card.c` - Serialize/deserialize implementation
- `src/components/tag.c` - NFC tag reading with magic word detection
- `src/system/host_spi.c` - Chunked SPI transfer handler
