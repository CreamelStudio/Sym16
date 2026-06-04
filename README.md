# Sym16

Korean documentation: [README.ko.md](README.ko.md)

`sym16.h` is a C99 header-only library that encodes each input byte into a
16-bit `Symbol`.

```c
[ high 4 bits: Type ][ low 12 bits: Value ]
```

The library uses no `malloc`, has no `.c` file, and the core header depends
only on `stdint.h`, `stdbool.h`, and `stddef.h`.

## Quick Start

```c
#include "sym16.h"

Symbol out[32];
size_t len = sym_encode("abc123+", out, 32);
```

Example dump:

```text
[LOWER:a]
[LOWER:b]
[LOWER:c]
[DIGIT:1]
[DIGIT:2]
[DIGIT:3]
[OP:+]
```

Build the example:

```sh
cc -std=c99 -Wall -Wextra -pedantic main.c -o sym16_example
./sym16_example
```

## Pick One Header

Choose exactly one profile header per translation unit.

```c
/* Pick one. Do not include all three. */
#include "sym16.h"          /* Simple default profile */
/* #include "sym16_fast.h" */    /* Faster ASCII encode profile */
/* #include "sym16_compact.h" */ /* Compact value profile */
```

All three expose the same API names, so normal code still calls
`sym_encode`, `sym_decode`, `sym_type`, `sym_value`, and friends.

That means you can start with the default `sym16.h` while writing tests and
prototypes, then later switch only the include line to `sym16_fast.h` or
`sym16_compact.h` when the right profile becomes clear. The function names stay
the same; only the internal encoding strategy changes. For compact mode, check
any code that directly depends on raw `sym_value()` numbers because those values
are intentionally remapped.

| Header | Value meaning | Best for |
| --- | --- | --- |
| `sym16.h` | Original byte value, such as `'a' == 97` | Learning, debugging, round-trip text, safest default |
| `sym16_fast.h` | Same as `sym16.h`, but ASCII classes use a lookup table | High-throughput repeated encoding of ASCII-heavy text |
| `sym16_compact.h` | Semantic index, such as digit `'7' == 7`, lower `'c' == 2`, operator `'+' == 0` | Lexer/parser logic that wants compact category-local values |

Do not include more than one Sym16 profile in the same `.c` file. They share
the same public API names by design.

## Header Decision Guide

Use `sym16.h` when:

- You are learning the library or prototyping.
- You want the easiest debugging experience.
- You need `sym_value(sym_from_char('a'))` to be the original byte value `97`.
- You want to encode and decode ASCII text with the least surprising behavior.
- You are not sure which profile to choose yet.

Use `sym16_fast.h` when:

- Encoding speed is more important than a tiny static lookup table.
- Your input is mostly ASCII.
- You repeatedly encode many strings or large buffers.
- You are building a scanner, validator, or preprocessor where input
  classification is a hot path.
- You still want `Value` to mean the original byte, just like `sym16.h`.

Use `sym16_compact.h` when:

- Later logic cares more about meaning than original byte values.
- You want digit values directly, such as `'7' -> 7`.
- You want alphabet indices directly, such as `'c' -> 2`.
- You want operator IDs directly, such as `'+' -> 0`.
- You are building lexer/parser rules that branch on `Type` first and then use
  small category-local `Value` numbers.

Quick examples:

| Project | Recommended header | Why |
| --- | --- | --- |
| First Sym16 experiment | `sym16.h` | Most readable and least surprising |
| Debug dump tool | `sym16.h` | Values match original bytes |
| ASCII-heavy log scanner | `sym16_fast.h` | Repeated classification can benefit from lookup |
| Source-code lexer | `sym16_fast.h` or `sym16_compact.h` | Fast for raw scanning, compact for semantic token rules |
| Calculator parser | `sym16_compact.h` | Digits and operators become small direct values |
| Embedded fixed-buffer parser | `sym16_compact.h` | No heap, predictable symbols, compact category values |
| Plain text storage | None; use `char` | `Symbol` doubles ASCII storage |
| Full Unicode text engine | None by itself | Needs a UTF-8/codepoint layer first |

## Memory Layout

`Symbol` is `uint16_t`.

```text
15            12 11                         0
+---------------+---------------------------+
| Type, 4 bits   | Value, 12 bits            |
+---------------+---------------------------+
```

`sym_make(type, value)` masks both fields, so `type` is limited to 4 bits and
`value` is limited to 12 bits. ASCII bytes fit directly in `Value`.

In the compact profile, `Value` is not always the original ASCII byte. It is a
small semantic index for digits, letters, spaces, and operators. `sym_decode`
and `sym_dump` still reconstruct the visible character for supported ASCII
input.

## Custom Symbols

Types `8` through `15` are reserved for user-defined symbols:

```c
SYM_CUSTOM0
SYM_CUSTOM1
SYM_CUSTOM2
SYM_CUSTOM3
SYM_CUSTOM4
SYM_CUSTOM5
SYM_CUSTOM6
SYM_CUSTOM7
```

Use them when your parser needs symbols that do not come directly from one
input character, such as token markers, virtual separators, macro placeholders,
AST hints, or domain-specific states.

```c
enum {
    MY_TOKEN_IDENTIFIER = 1,
    MY_TOKEN_NUMBER = 2
};

Symbol id = sym_make_custom(0, MY_TOKEN_IDENTIFIER);
Symbol num = sym_make_custom(0, MY_TOKEN_NUMBER);

if (sym_is_custom(id) && sym_custom_type(id) == 0) {
    /* handle custom token */
}
```

Important limits:

- `sym_encode()` does not create custom symbols automatically.
- `sym_make_custom(custom_type, value)` accepts `custom_type` from `0` to `7`.
- Invalid custom types return `SYM_NULL`.
- `value` is still 12 bits, so it ranges from `0` to `4095`.
- Custom symbols are not text; `sym_decode()` writes `?` for them.
- `sym_dump()` prints custom values as hex, for example `[CUSTOM0:0x0001]`.

This is intentionally not a registry system. There is no global name table, no
callbacks, and no allocation. If you need names for custom values, keep that
mapping in your lexer/parser layer.

## API

- `sym_make(type, value)`
- `sym_make_custom(custom_type, value)`
- `sym_type(symbol)`
- `sym_value(symbol)`
- `sym_is_custom(symbol)`
- `sym_is_custom_type(custom_type)`
- `sym_custom_type(symbol)`
- `sym_is_digit(symbol)`
- `sym_is_upper(symbol)`
- `sym_is_lower(symbol)`
- `sym_is_space(symbol)`
- `sym_is_operator(symbol)`
- `sym_encode(input, output, max_output)`
- `sym_decode(input, length, output, max_output)`
- `sym_compare(a, len_a, b, len_b)`
- `sym_find(data, len, target)`
- `sym_type_name(type)`
- `sym_format(symbol, output, max_output)`
- `sym_dump(input, length, output, max_output)`

## Where It Helps

Sym16 is not a faster replacement for plain `char` strings. A `char` is 1 byte;
a `Symbol` is 2 bytes. If you only store text, print text, or scan it once,
plain C strings are usually better.

Sym16 becomes useful when code repeatedly asks "what kind of character is
this?" after the first read.

Good fits:

- Lexers and parsers that scan the same source buffer multiple times.
- DSL interpreters where digits, identifiers, whitespace, and operators are
  the important units.
- Syntax highlighters or validators that benefit from a pre-classified stream.
- Embedded or allocation-free pipelines that want fixed-width symbols and no
  heap use.
- Token pre-processing where `sym_type(s)` replaces repeated classification
  branches.

Poor fits:

- Plain text storage.
- One-shot file copy or printing.
- Full Unicode text representation without an additional UTF-8/codepoint layer.
- Memory-constrained workloads where doubling ASCII storage is unacceptable.

## Performance

The core operations are constant-time bit shifts and masks.

- `sym_make`, `sym_type`, `sym_value`: O(1)
- type checks: O(1)
- encode/decode: O(n), one linear pass
- compare: O(min(n, m))
- find: O(n)

All functions are `static inline`, so optimizing compilers can remove normal
function-call overhead in hot paths.

Profile tradeoffs:

| Profile | Encode cost | Memory cost | Notes |
| --- | --- | --- | --- |
| Simple | Several predictable branches per byte | 2 bytes per symbol | Easiest to inspect because value is the original byte |
| Fast | Lookup table plus fallback per byte | 2 bytes per symbol plus a small static table per `.c` file | Best when encoding ASCII-heavy input repeatedly |
| Compact | Branches plus value remapping | 2 bytes per symbol | Useful when later logic wants numeric digit/letter/operator indices |

The fast profile can reduce branch work during encoding, but it adds a static
256-entry `Symbol` table in each translation unit that includes it. That is
about 512 bytes before compiler/linker optimizations.

## Extension Ideas

UTF-8 can be supported in two common ways:

- Byte-preserving mode: keep each UTF-8 byte as one `SYM_SPECIAL` symbol.
- Codepoint mode: decode UTF-8 first, then map codepoints into one or more
  symbols.

Hangul syllables require more than 12 bits if every modern syllable is stored
directly. Practical options are:

- Store Hangul jamo indices in `SYM_HANGUL`.
- Use multiple symbols for one Unicode codepoint.
- Keep an external table and store table indices in the 12-bit value field.

For lexer integration, encode source text into `Symbol` first, then write token
rules against `sym_type()` and `sym_value()`. Operators already have a dedicated
type, which makes simple tokenizers compact.
