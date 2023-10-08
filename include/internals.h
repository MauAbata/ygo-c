#ifndef __internals_h
#define __internals_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define _packed_ __attribute__((__packed__))
#else
#define _packed_ /* nothing */
#endif

#include <string.h>

#define LOGD(...) printf("DEBUG: " __VA_ARGS__)

#define ENUM_DEFS(id, name) id ,
#define ENUM_DEFS_VAL(id, name, value) id = ((unsigned) value ) ,
#define ENUM_DEFS_BITS(id, name, value) id = (0x1u << value ) ,

#define ENUM_CASE(id, name, ...) case id: return name;
#define ENUM_PARSE(id, name, ...) if (strcmp(str, name) == 0) return id;

#define ENUM_DECL(name, DEFS) \
    enum _packed_ name { \
        DEFS(ENUM_DEFS, ENUM_DEFS_VAL) \
    }; \
    typedef enum name name ## _t; \
    /** \
     * Parse a string into a name enum. Returns -1 if failure. \
     */ \
    name ## _t name ##_from_str(const char *str); \
    /** \
     * Return a string representation for the name enum. Returns "???" if no such value exists. \
     */ \
    const char *name ## _to_str(name ## _t val)

#define ENUM_DECL_BITS(name, DEFS) \
    enum _packed_ name { \
        DEFS(ENUM_DEFS_BITS) \
    }; \
    typedef enum name name ## _t; \
    /** \
     * Parse a string into a name enum. Returns -1 if failure. \
     */ \
    name ## _t name ##_from_str(const char *str); \
    /** \
     * Return a string representation for the name enum. Returns "???" if no such value exists. \
     */ \
    const char *name ## _to_str(name ## _t val)

#define ENUM_IMPL(name, DEFS) \
    name ## _t name ##_from_str(const char *str) { \
        DEFS(ENUM_PARSE, ENUM_PARSE); \
        LOGD("Could not determine " #name " from string: \"%s\"\n", str); \
        return -1; \
    } \
    const char *name ## _to_str(name ## _t val) { \
        switch (val) { \
            DEFS(ENUM_CASE, ENUM_CASE); \
            default: return "???"; \
        } \
    } \

#define ENUM_IMPL_BITS(name, DEFS) \
    name ## _t name ##_from_str(const char *str) { \
        DEFS(ENUM_PARSE); \
        LOGD("Could not determine " #name " from string: \"%s\"\n", str); \
        return -1; \
    } \
    const char *name ## _to_str(name ## _t val) { \
        switch (val) { \
            DEFS(ENUM_CASE); \
            default: return "???"; \
        } \
    } \

#ifdef __cplusplus
}
#endif

#endif
