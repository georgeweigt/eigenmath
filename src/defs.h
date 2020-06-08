#include "defs1.h"
#include "prototypes.h"

extern struct atom *mem[MAXBLOCKS];
extern struct atom *free_list;

extern int tos; // top of stack
extern int tof; // top of frame
 
extern struct atom *stack[STACKSIZE];
extern struct atom *frame[FRAMESIZE];
 
extern struct atom symtab[NSYM];
extern struct atom *binding[NSYM];
extern struct atom *arglist[NSYM];
 
extern struct atom *p0;
extern struct atom *p1;
extern struct atom *p2;
extern struct atom *p3;
extern struct atom *p4;
extern struct atom *p5;
extern struct atom *p6;
extern struct atom *p7;
extern struct atom *p8;
extern struct atom *p9;
 
extern struct atom *zero;
extern struct atom *one;
extern struct atom *minusone;
extern struct atom *imaginaryunit;

extern int expanding;
extern int interrupt;
extern int draw_flag;
extern int clear_flag;

extern jmp_buf stop_return;
extern jmp_buf draw_stop_return;

extern int block_count;
extern int free_count;
extern int gc_count;
extern int bignum_count;
extern int string_count;
extern int tensor_count;
extern int max_stack;
extern int max_frame;
 
extern char tbuf[];

extern char *outbuf;
extern int outbuf_index;
extern int outbuf_length;
