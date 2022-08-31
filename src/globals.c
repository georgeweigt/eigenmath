struct atom *mem[MAXBLOCKS];
struct atom *free_list;

int tos; // top of stack
int tof; // top of frame
int toj; // top of journal

struct atom *stack[STACKSIZE];
struct atom *frame[FRAMESIZE];
struct atom *journal[JOURNALSIZE];

struct atom *symtab[27 * NSYM];
struct atom *binding[27 * NSYM];
struct atom *usrfunc[27 * NSYM];

struct atom *zero;
struct atom *one;
struct atom *minusone;
struct atom *imaginaryunit;

int level;
int expanding;
int drawing;
int journaling;
int interrupt;
jmp_buf jmpbuf0;
jmp_buf jmpbuf1;

int alloc_count;
int block_count;
int free_count;
int gc_count;
int bignum_count;
int ksym_count;
int usym_count;
int string_count;
int tensor_count;
int max_level;
int max_stack;
int max_frame;
int max_journal;

char strbuf[STRBUFLEN];

char *outbuf;
int outbuf_index;
int outbuf_length;
