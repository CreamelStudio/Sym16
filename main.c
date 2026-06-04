#include <stdio.h>

#include "sym16.h"

int main(void)
{
    const char* text = "abc123+";
    Symbol symbols[64];
    char decoded[64];
    char dump[512];
    const Symbol* found;
    size_t len;

    len = sym_encode(text, symbols, sizeof(symbols) / sizeof(symbols[0]));

    sym_dump(symbols, len, dump, sizeof(dump));
    puts(dump);

    sym_decode(symbols, len, decoded, sizeof(decoded));
    printf("decoded: %s\n", decoded);

    found = sym_find(symbols, len, sym_from_char('+'));
    if (found) {
        printf("found '+': index=%ld raw=0x%04X type=%s value=%u\n",
               (long)(found - symbols),
               (unsigned)*found,
               sym_type_name(sym_type(*found)),
               (unsigned)sym_value(*found));
    }

    return sym_compare(symbols, len, symbols, len);
}
