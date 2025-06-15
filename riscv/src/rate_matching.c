#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/rate_matching.h"

void rate_matching1(
    const int* LDPC_out, int ldpc_length, int rep_factor,
    int** code_bits, int* output_length
) {
    // 重复编码
    int rep_length = ldpc_length * rep_factor;
    int* rep_bits = (int*)malloc(rep_length * sizeof(int));

    for (int i = 0; i < ldpc_length; i++) {
        for (int j = 0; j < rep_factor; j++) {
            rep_bits[i * rep_factor + j] = LDPC_out[i];
        }
    }

    // 确定交织器大小
    int rows, cols;
    switch (rep_factor) {
        case 3:
            rows = 144; cols = 91;  // 强保护
            break;
        case 2:
            rows = 104; cols = 84;  // 弱保护
            break;
        default:
            printf("Unsupported repetition factor\n");
            free(rep_bits);
            *code_bits = NULL;
            *output_length = 0;
            return;
    }

    // 检查输入长度是否足够填充交织器
    if (rep_length < rows * cols) {
        printf("Input length is insufficient for interleaver\n");
        free(rep_bits);
        *code_bits = NULL;
        *output_length = 0;
        return;
    }

    // 交织
    // 创建交织矩阵 (行填充)
    int** matrix = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        // 构建一个rows行×cols列的二维矩阵
        matrix[i] = (int*)malloc(cols * sizeof(int));
    }
    // 按行填充
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = rep_bits[i * cols + j];
        }
    }
    // 按列读出
    *output_length = rows * cols;
    *code_bits = (int*)malloc((*output_length) * sizeof(int));
    int index = 0;
    for (int j = 0; j < cols; j++) {  // 列优先遍历
        for (int i = 0; i < rows; i++) { // 每列从上到下
            (*code_bits)[index++] = matrix[i][j];
        }
    }

    // 释放内存
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(rep_bits);
}


int* concatenate_arrays(const int* arr1, int len1, const int* arr2, int len2) {
    // 合并两个数组
    int* result = (int*)malloc((len1 + len2) * sizeof(int));
    memcpy(result, arr1, len1 * sizeof(int));
    memcpy(result + len1, arr2, len2 * sizeof(int));
    return result;
}


int* generate_random_bits(int length) {
    // 生成随机二进制数组
    int* bits = (int*)malloc(length * sizeof(int));
    for (int i = 0; i < length; i++) {
        bits[i] = rand() % 2; // 生成0或1的随机数
    }
    return bits;
}

void rate_matching(
    const int* LDPC_out_11, int len11,
    const int* LDPC_out_12, int len12,
    const int* LDPC_out_21, int len21,
    const int* LDPC_out_22, int len22,
    int rep_factor1, int rep_factor2,
    int** encoder_seq_high, int* high_length,
    int** encoder_seq_low, int* low_length
) {
    // 初始化随机数生成器
    srand((unsigned int)time(NULL));

    // encoder_seq_high
    int* rm11_bits = NULL;
    int rm11_length = 0;
    rate_matching1(LDPC_out_11, len11, rep_factor1, &rm11_bits, &rm11_length);

    int* rm12_bits = NULL;
    int rm12_length = 0;
    rate_matching1(LDPC_out_12, len12, rep_factor2, &rm12_bits, &rm12_length);

    int* combined_high = concatenate_arrays(rm11_bits, rm11_length, rm12_bits, rm12_length);


    // // 添加随机比特
    // int* random_bits_high = generate_random_bits(3008);
    // *encoder_seq_high = concatenate_arrays(combined_high, rm11_length + rm12_length,
    //                                      random_bits_high, 3008);
    // *high_length = rm11_length + rm12_length + 3008;
    //
    // // encoder_seq_low
    // *encoder_seq_low = concatenate_arrays(LDPC_out_21, len21, LDPC_out_22, len22);
    // int* random_bits_low = generate_random_bits(1504);
    // *encoder_seq_low = concatenate_arrays(*encoder_seq_low, len21 + len22,
    //                                     random_bits_low, 1504);
    // *low_length = len21 + len22 + 1504;

    // 添加固定全0比特
    int* zero_bits_high = (int*)calloc(3008, sizeof(int));  // 全0数组
    *encoder_seq_high = concatenate_arrays(combined_high, rm11_length + rm12_length,
                                         zero_bits_high, 3008);
    *high_length = rm11_length + rm12_length + 3008;

    // encoder_seq_low
    *encoder_seq_low = concatenate_arrays(LDPC_out_21, len21, LDPC_out_22, len22);
    int* zero_bits_low = (int*)calloc(1504, sizeof(int));  // 全0数组
    *encoder_seq_low = concatenate_arrays(*encoder_seq_low, len21 + len22,
                                        zero_bits_low, 1504);
    *low_length = len21 + len22 + 1504;


    // 释放内存
    free(rm11_bits);
    free(rm12_bits);
    free(combined_high);
    free(zero_bits_high);
    free(zero_bits_low);


}

