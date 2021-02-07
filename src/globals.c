#include "defs.h"

struct atom *mem[MAXBLOCKS];
struct atom *free_list;

int tos; // top of stack
int tof; // top of frame

struct atom *stack[STACKSIZE];
struct atom *frame[FRAMESIZE];

struct atom *symtab[27 * NSYM];
struct atom *binding[27 * NSYM];
struct atom *arglist[27 * NSYM];

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
int drawing;
int interrupt;
int jmpsel;
jmp_buf jmpbuf0;
jmp_buf jmpbuf1;
char *errmsg;

int block_count;
int free_count;
int gc_count;
int bignum_count;
int ksym_count;
int usym_count;
int string_count;
int tensor_count;
int max_stack;
int max_frame;

char tbuf[1000];

char *outbuf;
int outbuf_index;
int outbuf_length;
