#include <stdio.h>
#include "mapping.h"
#include "extract.h"

FILE* mapping_file = NULL;


void init_mapping_writer(const char* filename)
{
    mapping_file = fopen(filename, "w");
    if (!mapping_file)
    {
        error("Cannot open mapping file for writing", 500);
    }
    fprintf(mapping_file, "# frame_id mb_addr b8_idx b4_idx category\n");
}

void close_mapping_writer()
{
    if (mapping_file)
        fclose(mapping_file);
}


int classify_conditions(VideoParameters* p_Vid, int mb_idx, int block_idx)
{
    int mb_x = mb_idx % p_Vid->PicWidthInMbs;
    int mb_y = mb_idx / p_Vid->PicWidthInMbs;
    int block_x = block_idx % 4;
    int block_y = block_idx / 4;

    int Mi_r = -1, Mi_ul = -1, Mi_u = -1, Mi_ur = -1;
    int r_valid = 0, ul_valid = 0, u_valid = 0, ur_valid = 0;


    // Right neighbor
    int right_mb = mb_idx;
    int right_block = block_idx + 1;
    if (block_x == 3) { // Right-most block
        if (mb_x < p_Vid->PicWidthInMbs - 1) { // Not right-most macroblock
            right_mb = mb_idx + 1;  // Move to right macroblock
            right_block = block_idx - 3;    // First block in the same row of the right macroblock
            r_valid = 1;
        }
    }
    else
        r_valid = 1;

    if (r_valid) {
        Mi_r = get_pred_mode(p_Vid, right_mb, right_block);
    }


    // Under-left neighbor
    int ul_mb, ul_block;
    if (block_x == 0) { // Left-most block in macroblock
        if (block_y == 3) { // Bottom-left corner block
            if (mb_x > 0 && mb_y < p_Vid->PicHeightInMbs - 1) {
                ul_mb = mb_idx + p_Vid->PicWidthInMbs - 1;
                ul_block = 3;
                ul_valid = 1;
            }
        }
        else {
            if (mb_x > 0) {
                ul_mb = mb_idx - 1;
                ul_block = (block_y + 1) * 4 + 3;
                ul_valid = 1;
            }
        }
    }
    else if (block_y == 3) { 
        ul_mb = mb_idx + p_Vid->PicWidthInMbs;
        ul_block = block_x - 1;
        ul_valid = 1;
    }
    else {
        ul_mb = mb_idx;
        ul_block = (block_y + 1) * 4 + (block_x - 1);
        ul_valid = 1;
    }

    if (ul_valid) {
        Mi_ul = get_pred_mode(p_Vid, ul_mb, ul_block);
    }


    // Under neighbor
    int under_mb = mb_idx;
    int under_block = (block_y + 1) * 4 + block_x;
    if (block_y == 3 && mb_y < p_Vid->PicHeightInMbs - 1) { // Bottom-most block but not bottom-most macroblock
        under_mb = mb_idx + p_Vid->PicWidthInMbs;
        under_block = block_x;
        u_valid = 1;
    }
    else if (block_y < 3) { // Not bottom-most block
        u_valid = 1;
    }

    if (u_valid) {
        Mi_u = get_pred_mode(p_Vid, under_mb, under_block);
    }


    // Under-right neighbor
    int ur_mb, ur_block;
    if (block_x == 3) { // Right-most block
        if (mb_x < p_Vid->PicWidthInMbs - 1) {  // Not right-most macroblock
            if (block_y == 3 && mb_y < p_Vid->PicHeightInMbs - 1) { // Bottom-right corner block but not buttom-most macroblock
                ur_mb = mb_idx + p_Vid->PicWidthInMbs + 1;
                ur_block = 0;
                ur_valid = 1;
            }
            else if (block_y < 3) {
                ur_mb = mb_idx + 1;
                ur_block = (block_y + 1) * 4;
                ur_valid = 1;
            }
        }
    }
    else {
        if (block_y == 3 && mb_y < p_Vid->PicHeightInMbs - 1) {
            ur_mb = mb_idx + p_Vid->PicWidthInMbs;
            ur_block = block_x + 1;
            ur_valid = 1;
        }
        else if (block_y < 3) {
            ur_mb = mb_idx;
            ur_block = (block_y + 1) * 4 + (block_x + 1);
            ur_valid = 1;
        }
    }

    if (ur_valid) {
        Mi_ur = get_pred_mode(p_Vid, ur_mb, ur_block);
    }


    // Define conditions for each category
    int Case1 = r_valid &&
        ((Mi_r >= 0 && Mi_r < 100 &&
            (Mi_r == 1 || Mi_r == 2 || Mi_r == 4 || Mi_r == 5 || Mi_r == 6 || Mi_r == 8)) ||
            (Mi_r >= 100 && Mi_r - 100 != 0));
    int Case2 = (u_valid || ul_valid) &&
        (((Mi_u >= 0 && Mi_u < 100 &&
            (Mi_u == 0 || Mi_u == 2 || Mi_u == 3 || Mi_u == 4 || Mi_u == 5 || Mi_u == 6 || Mi_u == 7)) ||
            (Mi_u >= 100 && Mi_u - 100 != 1)) ||
            (Mi_ul >= 0 && Mi_ul < 100 && (Mi_ul == 3 || Mi_ul == 7)));
    int Case3 = ur_valid &&
        (Mi_ur >= 0 && Mi_ur < 100 &&
            (Mi_ur == 4 || Mi_ur == 5 || Mi_ur == 6));

    if (Case1 && !Case2) return 1;
    else if (!Case1 && Case2) return 2;
    else if (!Case1 && !Case2 && !Case3) return 3;
    else if (!Case1 && !Case2 && Case3) return 4;
    else if (Case1 && Case2) return 5;
    else return 3;
}


void write_mapping_info(VideoParameters* p_Vid, int frame_id)
{
    for (int mb = 0; mb < p_Vid->PicSizeInMbs; mb++) {
        Macroblock* currMB = &p_Vid->mb_data[mb];
        int mb_mode = currMB->mb_type;

        if (mb_mode == I4MB) {
            for (int b = 0; b < 16; b++) {
                int b8 = b / 4;
                int b4 = b % 4;

                if (qdct_nonzero_flag[mb][b8][b4]) {
                    int block_idx = convert_to_block_idx(b8, b4);
                    int category = classify_conditions(p_Vid, mb, block_idx);
                    fprintf(mapping_file, "%d %d %d %d %d\n", frame_id, mb, b8, b4, category);
                }
            }
        }
    }
}