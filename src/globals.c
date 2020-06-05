#include "defs.h"

struct atom symtab[NSYM];
struct atom *binding[NSYM];
struct atom *arglist[NSYM];

int block_count;
int free_count;
int gc_count;
struct atom *mem[MAXBLOCKS];
struct atom *free_list;

int tos; // top of stack
int tof; // top of frame

struct atom *stack[STACKSIZE];
struct atom *frame[FRAMESIZE];

struct atom *p0;
struct atom *p1;
struct atom *p2;
struct atom *p3;
struct atom *p4;
struct atom *p5;
struct atom *p6;
struct atom *p7;
struct atom *p8;
struct atom *p9;

struct atom *zero;
struct atom *one;
struct atom *minusone;
struct atom *imaginaryunit;

int expanding;

int stop_flag;
int draw_flag;
int clear_flag;

jmp_buf stop_return;
jmp_buf draw_stop_return;

int bignum_count;
int string_count;
int tensor_count;
int max_stack;
int max_frame;

char tbuf[1000];

char *outbuf;
int outbuf_index;
int outbuf_length;
