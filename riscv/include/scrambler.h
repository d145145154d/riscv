#ifndef SCRAMBLER_H
#define SCRAMBLER_H

#include <stdint.h>

#define MAX_LEN 60160

int load_bit_file(const char *filename, uint8_t *buffer);
int save_bit_file(const char *filename, uint8_t *buffer, int len);
int scramble_bits(uint8_t *scramble_seq, uint8_t *source_bit, uint8_t *scramble_out, int len);
void split_scramble_buffer(uint8_t *scramble, uint8_t *high, uint8_t *low, int len);

#endif

