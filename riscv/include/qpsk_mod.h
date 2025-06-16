#ifndef QPSK_MOD_H
#define QPSK_MOD_H

#include <stdint.h>


typedef struct {
    float real;
    float imag;
} complex_float;

void bi2de_pairs(const uint8_t *bits, int num_bits, uint8_t *out_symbols);

void qammod_qpsk(const uint8_t *symbols, int num_symbols, const int *scatter_vector, complex_float *mod_symbols);

// 新增统一接口
int qpsk_modulate(const uint8_t *bits, int bit_len, const int *scatter_vector, complex_float *out_symbols);

#endif
