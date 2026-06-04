#ifndef SYM16_H
#define SYM16_H

/*
 * sym16.h - C99 header-only 16-bit symbol encoding library.
 *
 * This is the default SIMPLE profile:
 *   - Value stores the original byte value.
 *   - Easy to inspect and round-trip.
 *   - Good first choice unless profiling says otherwise.
 *
 * Layout:
 *   [ high 4 bits: type ][ low 12 bits: value ]
 *
 * This header intentionally depends only on stdint.h, stdbool.h, and
 * stddef.h so it can be dropped into small C projects and embedded code.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SYM16_MODE_FAST) && defined(SYM16_MODE_COMPACT)
#error "Choose only one Sym16 mode header."
#endif

#if !defined(SYM16_MODE_FAST) && !defined(SYM16_MODE_COMPACT)
#define SYM16_MODE_SIMPLE 1
#endif

typedef uint16_t Symbol;

enum {
    SYM_NULL     = 0,
    SYM_DIGIT    = 1,
    SYM_UPPER    = 2,
    SYM_LOWER    = 3,
    SYM_SPACE    = 4,
    SYM_OPERATOR = 5,
    SYM_SPECIAL  = 6,
    SYM_HANGUL   = 7
};

#define SYM_TYPE_SHIFT 12u
#define SYM_TYPE_MASK  ((uint16_t)0xF000u)
#define SYM_VALUE_MASK ((uint16_t)0x0FFFu)

#define SYM_TYPE(x)  ((uint8_t)((((Symbol)(x)) & SYM_TYPE_MASK) >> SYM_TYPE_SHIFT))
#define SYM_VALUE(x) ((uint16_t)(((Symbol)(x)) & SYM_VALUE_MASK))

#define SYM16_MAKE_CONST(type, value) \
    ((Symbol)(((((uint16_t)(type)) << SYM_TYPE_SHIFT) & SYM_TYPE_MASK) | \
              (((uint16_t)(value)) & SYM_VALUE_MASK)))

#define SYM16_VERSION_MAJOR 1
#define SYM16_VERSION_MINOR 1
#define SYM16_VERSION_PATCH 0

#if defined(SYM16_MODE_FAST)
#define SYM16_PROFILE_NAME "fast"
#elif defined(SYM16_MODE_COMPACT)
#define SYM16_PROFILE_NAME "compact"
#else
#define SYM16_PROFILE_NAME "simple"
#endif

static inline Symbol sym_make(uint8_t type, uint16_t value)
{
    return (Symbol)((((uint16_t)type << SYM_TYPE_SHIFT) & SYM_TYPE_MASK) |
                    (value & SYM_VALUE_MASK));
}

static inline uint8_t sym_type(Symbol s)
{
    return SYM_TYPE(s);
}

static inline uint16_t sym_value(Symbol s)
{
    return SYM_VALUE(s);
}

static inline bool sym_is_digit(Symbol s)
{
    return sym_type(s) == SYM_DIGIT;
}

static inline bool sym_is_upper(Symbol s)
{
    return sym_type(s) == SYM_UPPER;
}

static inline bool sym_is_lower(Symbol s)
{
    return sym_type(s) == SYM_LOWER;
}

static inline bool sym_is_space(Symbol s)
{
    return sym_type(s) == SYM_SPACE;
}

static inline bool sym_is_operator(Symbol s)
{
    return sym_type(s) == SYM_OPERATOR;
}

static inline bool sym_is_special(Symbol s)
{
    return sym_type(s) == SYM_SPECIAL;
}

static inline bool sym_is_hangul(Symbol s)
{
    return sym_type(s) == SYM_HANGUL;
}

static inline bool sym16_is_digit_byte(uint8_t c)
{
    return c >= (uint8_t)'0' && c <= (uint8_t)'9';
}

static inline bool sym16_is_upper_byte(uint8_t c)
{
    return c >= (uint8_t)'A' && c <= (uint8_t)'Z';
}

static inline bool sym16_is_lower_byte(uint8_t c)
{
    return c >= (uint8_t)'a' && c <= (uint8_t)'z';
}

static inline bool sym16_is_space_byte(uint8_t c)
{
    return c == (uint8_t)' '  || c == (uint8_t)'\t' ||
           c == (uint8_t)'\n' || c == (uint8_t)'\r' ||
           c == (uint8_t)'\v' || c == (uint8_t)'\f';
}

static inline bool sym16_is_operator_byte(uint8_t c)
{
    switch (c) {
    case '+': case '-': case '*': case '/': case '%':
    case '=': case '<': case '>': case '!':
    case '&': case '|': case '^': case '~':
    case '?': case ':': case '.':
        return true;
    default:
        return false;
    }
}

static inline uint8_t sym_classify_byte(uint8_t c)
{
    if (sym16_is_digit_byte(c)) {
        return SYM_DIGIT;
    }
    if (sym16_is_upper_byte(c)) {
        return SYM_UPPER;
    }
    if (sym16_is_lower_byte(c)) {
        return SYM_LOWER;
    }
    if (sym16_is_space_byte(c)) {
        return SYM_SPACE;
    }
    if (sym16_is_operator_byte(c)) {
        return SYM_OPERATOR;
    }
    return SYM_SPECIAL;
}

#if defined(SYM16_MODE_FAST)
static const Symbol sym16_fast_table[256] = {
    ['0'] = SYM16_MAKE_CONST(SYM_DIGIT, '0'),
    ['1'] = SYM16_MAKE_CONST(SYM_DIGIT, '1'),
    ['2'] = SYM16_MAKE_CONST(SYM_DIGIT, '2'),
    ['3'] = SYM16_MAKE_CONST(SYM_DIGIT, '3'),
    ['4'] = SYM16_MAKE_CONST(SYM_DIGIT, '4'),
    ['5'] = SYM16_MAKE_CONST(SYM_DIGIT, '5'),
    ['6'] = SYM16_MAKE_CONST(SYM_DIGIT, '6'),
    ['7'] = SYM16_MAKE_CONST(SYM_DIGIT, '7'),
    ['8'] = SYM16_MAKE_CONST(SYM_DIGIT, '8'),
    ['9'] = SYM16_MAKE_CONST(SYM_DIGIT, '9'),

    ['A'] = SYM16_MAKE_CONST(SYM_UPPER, 'A'),
    ['B'] = SYM16_MAKE_CONST(SYM_UPPER, 'B'),
    ['C'] = SYM16_MAKE_CONST(SYM_UPPER, 'C'),
    ['D'] = SYM16_MAKE_CONST(SYM_UPPER, 'D'),
    ['E'] = SYM16_MAKE_CONST(SYM_UPPER, 'E'),
    ['F'] = SYM16_MAKE_CONST(SYM_UPPER, 'F'),
    ['G'] = SYM16_MAKE_CONST(SYM_UPPER, 'G'),
    ['H'] = SYM16_MAKE_CONST(SYM_UPPER, 'H'),
    ['I'] = SYM16_MAKE_CONST(SYM_UPPER, 'I'),
    ['J'] = SYM16_MAKE_CONST(SYM_UPPER, 'J'),
    ['K'] = SYM16_MAKE_CONST(SYM_UPPER, 'K'),
    ['L'] = SYM16_MAKE_CONST(SYM_UPPER, 'L'),
    ['M'] = SYM16_MAKE_CONST(SYM_UPPER, 'M'),
    ['N'] = SYM16_MAKE_CONST(SYM_UPPER, 'N'),
    ['O'] = SYM16_MAKE_CONST(SYM_UPPER, 'O'),
    ['P'] = SYM16_MAKE_CONST(SYM_UPPER, 'P'),
    ['Q'] = SYM16_MAKE_CONST(SYM_UPPER, 'Q'),
    ['R'] = SYM16_MAKE_CONST(SYM_UPPER, 'R'),
    ['S'] = SYM16_MAKE_CONST(SYM_UPPER, 'S'),
    ['T'] = SYM16_MAKE_CONST(SYM_UPPER, 'T'),
    ['U'] = SYM16_MAKE_CONST(SYM_UPPER, 'U'),
    ['V'] = SYM16_MAKE_CONST(SYM_UPPER, 'V'),
    ['W'] = SYM16_MAKE_CONST(SYM_UPPER, 'W'),
    ['X'] = SYM16_MAKE_CONST(SYM_UPPER, 'X'),
    ['Y'] = SYM16_MAKE_CONST(SYM_UPPER, 'Y'),
    ['Z'] = SYM16_MAKE_CONST(SYM_UPPER, 'Z'),

    ['a'] = SYM16_MAKE_CONST(SYM_LOWER, 'a'),
    ['b'] = SYM16_MAKE_CONST(SYM_LOWER, 'b'),
    ['c'] = SYM16_MAKE_CONST(SYM_LOWER, 'c'),
    ['d'] = SYM16_MAKE_CONST(SYM_LOWER, 'd'),
    ['e'] = SYM16_MAKE_CONST(SYM_LOWER, 'e'),
    ['f'] = SYM16_MAKE_CONST(SYM_LOWER, 'f'),
    ['g'] = SYM16_MAKE_CONST(SYM_LOWER, 'g'),
    ['h'] = SYM16_MAKE_CONST(SYM_LOWER, 'h'),
    ['i'] = SYM16_MAKE_CONST(SYM_LOWER, 'i'),
    ['j'] = SYM16_MAKE_CONST(SYM_LOWER, 'j'),
    ['k'] = SYM16_MAKE_CONST(SYM_LOWER, 'k'),
    ['l'] = SYM16_MAKE_CONST(SYM_LOWER, 'l'),
    ['m'] = SYM16_MAKE_CONST(SYM_LOWER, 'm'),
    ['n'] = SYM16_MAKE_CONST(SYM_LOWER, 'n'),
    ['o'] = SYM16_MAKE_CONST(SYM_LOWER, 'o'),
    ['p'] = SYM16_MAKE_CONST(SYM_LOWER, 'p'),
    ['q'] = SYM16_MAKE_CONST(SYM_LOWER, 'q'),
    ['r'] = SYM16_MAKE_CONST(SYM_LOWER, 'r'),
    ['s'] = SYM16_MAKE_CONST(SYM_LOWER, 's'),
    ['t'] = SYM16_MAKE_CONST(SYM_LOWER, 't'),
    ['u'] = SYM16_MAKE_CONST(SYM_LOWER, 'u'),
    ['v'] = SYM16_MAKE_CONST(SYM_LOWER, 'v'),
    ['w'] = SYM16_MAKE_CONST(SYM_LOWER, 'w'),
    ['x'] = SYM16_MAKE_CONST(SYM_LOWER, 'x'),
    ['y'] = SYM16_MAKE_CONST(SYM_LOWER, 'y'),
    ['z'] = SYM16_MAKE_CONST(SYM_LOWER, 'z'),

    [' ']  = SYM16_MAKE_CONST(SYM_SPACE, ' '),
    ['\t'] = SYM16_MAKE_CONST(SYM_SPACE, '\t'),
    ['\n'] = SYM16_MAKE_CONST(SYM_SPACE, '\n'),
    ['\r'] = SYM16_MAKE_CONST(SYM_SPACE, '\r'),
    ['\v'] = SYM16_MAKE_CONST(SYM_SPACE, '\v'),
    ['\f'] = SYM16_MAKE_CONST(SYM_SPACE, '\f'),

    ['+'] = SYM16_MAKE_CONST(SYM_OPERATOR, '+'),
    ['-'] = SYM16_MAKE_CONST(SYM_OPERATOR, '-'),
    ['*'] = SYM16_MAKE_CONST(SYM_OPERATOR, '*'),
    ['/'] = SYM16_MAKE_CONST(SYM_OPERATOR, '/'),
    ['%'] = SYM16_MAKE_CONST(SYM_OPERATOR, '%'),
    ['='] = SYM16_MAKE_CONST(SYM_OPERATOR, '='),
    ['<'] = SYM16_MAKE_CONST(SYM_OPERATOR, '<'),
    ['>'] = SYM16_MAKE_CONST(SYM_OPERATOR, '>'),
    ['!'] = SYM16_MAKE_CONST(SYM_OPERATOR, '!'),
    ['&'] = SYM16_MAKE_CONST(SYM_OPERATOR, '&'),
    ['|'] = SYM16_MAKE_CONST(SYM_OPERATOR, '|'),
    ['^'] = SYM16_MAKE_CONST(SYM_OPERATOR, '^'),
    ['~'] = SYM16_MAKE_CONST(SYM_OPERATOR, '~'),
    ['?'] = SYM16_MAKE_CONST(SYM_OPERATOR, '?'),
    [':'] = SYM16_MAKE_CONST(SYM_OPERATOR, ':'),
    ['.'] = SYM16_MAKE_CONST(SYM_OPERATOR, '.')
};
#endif

#if defined(SYM16_MODE_COMPACT)
enum {
    SYM16_SPACE_SPACE = 0,
    SYM16_SPACE_TAB   = 1,
    SYM16_SPACE_LF    = 2,
    SYM16_SPACE_CR    = 3,
    SYM16_SPACE_VT    = 4,
    SYM16_SPACE_FF    = 5
};

static inline uint16_t sym16_compact_space_value(uint8_t c)
{
    switch (c) {
    case ' ':
        return SYM16_SPACE_SPACE;
    case '\t':
        return SYM16_SPACE_TAB;
    case '\n':
        return SYM16_SPACE_LF;
    case '\r':
        return SYM16_SPACE_CR;
    case '\v':
        return SYM16_SPACE_VT;
    default:
        return SYM16_SPACE_FF;
    }
}

static inline char sym16_compact_space_char(uint16_t value)
{
    switch (value) {
    case SYM16_SPACE_SPACE:
        return ' ';
    case SYM16_SPACE_TAB:
        return '\t';
    case SYM16_SPACE_LF:
        return '\n';
    case SYM16_SPACE_CR:
        return '\r';
    case SYM16_SPACE_VT:
        return '\v';
    case SYM16_SPACE_FF:
        return '\f';
    default:
        return '?';
    }
}

static inline uint16_t sym16_compact_operator_value(uint8_t c)
{
    switch (c) {
    case '+':
        return 0;
    case '-':
        return 1;
    case '*':
        return 2;
    case '/':
        return 3;
    case '%':
        return 4;
    case '=':
        return 5;
    case '<':
        return 6;
    case '>':
        return 7;
    case '!':
        return 8;
    case '&':
        return 9;
    case '|':
        return 10;
    case '^':
        return 11;
    case '~':
        return 12;
    case '?':
        return 13;
    case ':':
        return 14;
    default:
        return 15;
    }
}

static inline char sym16_compact_operator_char(uint16_t value)
{
    switch (value) {
    case 0:
        return '+';
    case 1:
        return '-';
    case 2:
        return '*';
    case 3:
        return '/';
    case 4:
        return '%';
    case 5:
        return '=';
    case 6:
        return '<';
    case 7:
        return '>';
    case 8:
        return '!';
    case 9:
        return '&';
    case 10:
        return '|';
    case 11:
        return '^';
    case 12:
        return '~';
    case 13:
        return '?';
    case 14:
        return ':';
    case 15:
        return '.';
    default:
        return '?';
    }
}
#endif

static inline Symbol sym_from_char(char c)
{
    uint8_t b = (uint8_t)c;

#if defined(SYM16_MODE_FAST)
    Symbol cached = sym16_fast_table[b];
    return cached != (Symbol)0 ? cached : sym_make(SYM_SPECIAL, (uint16_t)b);
#elif defined(SYM16_MODE_COMPACT)
    if (sym16_is_digit_byte(b)) {
        return sym_make(SYM_DIGIT, (uint16_t)(b - (uint8_t)'0'));
    }
    if (sym16_is_upper_byte(b)) {
        return sym_make(SYM_UPPER, (uint16_t)(b - (uint8_t)'A'));
    }
    if (sym16_is_lower_byte(b)) {
        return sym_make(SYM_LOWER, (uint16_t)(b - (uint8_t)'a'));
    }
    if (sym16_is_space_byte(b)) {
        return sym_make(SYM_SPACE, sym16_compact_space_value(b));
    }
    if (sym16_is_operator_byte(b)) {
        return sym_make(SYM_OPERATOR, sym16_compact_operator_value(b));
    }
    return sym_make(SYM_SPECIAL, (uint16_t)b);
#else
    return sym_make(sym_classify_byte(b), (uint16_t)b);
#endif
}

static inline char sym_to_char(Symbol s)
{
    uint16_t v = sym_value(s);

#if defined(SYM16_MODE_COMPACT)
    switch (sym_type(s)) {
    case SYM_DIGIT:
        return v < 10u ? (char)('0' + v) : '?';
    case SYM_UPPER:
        return v < 26u ? (char)('A' + v) : '?';
    case SYM_LOWER:
        return v < 26u ? (char)('a' + v) : '?';
    case SYM_SPACE:
        return sym16_compact_space_char(v);
    case SYM_OPERATOR:
        return sym16_compact_operator_char(v);
    case SYM_SPECIAL:
        return v <= 0xFFu ? (char)(uint8_t)v : '?';
    default:
        return '?';
    }
#else
    return v <= 0xFFu ? (char)(uint8_t)v : '?';
#endif
}

static inline const char* sym_type_name(uint8_t type)
{
    switch (type) {
    case SYM_NULL:
        return "NULL";
    case SYM_DIGIT:
        return "DIGIT";
    case SYM_UPPER:
        return "UPPER";
    case SYM_LOWER:
        return "LOWER";
    case SYM_SPACE:
        return "SPACE";
    case SYM_OPERATOR:
        return "OP";
    case SYM_SPECIAL:
        return "SPECIAL";
    case SYM_HANGUL:
        return "HANGUL";
    default:
        return "UNKNOWN";
    }
}

static inline size_t sym_encode(const char* input, Symbol* output, size_t max_output)
{
    size_t n = 0;

    if (!input || !output) {
        return 0;
    }

    while (input[n] != '\0' && n < max_output) {
        output[n] = sym_from_char(input[n]);
        ++n;
    }

    return n;
}

static inline size_t sym_decode(const Symbol* input,
                                size_t length,
                                char* output,
                                size_t max_output)
{
    size_t n = 0;

    if (!input || !output || max_output == 0) {
        return 0;
    }

    while (n < length && n + 1 < max_output) {
        output[n] = sym_to_char(input[n]);
        ++n;
    }

    output[n] = '\0';
    return n;
}

static inline int sym_compare(const Symbol* a,
                              size_t len_a,
                              const Symbol* b,
                              size_t len_b)
{
    size_t i = 0;
    size_t min_len = len_a < len_b ? len_a : len_b;

    if (a == b && len_a == len_b) {
        return 0;
    }
    if (!a && len_a != 0) {
        return -1;
    }
    if (!b && len_b != 0) {
        return 1;
    }

    for (i = 0; i < min_len; ++i) {
        if (a[i] < b[i]) {
            return -1;
        }
        if (a[i] > b[i]) {
            return 1;
        }
    }

    if (len_a < len_b) {
        return -1;
    }
    if (len_a > len_b) {
        return 1;
    }
    return 0;
}

static inline const Symbol* sym_find(const Symbol* data, size_t len, Symbol target)
{
    size_t i = 0;

    if (!data) {
        return (const Symbol*)0;
    }

    for (i = 0; i < len; ++i) {
        if (data[i] == target) {
            return data + i;
        }
    }

    return (const Symbol*)0;
}

static inline size_t sym16_cstr_len(const char* s)
{
    size_t n = 0;

    if (!s) {
        return 0;
    }

    while (s[n] != '\0') {
        ++n;
    }

    return n;
}

static inline size_t sym16_write_char(char* output,
                                      size_t max_output,
                                      size_t pos,
                                      char c)
{
    if (output && pos + 1 < max_output) {
        output[pos] = c;
    }
    return pos + 1;
}

static inline size_t sym16_write_cstr(char* output,
                                      size_t max_output,
                                      size_t pos,
                                      const char* text)
{
    size_t i = 0;

    if (!text) {
        return pos;
    }

    while (text[i] != '\0') {
        pos = sym16_write_char(output, max_output, pos, text[i]);
        ++i;
    }

    return pos;
}

static inline char sym16_hex_digit(uint8_t value)
{
    value = (uint8_t)(value & 0x0Fu);
    return value < 10u ? (char)('0' + value) : (char)('A' + (value - 10u));
}

static inline size_t sym16_write_u16_hex(char* output,
                                         size_t max_output,
                                         size_t pos,
                                         uint16_t value)
{
    pos = sym16_write_cstr(output, max_output, pos, "0x");
    pos = sym16_write_char(output, max_output, pos,
                           sym16_hex_digit((uint8_t)(value >> 12)));
    pos = sym16_write_char(output, max_output, pos,
                           sym16_hex_digit((uint8_t)(value >> 8)));
    pos = sym16_write_char(output, max_output, pos,
                           sym16_hex_digit((uint8_t)(value >> 4)));
    pos = sym16_write_char(output, max_output, pos,
                           sym16_hex_digit((uint8_t)value));
    return pos;
}

static inline char sym16_visible_char(Symbol s)
{
    char c = sym_to_char(s);
    uint8_t v = (uint8_t)c;

    if (v >= 32u && v <= 126u) {
        return c;
    }
    return '?';
}

/*
 * Formats one symbol as "[TYPE:value]".
 * Returns the number of characters that would be written, excluding the
 * trailing NUL. The output is always NUL-terminated when max_output > 0.
 */
static inline size_t sym_format(Symbol s, char* output, size_t max_output)
{
    size_t pos = 0;
    uint8_t type = sym_type(s);
    char c = sym_to_char(s);
    uint8_t visible = (uint8_t)c;

    pos = sym16_write_char(output, max_output, pos, '[');
    pos = sym16_write_cstr(output, max_output, pos, sym_type_name(type));
    pos = sym16_write_char(output, max_output, pos, ':');

    if (type == SYM_SPACE) {
        switch (c) {
        case ' ':
            pos = sym16_write_cstr(output, max_output, pos, "space");
            break;
        case '\t':
            pos = sym16_write_cstr(output, max_output, pos, "\\t");
            break;
        case '\n':
            pos = sym16_write_cstr(output, max_output, pos, "\\n");
            break;
        case '\r':
            pos = sym16_write_cstr(output, max_output, pos, "\\r");
            break;
        default:
            pos = sym16_write_u16_hex(output, max_output, pos, sym_value(s));
            break;
        }
    } else if (visible >= 32u && visible <= 126u) {
        pos = sym16_write_char(output, max_output, pos, c);
    } else {
        pos = sym16_write_u16_hex(output, max_output, pos, sym_value(s));
    }

    pos = sym16_write_char(output, max_output, pos, ']');

    if (output && max_output > 0) {
        output[pos < max_output ? pos : max_output - 1u] = '\0';
    }

    return pos;
}

static inline size_t sym_to_string(Symbol s, char* output, size_t max_output)
{
    return sym_format(s, output, max_output);
}

/*
 * Dumps many symbols as one symbol per line:
 *   [LOWER:a]
 *   [DIGIT:1]
 * Returns the number of characters that would be written, excluding the
 * trailing NUL. The output is always NUL-terminated when max_output > 0.
 */
static inline size_t sym_dump(const Symbol* input,
                              size_t length,
                              char* output,
                              size_t max_output)
{
    size_t i = 0;
    size_t pos = 0;
    char one[32];

    if (!input) {
        if (output && max_output > 0) {
            output[0] = '\0';
        }
        return 0;
    }

    for (i = 0; i < length; ++i) {
        size_t j = 0;
        sym_format(input[i], one, sizeof(one));

        while (one[j] != '\0') {
            pos = sym16_write_char(output, max_output, pos, one[j]);
            ++j;
        }

        if (i + 1 < length) {
            pos = sym16_write_char(output, max_output, pos, '\n');
        }
    }

    if (output && max_output > 0) {
        output[pos < max_output ? pos : max_output - 1u] = '\0';
    }

    return pos;
}

#ifdef __cplusplus
}
#endif

#endif /* SYM16_H */
