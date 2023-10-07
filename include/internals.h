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

#ifdef __cplusplus
}
#endif

#endif
