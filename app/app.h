#include "../src/defs.h"
#include <ApplicationServices/ApplicationServices.h>

struct display {
	struct display *next;
	uint8_t type, attr;
	int h, w;
	int tot_h, tot_w;
	int len;
	float tab[1];
	uint8_t buf[0];
};

#define SMALL_FONT 1
#define DEFAULT_FONT 2
#define TIMES_FONT 3
#define ITALIC_TIMES_FONT 4
#define SYMBOL_FONT 5
#define ITALIC_SYMBOL_FONT 6
#define SMALL_TIMES_FONT 7
#define SMALL_ITALIC_TIMES_FONT 8
#define SMALL_SYMBOL_FONT 9
#define SMALL_ITALIC_SYMBOL_FONT 10

#define DRAW_STOP 0
#define DRAW_CHAR 1
#define DRAW_STROKE 2

#define DRAW_HRULE 20
#define DRAW_LEFT_BRACKET 21
#define DRAW_RIGHT_BRACKET 22
#define DRAW_LINE 23
#define DRAW_POINT 24
#define DRAW_BOX 25
#define DRAW_SPACE 32

#define ITALIC_FONT ITALIC_TIMES_FONT
#define SMALL_ITALIC_FONT SMALL_ITALIC_TIMES_FONT

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

#define MINUS_HEIGHT 8.5
#define SUPERSCRIPT_HEIGHT 10.0
#define SUBSCRIPT_DEPTH 7.0
#define FRAC_VSPACE 5.5
#define FRAC_STROKE 1.5
#define DELIM_STROKE 2.0

#define SMALL_MINUS_HEIGHT 6.0
#define SMALL_SUPERSCRIPT_HEIGHT 10.0
#define SMALL_SUBSCRIPT_DEPTH 7.0
#define SMALL_FRAC_VSPACE 4.0
#define SMALL_FRAC_STROKE 1.0
#define SMALL_DELIM_STROKE 1.5

#define TABLE_HSPACE 12.0
#define TABLE_VSPACE 12.0
#define TABLE_STROKE 2.5

#define OPCODE(p) car(p)->u.d
#define HEIGHT(p) car(cdr(p))->u.d
#define DEPTH(p) car(cdr(cdr(p)))->u.d
#define WIDTH(p) car(cdr(cdr(cdr(p))))->u.d
#define DX car(cdr(cdr(cdr(cdr))))->u.d
#define DY car(cdr(cdr(cdr(cdr(cdr(p))))))->u.d

#define HBAR 176
#define TIMES 177
#define PARTIAL 178
#define MINUS 179
#define GREATEREQUAL 180
#define LESSEQUAL 181

extern CGContextRef gcontext;

extern int app_total_h;
extern int app_total_w;

extern struct display *emit_display;
extern int emit_level;
extern int emit_index;
extern int emit_count;

#include "app-prototypes.h"
