#ifndef _EXTRACT_H_
#define _EXTRACT_H_

#include "global.h"

extern FILE* mode_file;
extern int*** qdct_nonzero_flag;
extern short***** qdct_coeff_store;

void init_mode_writer(const char* filename);
void close_mode_writer();

void alloc_qdct_nonzero_flag(VideoParameters* p_Vid);
void free_qdct_nonzero_flag(VideoParameters* p_Vid);

void alloc_qdct_coeff_store(VideoParameters* p_Vid);
void free_qdct_coeff_store(VideoParameters* p_Vid);

int convert_to_block_idx(int b8, int b4);
int get_pred_mode(VideoParameters* p_Vid, int mb_idx, int block_idx);

void write_mode_info(VideoParameters* p_Vid, int frame_id);
void extract_qdct_info_for_I4MB(Macroblock* currMB, int b8, int b4, short qdct_block[4][4]);

#endif
