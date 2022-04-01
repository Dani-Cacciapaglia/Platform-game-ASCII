#ifndef MY_CONST_H
#define MY_CONST_H
#define width 70
#define height 17

struct matrix_col {
    char col[height];
    matrix_col* next;
};

typedef matrix_col *lista;
#endif
