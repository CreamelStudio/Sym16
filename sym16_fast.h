#ifndef SYM16_FAST_H
#define SYM16_FAST_H

/*
 * Fast profile for Sym16.
 *
 * Include this instead of sym16.h when encode throughput matters more than
 * the small per-translation-unit lookup table.
 */

#ifdef SYM16_H
#error "Choose only one Sym16 header: sym16.h, sym16_fast.h, or sym16_compact.h."
#endif

#define SYM16_MODE_FAST 1
#include "sym16.h"

#endif /* SYM16_FAST_H */
