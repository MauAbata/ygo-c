#ifndef __ygo_sig_h
#define __ygo_sig_h

#ifdef __cplusplus
extern "C" {
#endif

#include "ygo_bin.h"
#include "ygo_card.h"
#include <stdint.h>

// Signature algorithm identifiers
#define YGO_SIG_ALG_ED25519 0x00

// Signature flags
#define YGO_SIG_FLAG_BOUND_DUELIST 0x01 // Signature includes duelist_id
#define YGO_SIG_FLAG_BOUND_DECK 0x02    // Signature includes deck_id
#define YGO_SIG_FLAG_SUPERSEDE 0x04     // Invalidates older signatures from same authority
#define YGO_SIG_FLAG_HAS_EXPIRY 0x08    // Expiry field is valid
#define YGO_SIG_FLAG_TOURNAMENT 0x10    // Tournament-level signature (EV+)

// Domain tag for canonical payload (prevents cross-protocol attacks)
#define YGO_SIG_DOMAIN_TAG "CYBSIGv1"

/**
 * Card signature record for certification and authentication.
 * Stored as BIN_RECORD_CARD_SIGNATURE (0x10).
 *
 * Size breakdown:
 * - Minimal (Level 1 - Shop Verified): 83 bytes
 * - Level 2 (+ Duelist binding): 99 bytes
 * - Level 3 (+ Deck binding): 115 bytes
 */
typedef struct {
    uint8_t version;         // 1 = initial spec
    uint8_t flags;           // Bitmask (see YGO_SIG_FLAG_*)
    uint8_t algorithm;       // Signature algorithm (0 = Ed25519)
    uint8_t authority_id[8]; // Fingerprint of signing authority's public key
    uint32_t timestamp;      // Unix timestamp (issuance time)
    uint32_t expiry;         // Unix timestamp (0 = no expiry)
    uint8_t duelist_id[16];  // Optional: present if flags & YGO_SIG_FLAG_BOUND_DUELIST
    uint8_t deck_id[16];     // Optional: present if flags & YGO_SIG_FLAG_BOUND_DECK
    uint8_t signature[64];   // Ed25519 signature over canonical payload
} ygo_card_signature_t;

/**
 * Read a signature record from binary buffer.
 * Respects flags to determine which optional fields are present.
 *
 * @param ctx Read context positioned at start of signature record data
 * @param sig Output signature structure
 * @return YGO_BIN_OK on success, error code otherwise
 */
ygo_bin_errno_t ygo_sig_read(ygo_bin_read_context_t *ctx, ygo_card_signature_t *sig);

/**
 * Write a signature record to binary buffer.
 * Only writes fields indicated by flags to minimize size.
 *
 * @param ctx Write context
 * @param sig Signature structure to serialize
 */
void ygo_sig_write(ygo_bin_write_context_t *ctx, const ygo_card_signature_t *sig);

/**
 * Build canonical payload for signing.
 * Creates a deterministic byte sequence from card data and signature metadata.
 *
 * Payload format:
 * - Domain tag: "CYBSIGv1" (8 bytes)
 * - Card ID: 4 bytes
 * - Card hash: 32 bytes (SHA-256 of BASIC record)
 * - Flags: 1 byte
 * - Timestamp: 4 bytes
 * - Expiry: 4 bytes
 * - Duelist ID: 0 or 16 bytes (if flag set)
 * - Deck ID: 0 or 16 bytes (if flag set)
 * - Authority ID: 8 bytes
 *
 * @param card Card data to sign
 * @param sigmeta Signature metadata (flags, IDs, timestamps)
 * @param payload_out Buffer to receive canonical payload (must be >= 93 bytes)
 * @param payload_len Output: actual payload length
 * @return 0 on success, negative error code otherwise
 */
int ygo_sig_build_payload(const ygo_card_t *card,
                          const ygo_card_signature_t *sigmeta,
                          uint8_t *payload_out,
                          size_t *payload_len);

/**
 * Calculate signature record size based on flags.
 * Useful for checking NTAG capacity before writing.
 *
 * @param flags Signature flags bitmask
 * @return Total size in bytes (including record header)
 */
size_t ygo_sig_calc_size(uint8_t flags);

/**
 * Verify signature (requires Ed25519 implementation - not included on AVR).
 * This function is a stub on embedded systems; actual verification happens
 * on cybermat-core or connected apps.
 *
 * @param card Card data
 * @param sig Signature record
 * @param pubkey Signing authority's public key (32 bytes)
 * @return 1 if valid, 0 if invalid, negative on error
 */
int ygo_sig_verify(const ygo_card_t *card,
                   const ygo_card_signature_t *sig,
                   const uint8_t pubkey[32]);

#ifdef __cplusplus
}
#endif

#endif
