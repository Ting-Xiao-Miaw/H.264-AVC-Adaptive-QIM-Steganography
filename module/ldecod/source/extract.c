#include <stdio.h>
#include "extract.h"

FILE* mode_file = NULL;
int*** qdct_nonzero_flag = NULL;
short***** qdct_coeff_store = NULL;


void init_mode_writer(const char* filename)
{
    mode_file = fopen(filename, "w");
    if (!mode_file)
    {
        error("Cannot open mode file for writing", 500);
    }
    fprintf(mode_file, "# frame_id mb_addr mb_mode b8_idx b4_idx pred_mode\n");
}


void close_mode_writer()
{
    if (mode_file)
        fclose(mode_file);
}


void alloc_qdct_nonzero_flag(VideoParameters* p_Vid)
{
    int num_mb = p_Vid->PicSizeInMbs;

    qdct_nonzero_flag = (int***)calloc(num_mb, sizeof(int**));
    for (int mb = 0; mb < num_mb; mb++) {
        qdct_nonzero_flag[mb] = (int**)calloc(4, sizeof(int*)); // 4 b8
        for (int b8 = 0; b8 < 4; b8++) {
            qdct_nonzero_flag[mb][b8] = (int*)calloc(4, sizeof(int)); // 4 b4
        }
    }
}


void free_qdct_nonzero_flag(VideoParameters* p_Vid)
{
    for (int mb = 0; mb < p_Vid->PicSizeInMbs; mb++) {
        for (int b8 = 0; b8 < 4; b8++) {
            free(qdct_nonzero_flag[mb][b8]);
        }
        free(qdct_nonzero_flag[mb]);
    }
    free(qdct_nonzero_flag);
}


void alloc_qdct_coeff_store(VideoParameters* p_Vid)
{
    int num_mb = p_Vid->PicSizeInMbs;

    qdct_coeff_store = (short*****)calloc(num_mb, sizeof(short****));
    for (int mb = 0; mb < num_mb; mb++) {
        qdct_coeff_store[mb] = (short****)calloc(4, sizeof(short***)); // 4 b8
        for (int b8 = 0; b8 < 4; b8++) {
            qdct_coeff_store[mb][b8] = (short***)calloc(4, sizeof(short**)); // 4 b4
            for (int b4 = 0; b4 < 4; b4++) {
                qdct_coeff_store[mb][b8][b4] = (short**)calloc(4, sizeof(short*)); // 4 rows
                for (int jj = 0; jj < 4; jj++) {
                    qdct_coeff_store[mb][b8][b4][jj] = (short*)calloc(4, sizeof(short)); // 4 cols
                }
            }
        }
    }
}


void free_qdct_coeff_store(VideoParameters* p_Vid)
{
    int num_mb = p_Vid->PicSizeInMbs;
    for (int mb = 0; mb < num_mb; mb++) {
        for (int b8 = 0; b8 < 4; b8++) {
            for (int b4 = 0; b4 < 4; b4++) {
                for (int jj = 0; jj < 4; jj++) {
                    free(qdct_coeff_store[mb][b8][b4][jj]);
                }
                free(qdct_coeff_store[mb][b8][b4]);
            }
            free(qdct_coeff_store[mb][b8]);
        }
        free(qdct_coeff_store[mb]);
    }
    free(qdct_coeff_store);
}


int convert_to_block_idx(int b8, int b4) {
    int base_x = (b8 % 2) * 2;
    int base_y = (b8 / 2) * 2;

    int offset_x = (b4 % 2);
    int offset_y = (b4 / 2);

    return (base_y + offset_y) * 4 + (base_x + offset_x);
}


int get_pred_mode(VideoParameters* p_Vid, int mb_idx, int block_idx) {
    if (mb_idx < 0 || mb_idx >= p_Vid->PicSizeInMbs)
        return -1;

    Macroblock* mb = &p_Vid->mb_data[mb_idx];


    if (mb->mb_type == I4MB || mb->mb_type == I8MB) {
        int mb_x = mb_idx % p_Vid->PicWidthInMbs;
        int mb_y = mb_idx / p_Vid->PicWidthInMbs;
        int block_x = block_idx % 4;
        int block_y = block_idx / 4;
        int img_x = mb_x * 4 + block_x;
        int img_y = mb_y * 4 + block_y;

        if (img_x >= p_Vid->PicWidthInMbs * 4 || img_y >= p_Vid->PicHeightInMbs * 4)
            return -1;
        return p_Vid->ipredmode[img_y][img_x];
    }
    else if (mb->mb_type == I16MB) {
        return mb->i16mode + 100;
    }
    return -1;
}


void write_mode_info(VideoParameters* p_Vid, int frame_id)
{
    for (int mb = 0; mb < p_Vid->PicSizeInMbs; mb++) {
        Macroblock* currMB = &p_Vid->mb_data[mb];
        int mb_mode = currMB->mb_type;

        if (mb_mode == I16MB) {
            fprintf(mode_file, "%d %d %d -1 -1 %d\n", frame_id, mb, mb_mode, currMB->i16mode);
        }
        else if (mb_mode == I8MB) {
            for (int b8 = 0; b8 < 4; b8++) {
                int b4 = 0;
                int block_idx = convert_to_block_idx(b8, b4);
                int Mode = get_pred_mode(p_Vid, mb, block_idx);
                fprintf(mode_file, "%d %d %d %d -1 %d\n", frame_id, mb, mb_mode, b8, Mode);
            }
        }
        else if (mb_mode == I4MB) {

            for (int b = 0; b < 16; b++) {
                int b8 = b / 4;
                int b4 = b % 4;

                int block_idx = convert_to_block_idx(b8, b4);
                int Mode = get_pred_mode(p_Vid, mb, block_idx);
                fprintf(mode_file, "%d %d %d %d %d %d\n", frame_id, mb, mb_mode, b8, b4, Mode);
            }
        }
    }
}


void extract_qdct_info_for_I4MB(Macroblock* currMB, int b8, int b4, short qdct_block[4][4])
{
    int nonzero = 0;
    for (int jj = 0; jj < 4; ++jj) {
        for (int ii = 0; ii < 4; ++ii) {
            if (qdct_block[jj][ii] != 0) {
                nonzero = 1;
                break;
            }
        }
        if (nonzero)
            break;
    }
    if (nonzero) {
        qdct_nonzero_flag[currMB->mbAddrX][b8][b4] = 1;
        for (int jj = 0; jj < 4; ++jj) {
            for (int ii = 0; ii < 4; ++ii) {
                qdct_coeff_store[currMB->mbAddrX][b8][b4][jj][ii] = qdct_block[jj][ii];
            }
        }
    }
}