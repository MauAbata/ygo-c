#include "ygo_sig.h"
#include <string.h>

ygo_bin_errno_t ygo_sig_read(ygo_bin_read_context_t *ctx, ygo_card_signature_t *sig) {
    if (ctx == NULL || sig == NULL) return YGO_BIN_ERR_BAD_ARGS;

    // Clear structure
    memset(sig, 0, sizeof(ygo_card_signature_t));

    // Read fixed fields
    ygo_bin_errno_t err;
    err = ygo_bin_read_int8(ctx, &sig->version);
    if (err != YGO_BIN_OK) return err;

    err = ygo_bin_read_int8(ctx, &sig->flags);
    if (err != YGO_BIN_OK) return err;

    err = ygo_bin_read_int8(ctx, &sig->algorithm);
    if (err != YGO_BIN_OK) return err;

    // Read authority_id (8 bytes)
    for (int i = 0; i < 8; i++) {
        err = ygo_bin_read_int8(ctx, &sig->authority_id[i]);
        if (err != YGO_BIN_OK) return err;
    }

    err = ygo_bin_read_int32(ctx, &sig->timestamp);
    if (err != YGO_BIN_OK) return err;

    err = ygo_bin_read_int32(ctx, &sig->expiry);
    if (err != YGO_BIN_OK) return err;

    // Read optional duelist_id if flag set
    if (sig->flags & YGO_SIG_FLAG_BOUND_DUELIST) {
        for (int i = 0; i < 16; i++) {
            err = ygo_bin_read_int8(ctx, &sig->duelist_id[i]);
            if (err != YGO_BIN_OK) return err;
        }
    }

    // Read optional deck_id if flag set
    if (sig->flags & YGO_SIG_FLAG_BOUND_DECK) {
        for (int i = 0; i < 16; i++) {
            err = ygo_bin_read_int8(ctx, &sig->deck_id[i]);
            if (err != YGO_BIN_OK) return err;
        }
    }

    // Read signature (64 bytes)
    for (int i = 0; i < 64; i++) {
        err = ygo_bin_read_int8(ctx, &sig->signature[i]);
        if (err != YGO_BIN_OK) return err;
    }

    return YGO_BIN_OK;
}

void ygo_sig_write(ygo_bin_write_context_t *ctx, const ygo_card_signature_t *sig) {
    if (ctx == NULL || sig == NULL) return;

    // Write fixed fields
    ygo_bin_write_int8(ctx, sig->version);
    ygo_bin_write_int8(ctx, sig->flags);
    ygo_bin_write_int8(ctx, sig->algorithm);

    // Write authority_id (8 bytes)
    ygo_bin_write_bytes(ctx, sig->authority_id, 8);

    ygo_bin_write_int32(ctx, sig->timestamp);
    ygo_bin_write_int32(ctx, sig->expiry);

    // Write optional duelist_id if flag set
    if (sig->flags & YGO_SIG_FLAG_BOUND_DUELIST) {
        ygo_bin_write_bytes(ctx, sig->duelist_id, 16);
    }

    // Write optional deck_id if flag set
    if (sig->flags & YGO_SIG_FLAG_BOUND_DECK) {
        ygo_bin_write_bytes(ctx, sig->deck_id, 16);
    }

    // Write signature (64 bytes)
    ygo_bin_write_bytes(ctx, sig->signature, 64);
}

int ygo_sig_build_payload(const ygo_card_t *card,
                          const ygo_card_signature_t *sigmeta,
                          uint8_t *payload_out,
                          size_t *payload_len) {
    if (card == NULL || sigmeta == NULL || payload_out == NULL || payload_len == NULL) {
        return -1;
    }

    size_t offset = 0;

    // Domain tag: "CYBSIGv1" (8 bytes)
    const char *domain_tag = YGO_SIG_DOMAIN_TAG;
    memcpy(payload_out + offset, domain_tag, 8);
    offset += 8;

    // Card ID (4 bytes, big-endian)
    payload_out[offset++] = (card->id >> 24) & 0xFF;
    payload_out[offset++] = (card->id >> 16) & 0xFF;
    payload_out[offset++] = (card->id >> 8) & 0xFF;
    payload_out[offset++] = card->id & 0xFF;

    // Card hash (32 bytes) - TODO: implement SHA-256 of serialized BASIC record
    // For now, use placeholder zeros; proper implementation requires SHA-256
    // on cybermat-core or signing tool
    memset(payload_out + offset, 0, 32);
    offset += 32;

    // Flags (1 byte)
    payload_out[offset++] = sigmeta->flags;

    // Timestamp (4 bytes, big-endian)
    payload_out[offset++] = (sigmeta->timestamp >> 24) & 0xFF;
    payload_out[offset++] = (sigmeta->timestamp >> 16) & 0xFF;
    payload_out[offset++] = (sigmeta->timestamp >> 8) & 0xFF;
    payload_out[offset++] = sigmeta->timestamp & 0xFF;

    // Expiry (4 bytes, big-endian)
    payload_out[offset++] = (sigmeta->expiry >> 24) & 0xFF;
    payload_out[offset++] = (sigmeta->expiry >> 16) & 0xFF;
    payload_out[offset++] = (sigmeta->expiry >> 8) & 0xFF;
    payload_out[offset++] = sigmeta->expiry & 0xFF;

    // Optional duelist_id (16 bytes if flag set)
    if (sigmeta->flags & YGO_SIG_FLAG_BOUND_DUELIST) {
        memcpy(payload_out + offset, sigmeta->duelist_id, 16);
        offset += 16;
    }

    // Optional deck_id (16 bytes if flag set)
    if (sigmeta->flags & YGO_SIG_FLAG_BOUND_DECK) {
        memcpy(payload_out + offset, sigmeta->deck_id, 16);
        offset += 16;
    }

    // Authority ID (8 bytes)
    memcpy(payload_out + offset, sigmeta->authority_id, 8);
    offset += 8;

    *payload_len = offset;
    return 0;
}

size_t ygo_sig_calc_size(uint8_t flags) {
    // Base size: version(1) + flags(1) + alg(1) + authority_id(8) +
    //            timestamp(4) + expiry(4) + signature(64)
    size_t size = 1 + 1 + 1 + 8 + 4 + 4 + 64; // 83 bytes

    if (flags & YGO_SIG_FLAG_BOUND_DUELIST) {
        size += 16;
    }

    if (flags & YGO_SIG_FLAG_BOUND_DECK) {
        size += 16;
    }

    // Add record header overhead (type, version, length field, CRC16, padding)
    // Actual overhead depends on ygo_bin record framing (~4-8 bytes)
    size += 8; // Conservative estimate

    return size;
}

int ygo_sig_verify(const ygo_card_t *card,
                   const ygo_card_signature_t *sig,
                   const uint8_t pubkey[32]) {
    // Stub implementation for AVR (no Ed25519 verification on embedded)
    // Actual verification happens on cybermat-core or connected apps
    // Return -1 to indicate "not implemented on this platform"
    (void)card;
    (void)sig;
    (void)pubkey;
    return -1;
}
