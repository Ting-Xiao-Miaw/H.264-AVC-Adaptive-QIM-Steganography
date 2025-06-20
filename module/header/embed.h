#ifndef EMBED_H
#define EMBED_H

#define METHOD_M 0
#define METHOD_L 1
#define METHOD_P 2

typedef struct {
    int frame_id;
    int mb_addr;
    int mb_mode;
    int b8_idx;
    int b4_idx;
    int pred_mode;
} ModeEntry;

typedef struct {
    int frame_id;
    int mb_addr;
    int b8_idx;
    int b4_idx;
    int category;
} MappingEntry;

extern int frame_id;
extern int embedding_method;

extern const double P[4][4];
extern int H[2][3];

void init_mode_reader(const char* filename);
void init_mapping_reader(const char* filename);
void init_sender_writer(const char* filename);

int read_next_mode(ModeEntry* entry_out);
int read_next_mapping(MappingEntry* entry_out);

ModeEntry* get_current_mode();
int next_mode_available();
void update_mode();

MappingEntry* get_current_mapping();
int next_mapping_available();
void update_mapping();

void close_mode_stream();
void close_mapping_stream();
void close_sender_writer();

void embed_category_1(int coef_matrix[4][4]);
void embed_category_2(int coef_matrix[4][4]);
void embed_category_3(int coef_matrix[4][4]);
void embed_category_4(int coef_matrix[4][4]);
void embed_category_5(int coef_matrix[4][4]);

#endif
