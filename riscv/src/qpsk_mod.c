#include "../include/qpsk_mod.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

void bi2de_pairs(const uint8_t *bits, int num_bits, uint8_t *out_symbols) {
    int num_symbols = num_bits / 2;
    for (int i = 0; i < num_symbols; ++i) {
        uint8_t high = bits[2 * i];
        uint8_t low  = bits[2 * i + 1];
        out_symbols[i] = (high << 1) | low;
    }
}

void qammod_qpsk(const uint8_t *symbols, int num_symbols, const int *scatter_vector, complex_float *mod_symbols) {
    static const complex_float QPSK_table[4] = {
        {-1 / sqrtf(2),  1 / sqrtf(2)}, // 00
        {-1 / sqrtf(2), -1 / sqrtf(2)}, // 01
        {1 / sqrtf(2),   1 / sqrtf(2)}, // 10
        {1 / sqrtf(2),  -1 / sqrtf(2)}  // 11
    };

    for (int i = 0; i < num_symbols; ++i) {
        int mapped_index = scatter_vector[symbols[i]];
        mod_symbols[i] = QPSK_table[mapped_index];
    }
}

int qpsk_modulate(const uint8_t *bits, int bit_len, const int *scatter_vector, complex_float *out_symbols) {
    if (bit_len % 2 != 0) return -1;
    int num_symbols = bit_len / 2;
    uint8_t *symbols = (uint8_t *)malloc(num_symbols);
    if (!symbols) return -2;

    bi2de_pairs(bits, bit_len, symbols);
    qammod_qpsk(symbols, num_symbols, scatter_vector, out_symbols);
    free(symbols);
    return 0;
}
