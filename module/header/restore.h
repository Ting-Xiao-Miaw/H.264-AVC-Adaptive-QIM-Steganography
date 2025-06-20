#ifndef RESTORE_H
#define RESTORE_H

#include <stdio.h>
#include "global.h"
#include "mapping.h"
#include "extract.h"

#define METHOD_M 0
#define METHOD_L 1
#define METHOD_P 2

extern FILE* receiver_message_file;
extern int embedding_method;

extern int H[2][3];

void init_receiver_writer(const char* filename);
void close_receiver_writer();

void restore_category_1(short** coef_matrix);
void restore_category_2(short** coef_matrix);
void restore_category_3(short** coef_matrix);
void restore_category_4(short** coef_matrix);
void restore_category_5(short** coef_matrix);

void check_restore(VideoParameters* p_Vid, int frame_id);

#endif
