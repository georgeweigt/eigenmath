struct atom *mem[MAXBLOCKS]; // an array of pointers
struct atom *free_list;

int tos; // top of stack
int tof; // top of frame

struct atom *stack[STACKSIZE];
struct atom *frame[FRAMESIZE];

struct atom *symtab[27 * BUCKETSIZE];
struct atom *binding[27 * BUCKETSIZE];
struct atom *usrfunc[27 * BUCKETSIZE];

struct atom *zero;
struct atom *one;
struct atom *minusone;
struct atom *imaginaryunit;

int eval_level;
int gc_level;
int expanding;
int drawing;
int nonstop;
int interrupt;
jmp_buf jmpbuf0;
jmp_buf jmpbuf1;
char *trace1;
char *trace2;

int alloc_count;
int block_count;
int free_count;
int gc_count;
int bignum_count;
int ksym_count;
int usym_count;
int string_count;
int tensor_count;
int max_eval_level;
int max_tos;
int max_tof;

char strbuf[STRBUFLEN];

char *outbuf;
int outbuf_index;
int outbuf_length;
