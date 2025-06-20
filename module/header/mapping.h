#ifndef _MAPPING_H_
#define _MAPPING_H_

#include "global.h"

void init_mapping_writer(const char* filename);
void close_mapping_writer(void);

int classify_conditions(VideoParameters* p_Vid, int mb_idx, int block_idx);
void write_mapping_info(VideoParameters* p_Vid, int frame_id);

#endif
