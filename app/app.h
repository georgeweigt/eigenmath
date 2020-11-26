#include "../src/defs.h"
#include <ApplicationServices/ApplicationServices.h>

#define FONT_SIZE 20.0
#define SMALL_FONT_SIZE 16.0

#define HPAD 10
#define VPAD 10

#define TABLE_HSPACE (FONT_SIZE / 2.0)
#define TABLE_VSPACE (FONT_SIZE / 2.0)

#define THIN_STROKE 1.0
#define MEDIUM_STROKE 1.5
#define THICK_STROKE 2.5

#define SMALL_FONT 0
#define DEFAULT_FONT 0

#define ROMAN_FONT 1
#define ITALIC_FONT 2

#define SMALL_ROMAN_FONT 3
#define SMALL_ITALIC_FONT 4

#define DRAW_END 0
#define DRAW_CHAR 1
#define DRAW_STROKE 2

#define DRAW_LINE 23
#define DRAW_POINT 24
#define DRAW_BOX 25
#define DRAW_SPACE 32

#define EMIT_SPACE 1
#define EMIT_CHAR 2
#define EMIT_LIST 3
#define EMIT_SUPERSCRIPT 4
#define EMIT_SUBSCRIPT 5
#define EMIT_SUBEXPR 6
#define EMIT_SMALL_SUBEXPR 7
#define EMIT_FRACTION 8
#define EMIT_SMALL_FRACTION 9
#define EMIT_TABLE 10

#define OPCODE(p) car(p)->u.d
#define HEIGHT(p) cadr(p)->u.d
#define DEPTH(p) caddr(p)->u.d
#define WIDTH(p) cadddr(p)->u.d

#define VAL1(p) car(p)->u.d
#define VAL2(p) cadr(p)->u.d

#define HBAR 176
#define PLUS_SIGN 177
#define MINUS_SIGN 178
#define MULTIPLY_SIGN 179
#define GREATEREQUAL 180
#define LESSEQUAL 181
#define PARTIALDIFF 182

#define GDIM 300

#define DRAW_HEIGHT 300
#define DRAW_WIDTH 300

#define DRAW_LEFT_PAD 120
#define DRAW_RIGHT_PAD 40

#define DRAW_TOP_PAD 0
#define DRAW_BOTTOM_PAD 30

#define DRAW_LABEL_PAD 10

struct display {
	struct display *next;
	int type;
	int color;
	double h;
	double w;
	double document_height;	// for parent process
	double document_width;	// for parent process
	int len;
	uint8_t buf[0];
	double tab[0];
};

extern double document_height;
extern double document_width;

extern CGContextRef gcontext;

extern int cleared;
extern int malloc_kaput_flag;
extern double total_h;
extern double total_w;
extern struct display *first;
extern struct display *barrier;
extern struct display *last;

extern struct display *emit_display;
extern int emit_level;
extern int emit_index;
extern int emit_count;

extern CTFontRef font_ref_tab[5];

#include "app-prototypes.h"
