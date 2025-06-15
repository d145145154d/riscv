#include "../include/scrambler.h"
#include <stdio.h>

int load_bit_file(const char *filename, uint8_t *buffer) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    int count = 0, bit;
    while (fscanf(fp, "%d", &bit) == 1 && count < MAX_LEN) {
        if (bit != 0 && bit != 1) return -2;
        buffer[count++] = (uint8_t)bit;
    }
    fclose(fp);
    return count;
}

int save_bit_file(const char *filename, uint8_t *buffer, int len) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;
    for (int i = 0; i < len; i++) fprintf(fp, "%d\n", buffer[i]);
    fclose(fp);
    return 0;
}

int scramble_bits(uint8_t *scramble_seq, uint8_t *source_bit, uint8_t *scramble_out, int len) {
    for (int i = 0; i < len; i++) {
        scramble_out[i] = scramble_seq[i] ^ source_bit[i];
    }
    return 0;
}

void split_scramble_buffer(uint8_t *scramble, uint8_t *high, uint8_t *low, int len) {
    int half = len / 2;
    for (int i = 0; i < half; i++) {
        high[i] = scramble[i];
        low[i] = scramble[half + i];
    }
}
