#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "embed.h"

static FILE* mapping_file = NULL;
static FILE* mode_file = NULL;
static FILE* sender_message_file = NULL;
static ModeEntry current_mode;
static MappingEntry current_mapping;
static int next_mapping = 0;
static int next_mode = 0;

int frame_id = 0;
int embedding_method = -1;

int H[2][3] = { {1, 1, 0}, {0, 1, 1} };


void init_mode_reader(const char* filename) {
    mode_file = fopen(filename, "r");
    if (!mode_file) {
        fprintf(stderr, "Error: Cannot open mode file %s\n", filename);
        return 0;
    }
    next_mode = read_next_mode(&current_mode);
}


void init_mapping_reader(const char* filename) {
    mapping_file = fopen(filename, "r");
    if (!mapping_file) {
        fprintf(stderr, "Error: Cannot open mapping file %s\n", filename);
        return 0;
    }
    next_mapping = read_next_mapping(&current_mapping);
}


void init_sender_writer(const char* filename) {
    sender_message_file = fopen(filename, "w");
    if (!sender_message_file)
    {
        error("Cannot open sender message file for writing", 500);
    }
}


int read_next_mode(ModeEntry* entry_out) {
    char line[256];
    while (fgets(line, sizeof(line), mode_file)) {
        if (line[0] == '#')
            continue;

        if (sscanf(line, "%d %d %d %d %d %d",
            &entry_out->frame_id,
            &entry_out->mb_addr,
            &entry_out->mb_mode,
            &entry_out->b8_idx,
            &entry_out->b4_idx,
            &entry_out->pred_mode) == 6)
            return 1;
    }
    return 0;
}

int read_next_mapping(MappingEntry* entry_out) {
    char line[256];
    while (fgets(line, sizeof(line), mapping_file)) {
        if (line[0] == '#')
            continue;

        if (sscanf(line, "%d %d %d %d %d",
            &entry_out->frame_id,
            &entry_out->mb_addr,
            &entry_out->b8_idx,
            &entry_out->b4_idx,
            &entry_out->category) == 5)
            return 1;
    }
    return 0;
}


ModeEntry* get_current_mode() {
    return &current_mode;
}

int next_mode_available() {
    return next_mode;
}

void update_mode() {
    next_mode = read_next_mode(&current_mode);
}


MappingEntry* get_current_mapping() {
    return &current_mapping;
}

int next_mapping_available() {
    return next_mapping;
}

void update_mapping() {
    next_mapping = read_next_mapping(&current_mapping);
}


void close_mapping_stream() {
    if (mapping_file) {
        fclose(mapping_file);
        mapping_file = NULL;
    }
}

void close_mode_stream() {
    if (mode_file) {
        fclose(mode_file);
        mode_file = NULL;
    }
}

void close_sender_writer()
{
    if (sender_message_file) {
        fclose(sender_message_file);
        sender_message_file = NULL;
    }
}


void embed_category_1(int coef_matrix[4][4]) {
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

            int bit = rand() % 2;
            fprintf(sender_message_file, "%d", bit);

            int modify = (abs(Ymn) % 2 == bit) ? 0 : 1;

            if (modify) {
                if (Ymn >= 0) {
                    coef_matrix[m][n] += 1;
                    coef_matrix[p][q] -= 1;
                }
                else {
                    coef_matrix[m][n] -= 1;
                    coef_matrix[p][q] += 1;
                }
            }
        }
    }
    else if (embedding_method == METHOD_P) {
        int x[3];

        for (int i = 0; i < 3; i++) {
            int y1 = pairs[i][0];
            int x1 = pairs[i][1];
            x[i] = coef_matrix[y1][x1];
        }

        int msg = rand() % 16;
        for (int bit_pos = 3; bit_pos >= 0; bit_pos--) {
            int bit = (msg >> bit_pos) & 1;
            fprintf(sender_message_file, "%d", bit);
        }

        int z_orig[3];
        for (int i = 0; i < 3; i++) {
            z_orig[i] = (x[i] % 4 + 4) % 4;
        }

        int best_d[3] = { 0 };
        int min_cost = 1000;

        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 4; b++) {
                for (int c = 0; c < 4; c++) {
                    int z[3] = { a, b, c };
                    int s0 = (H[0][0] * a + H[0][1] * b + H[0][2] * c) % 4;
                    int s1 = (H[1][0] * a + H[1][1] * b + H[1][2] * c) % 4;
                    int target_s0 = (msg >> 2) & 0x3;
                    int target_s1 = msg & 0x3;
                    if (s0 == target_s0 && s1 == target_s1) {
                        int cost = 0;
                        int temp_d[3];
                        for (int i = 0; i < 3; i++) {
                            int xi = x[i];
                            int zi = z[i];
                            int forward = (zi - z_orig[i] + 4) % 4;
                            int backward = (z_orig[i] - zi + 4) % 4;
                            int d = (forward <= backward) ? forward : -backward;
                            cost += abs(d);
                            temp_d[i] = d;
                        }

                        if (cost < min_cost) {
                            min_cost = cost;
                            for (int i = 0; i < 3; i++) {
                                best_d[i] = temp_d[i];
                            }
                        }

                    }
                }
            }
        }
     
        for (int i = 0; i < pair_count; i++) {
            int m = pairs[i][0], n = pairs[i][1];
            int p = pairs[i][2], q = pairs[i][3];

            coef_matrix[m][n] += best_d[i];
            coef_matrix[p][q] -= best_d[i];
        }

        int is_all_zero = 1;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (coef_matrix[i][j] != 0) {
                    is_all_zero = 0;
                    break;
                }
            }
            if (!is_all_zero)
                break;
        }

        if (is_all_zero) {
            coef_matrix[0][0] += 1;
            coef_matrix[0][2] -= 1;
        }
    }
}


void embed_category_2(int coef_matrix[4][4]) {
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

            int bit = rand() % 2;
            fprintf(sender_message_file, "%d", bit);

            int modify = (abs(Ymn) % 2 == bit) ? 0 : 1;

            if (modify) {
                if (Ymn >= 0) {
                    coef_matrix[m][n] += 1;
                    coef_matrix[p][q] -= 1;
                }
                else {
                    coef_matrix[m][n] -= 1;
                    coef_matrix[p][q] += 1;
                }
            }
        }
    }
    else if (embedding_method == METHOD_P) {
        int x[3];

        for (int i = 0; i < 3; i++) {
            int y1 = pairs[i][0];
            int x1 = pairs[i][1];
            x[i] = coef_matrix[y1][x1];
        }

        int msg = rand() % 16;
        for (int bit_pos = 3; bit_pos >= 0; bit_pos--) {
            int bit = (msg >> bit_pos) & 1;
            fprintf(sender_message_file, "%d", bit);
        }

        int z_orig[3];
        for (int i = 0; i < 3; i++) {
            z_orig[i] = (x[i] % 4 + 4) % 4;
        }

        int best_d[3] = { 0 };
        int min_cost = 1000;

        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 4; b++) {
                for (int c = 0; c < 4; c++) {
                    int z[3] = { a, b, c };
                    int s0 = (H[0][0] * a + H[0][1] * b + H[0][2] * c) % 4;
                    int s1 = (H[1][0] * a + H[1][1] * b + H[1][2] * c) % 4;
                    int target_s0 = (msg >> 2) & 0x3;
                    int target_s1 = msg & 0x3;
                    if (s0 == target_s0 && s1 == target_s1) {
                        int cost = 0;
                        int temp_d[3];
                        for (int i = 0; i < 3; i++) {
                            int xi = x[i];
                            int zi = z[i];
                            int forward = (zi - z_orig[i] + 4) % 4;
                            int backward = (z_orig[i] - zi + 4) % 4;
                            int d = (forward <= backward) ? forward : -backward;
                            cost += abs(d);
                            temp_d[i] = d;
                        }

                        if (cost < min_cost) {
                            min_cost = cost;
                            for (int i = 0; i < 3; i++) {
                                best_d[i] = temp_d[i];
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < pair_count; i++) {
            int m = pairs[i][0], n = pairs[i][1];
            int p = pairs[i][2], q = pairs[i][3];

            coef_matrix[m][n] += best_d[i];
            coef_matrix[p][q] -= best_d[i];
        }

        int is_all_zero = 1;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (coef_matrix[i][j] != 0) {
                    is_all_zero = 0;
                    break;
                }
            }
            if (!is_all_zero)
                break;
        }

        if (is_all_zero) {
            coef_matrix[0][0] += 1;
            coef_matrix[2][0] -= 1;
        }
    }
}


void embed_category_3(int coef_matrix[4][4]) {

    if (embedding_method == METHOD_M || embedding_method == METHOD_L) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (coef_matrix[i][j] == 0)
                    continue;

                int bit = rand() % 2;
                fprintf(sender_message_file, "%d", bit);

                int modify = (abs(coef_matrix[i][j]) % 2 == bit) ? 0 : 1;

                if (modify) {
                    if (coef_matrix[i][j] >= 0) {
                        coef_matrix[i][j] += 1;
                    }
                    else {
                        coef_matrix[i][j] -= 1;
                    }
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
            for (int i = 0; i < pair_count; i++) {
                int y1 = pairs[g][i][0];
                int x1 = pairs[g][i][1];
                x[i] = coef_matrix[y1][x1];
            }

            int msg = rand() % 16;
            for (int bit_pos = 3; bit_pos >= 0; bit_pos--) {
                int bit = (msg >> bit_pos) & 1;
                fprintf(sender_message_file, "%d", bit);
            }


            int z_orig[3];
            for (int i = 0; i < 3; i++) {
                z_orig[i] = (x[i] % 4 + 4) % 4;
            }

            int best_d[3] = { 0 };
            int min_cost = 1000;


            for (int a = 0; a < 4; a++) {
                for (int b = 0; b < 4; b++) {
                    for (int c = 0; c < 4; c++) {
                        int z[3] = { a, b, c };
                        int s0 = (H[0][0] * a + H[0][1] * b + H[0][2] * c) % 4;
                        int s1 = (H[1][0] * a + H[1][1] * b + H[1][2] * c) % 4;
                        int target_s0 = (msg >> 2) & 0x3;
                        int target_s1 = msg & 0x3;
                        if (s0 == target_s0 && s1 == target_s1) {
                            int cost = 0;
                            int temp_d[3];
                            for (int i = 0; i < 3; i++) {
                                int xi = x[i];
                                int zi = z[i];
                                int forward = (zi - z_orig[i] + 4) % 4;
                                int backward = (z_orig[i] - zi + 4) % 4;
                                int d = (forward <= backward) ? forward : -backward;
                                cost += abs(d);
                                temp_d[i] = d;
                            }

                            if (cost < min_cost) {
                                min_cost = cost;
                                for (int i = 0; i < 3; i++) {
                                    best_d[i] = temp_d[i];
                                }
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < 3; i++) {
                int m = pairs[g][i][0], n = pairs[g][i][1];

                coef_matrix[m][n] += best_d[i];
            }
        }

        int is_all_zero = 1;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (coef_matrix[i][j] != 0) {
                    is_all_zero = 0;
                    break;
                }
            }
            if (!is_all_zero)
                break;
        }

        if (is_all_zero) {
            coef_matrix[0][0] += 1;
        }
    }
}


void embed_category_4(int coef_matrix[4][4]) {
    embed_category_2(coef_matrix[4][4]);
}


void embed_category_5(int coef_matrix[4][4]) {

    if (embedding_method == METHOD_M || embedding_method == METHOD_P)
        return;

    int pairs[4][2] = { {0,0}, {0,2}, {2,0}, {2,2} };
    if (coef_matrix[0][0] == 0 && coef_matrix[0][2] == 0 && coef_matrix[2][0] == 0 && coef_matrix[2][2] == 0)
        return;

    int bit = rand() % 2;
    fprintf(sender_message_file, "%d", bit);

    int modify = (abs(coef_matrix[0][0]) % 2 == bit) ? 0 : 1;

    if (modify) {
        if (coef_matrix[0][0] >= 0) {
            coef_matrix[0][0] += 1;
            coef_matrix[0][2] -= 1;
            coef_matrix[2][0] -= 1;
            coef_matrix[2][2] += 1;
        }
        else {
            coef_matrix[0][0] -= 1;
            coef_matrix[0][2] += 1;
            coef_matrix[2][0] += 1;
            coef_matrix[2][2] -= 1;
        }
    }
}