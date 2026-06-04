#ifndef SYM16_COMPACT_H
#define SYM16_COMPACT_H

/*
 * Compact value profile for Sym16.
 *
 * Include this instead of sym16.h when you want values like digit 7 -> 7,
 * lower 'c' -> 2, and operator '+' -> 0.
 */

#ifdef SYM16_H
#error "Choose only one Sym16 header: sym16.h, sym16_fast.h, or sym16_compact.h."
#endif

#define SYM16_MODE_COMPACT 1
#include "sym16.h"

#endif /* SYM16_COMPACT_H */
