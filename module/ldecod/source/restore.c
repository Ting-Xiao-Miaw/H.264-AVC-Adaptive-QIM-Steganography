#include <stdio.h>
#include "restore.h"

FILE* receiver_message_file = NULL;
int embedding_method = -1;

int H[2][3] = { {1, 1, 0}, {0, 1, 1} };


void init_receiver_writer(const char* filename)
{
    receiver_message_file = fopen(filename, "w");
    if (!receiver_message_file)
    {
        error("Cannot open mapping file for writing", 500);
    }
}

void close_receiver_writer()
{
    if (receiver_message_file)
        fclose(receiver_message_file);
}


void restore_category_1(short** coef_matrix) {
    int pairs[4][4];
    int pair_count = 0;

    if (embedding_method == METHOD_L) {
        int temp[4][4] = { {0,0,0,2}, {1,0,1,2}, {2,0,2,2}, {3,0,3,2} };
        memcpy(pairs, temp, sizeof(temp));
        pair_count = 4;
    }
    else if (embedding_method == METHOD_M || embedding_method == METHOD_P) {
        int temp[3][4] = { {1,0,1,2}, {2,0,2,2}, {3,0,3,2} };
        memcpy(pairs, temp, sizeof(temp));
        pair_count = 3;
    }

    if (embedding_method == METHOD_M || embedding_method == METHOD_L) {
        for (int i = 0; i < pair_count; i++) {
            int m = pairs[i][0], n = pairs[i][1];
            int p = pairs[i][2], q = pairs[i][3];
            int Ymn = coef_matrix[m][n];
            int Ypq = coef_matrix[p][q];

            if (Ymn == 0 && Ypq == 0)
                continue;

            if (Ymn % 2 == 0) {
                fprintf(receiver_message_file, "0");
            }
            else {
                fprintf(receiver_message_file, "1");
            }
        }
    }
    else if (embedding_method == METHOD_P) {
        int z[3];

        for (int i = 0; i < pair_count; i++) {
            int m = pairs[i][0], n = pairs[i][1];
            int coeff = coef_matrix[m][n];
            z[i] = (coeff % 4 + 4) % 4;
        }

        int s0 = (H[0][0] * z[0] + H[0][1] * z[1] + H[0][2] * z[2]) % 4;
        int s1 = (H[1][0] * z[0] + H[1][1] * z[1] + H[1][2] * z[2]) % 4;

        int msg = (s0 << 2) | s1;

        for (int bit_pos = 3; bit_pos >= 0; bit_pos--) {
            int bit = (msg >> bit_pos) & 1;
            fprintf(receiver_message_file, "%d", bit);
        }
    }
}


void restore_category_2(short** coef_matrix) {
    int pairs[4][4];
    int pair_count = 0;

    if (embedding_method == METHOD_L) {
        int temp[4][4] = { {0,0,2,0}, {0,1,2,1}, {0,2,2,2}, {0,3,2,3} };
        memcpy(pairs, temp, sizeof(temp));
        pair_count = 4;
    }
    else if (embedding_method == METHOD_M || embedding_method == METHOD_P) {
        int temp[3][4] = { {0,1,2,1}, {0,2,2,2}, {0,3,2,3} };
        memcpy(pairs, temp, sizeof(temp));
        pair_count = 3;
    }

    if (embedding_method == METHOD_M || embedding_method == METHOD_L) {
        for (int i = 0; i < pair_count; i++) {
            int m = pairs[i][0], n = pairs[i][1];
            int p = pairs[i][2], q = pairs[i][3];
            int Ymn = coef_matrix[m][n];
            int Ypq = coef_matrix[p][q];

            if (Ymn == 0 && Ypq == 0)
                continue;

            if (Ymn % 2 == 0) {
                fprintf(receiver_message_file, "0");
            }
            else {
                // printf("Extract: 1\n");
                fprintf(receiver_message_file, "1");
            }
        }
    }
    else if (embedding_method == METHOD_P) {
        int z[3];

        for (int i = 0; i < pair_count; i++) {
            int m = pairs[i][0], n = pairs[i][1];
            int coeff = coef_matrix[m][n];
            z[i] = (coeff % 4 + 4) % 4;
        }

        int s0 = (H[0][0] * z[0] + H[0][1] * z[1] + H[0][2] * z[2]) % 4;
        int s1 = (H[1][0] * z[0] + H[1][1] * z[1] + H[1][2] * z[2]) % 4;

        int msg = (s0 << 2) | s1;

        for (int bit_pos = 3; bit_pos >= 0; bit_pos--) {
            int bit = (msg >> bit_pos) & 1;
            fprintf(receiver_message_file, "%d", bit);
        }
    }
}


void restore_category_3(short** coef_matrix) {
    if (embedding_method == METHOD_M || embedding_method == METHOD_L) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (coef_matrix[i][j] == 0)
                    continue;

                if (coef_matrix[i][j] % 2 == 0) {
                    fprintf(receiver_message_file, "0");
                }
                else {
                    fprintf(receiver_message_file, "1");
                }
            }
        }
    }
    else if (embedding_method == METHOD_P) {
        int pairs[5][3][2] = {
            { {0,1}, {0,2}, {0,3} },
            { {1,0}, {2,0}, {3,0} },
            { {1,1}, {2,1}, {3,1} },
            { {1,2}, {2,2}, {3,2} },
            { {1,3}, {2,3}, {3,3} }
        };

        int pair_group = 5;
        int pair_count = 3;

        for (int g = 0; g < pair_group; g++) {
            int x[3];
            int z[3];

            for (int i = 0; i < pair_count; i++) {
                int y = pairs[g][i][0];
                int x_pos = pairs[g][i][1];
                x[i] = coef_matrix[y][x_pos];
                z[i] = (x[i] % 4 + 4) % 4;
            }

            int s0 = (H[0][0] * z[0] + H[0][1] * z[1] + H[0][2] * z[2]) % 4;
            int s1 = (H[1][0] * z[0] + H[1][1] * z[1] + H[1][2] * z[2]) % 4;

            int msg = (s0 << 2) | s1;

            for (int bit_pos = 3; bit_pos >= 0; bit_pos--) {
                int bit = (msg >> bit_pos) & 1;
                fprintf(receiver_message_file, "%d", bit);
            }
        }
    }
}


void restore_category_4(short** coef_matrix) {
    restore_category_2(coef_matrix);
}


void restore_category_5(short** coef_matrix) {
    if (embedding_method == METHOD_M || embedding_method == METHOD_P)
        return;

    int pairs[4][2] = { {0,0}, {0,2}, {2,0}, {2,2} };
    if (coef_matrix[0][0] == 0 && coef_matrix[0][2] == 0 && coef_matrix[2][0] == 0 && coef_matrix[2][2] == 0)
        return;

    if (coef_matrix[0][0] % 2 == 0) {
        fprintf(receiver_message_file, "0");
    }
    else {
        fprintf(receiver_message_file, "1");
    }
}


void check_restore(VideoParameters* p_Vid, int frame_id)
{
    for (int mb = 0; mb < p_Vid->PicSizeInMbs; mb++) {
        if (p_Vid->mb_data[mb].mb_type != I4MB)
            continue;
        for (int b = 0; b < 16; b++) {
            int b8 = b / 4;
            int b4 = b % 4;

            if (qdct_nonzero_flag[mb][b8][b4]) {
                int block_idx = convert_to_block_idx(b8, b4);
                int category = classify_conditions(p_Vid, mb, block_idx);

                if (category == 1)
                    restore_category_1(qdct_coeff_store[mb][b8][b4]);
                else if (category == 2 || category == 4)
                    restore_category_2(qdct_coeff_store[mb][b8][b4]);
                else if (category == 3)
                    restore_category_3(qdct_coeff_store[mb][b8][b4]);
                else if (category == 5)
                    restore_category_5(qdct_coeff_store[mb][b8][b4]);
            }
        }
    }
}
