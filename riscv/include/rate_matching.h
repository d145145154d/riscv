#ifndef RATE_MATCHING_H
#define RATE_MATCHING_H

void rate_matching(
    const int* LDPC_out_11, int len11,
    const int* LDPC_out_12, int len12,
    const int* LDPC_out_21, int len21,
    const int* LDPC_out_22, int len22,
    int rep_factor1, int rep_factor2,
    int** encoder_seq_high, int* high_length,
    int** encoder_seq_low, int* low_length
);

#endif
