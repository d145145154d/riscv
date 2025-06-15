#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "src/rate_matching.c"
#include  "src/qpsk_mod.c"
#include "src/scrambler.c"

#define LENGTH 4368

void read_array_from_file(const char* filename, int* array, int length) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open LDPC output file");
        exit(EXIT_FAILURE);
    }
    int val, count = 0;
    while (fscanf(file, "%d", &val) == 1 && count < length) {
        array[count++] = val;
    }
    fclose(file);

    if (count != length) {
        fprintf(stderr, "File %s length mismatch: expect %d, got %d\n",
                filename, length, count);
        exit(EXIT_FAILURE);
    }

}

int save_symbols_to_file(const char *filename, complex_float *symbols, int length) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open output file");
        return -1;
    }
    for (int i = 0; i < length; ++i)
        fprintf(file, "%.6f %.6f\n", symbols[i].real, symbols[i].imag);
    fclose(file);
    return 0;
}


int main() {

    // 0.scrambler
    uint8_t scramble_seq[MAX_LEN], source_bit[MAX_LEN], scramble_buffer[MAX_LEN];
    uint8_t scramble_high[MAX_LEN / 2], scramble_low[MAX_LEN / 2];

    int len1 = load_bit_file("scramble_seq.txt", scramble_seq);
    int len2 = load_bit_file("sourceBit.txt", source_bit);
    if (len1 <= 0 || len2 <= 0 || len1 != len2) return -1;

    scramble_bits(scramble_seq, source_bit, scramble_buffer, len1);
    split_scramble_buffer(scramble_buffer, scramble_high, scramble_low, len1);

    save_bit_file("scramble_buffer_high.txt", scramble_high, len1 / 2);
    save_bit_file("scramble_buffer_low.txt", scramble_low, len1 / 2);


    // 1. 读取 LDPC 输出
    int LDPC_out_11[LENGTH], LDPC_out_12[LENGTH], LDPC_out_21[LENGTH], LDPC_out_22[LENGTH];
    read_array_from_file("LDPC_out_11.txt", LDPC_out_11, LENGTH);
    read_array_from_file("LDPC_out_12.txt", LDPC_out_12, LENGTH);
    read_array_from_file("LDPC_out_21.txt", LDPC_out_21, LENGTH);
    read_array_from_file("LDPC_out_22.txt", LDPC_out_22, LENGTH);

    // 2. 速率匹配
    int rep_factor1 = 2, rep_factor2 = 2;
    int *encoder_seq_high = NULL, *encoder_seq_low = NULL;
    int high_len = 0, low_len = 0;

    rate_matching(
        LDPC_out_11, LENGTH,
        LDPC_out_12, LENGTH,
        LDPC_out_21, LENGTH,
        LDPC_out_22, LENGTH,
        rep_factor1, rep_factor2,
        &encoder_seq_high, &high_len,
        &encoder_seq_low, &low_len
    );

    if (!encoder_seq_high || !encoder_seq_low || high_len == 0 || low_len == 0) {
        fprintf(stderr, "rate_matching failed (NULL output)\n");
        return -1;
    }


    // 3. QPSK 调制
    if (high_len % 2 != 0 || low_len % 2 != 0) {
        fprintf(stderr, "Bit lengths must be even for QPSK.\n");
        return -1;
    }

    uint8_t *bits_high = (uint8_t *)malloc(high_len);
    uint8_t *bits_low = (uint8_t *)malloc(low_len);
    if (!bits_high || !bits_low) {
        perror("Failed to allocate memory for bit arrays");
        return -1;
    }

    // 转换 int -> uint8_t
    for (int i = 0; i < high_len; ++i) bits_high[i] = (uint8_t)encoder_seq_high[i];
    for (int i = 0; i < low_len; ++i)  bits_low[i]  = (uint8_t)encoder_seq_low[i];

    // 调制结果
    complex_float *symbols_high = malloc((high_len / 2) * sizeof(complex_float));
    complex_float *symbols_low = malloc((low_len / 2) * sizeof(complex_float));
    int scatter_vector[4] = {2, 3, 0, 1};

    if (qpsk_modulate(bits_high, high_len, scatter_vector, symbols_high) != 0) {
        fprintf(stderr, "High protection QPSK modulation failed.\n");
        return -1;
    }
    if (qpsk_modulate(bits_low, low_len, scatter_vector, symbols_low) != 0) {
        fprintf(stderr, "Low protection QPSK modulation failed.\n");
        return -1;
    }
    printf("QPSK modulation results have been written to mod_data_high.txt and mod_data_low.txt.\n");

    // 保存
    save_symbols_to_file("mod_data_high.txt", symbols_high, high_len / 2);
    save_symbols_to_file("mod_data_low.txt", symbols_low, low_len / 2);

    // 清理
    free(symbols_high);
    free(symbols_low);
    free(bits_high);
    free(bits_low);
    free(encoder_seq_high);
    free(encoder_seq_low);

    return 0;
}

