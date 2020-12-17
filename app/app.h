#include "../src/defs.h"
#include <ApplicationServices/ApplicationServices.h>

#define HPAD 10
#define VPAD 10

#define FONT_SIZE 20.0
#define SMALL_FONT_SIZE 16.0

#define DELIM_STROKE 0.09
#define FRAC_STROKE 0.07

#define TABLE_HSPACE 12
#define TABLE_VSPACE 10

#define TEXT_FONT 0
#define ROMAN_FONT 1
#define ITALIC_FONT 2
#define SMALL_ROMAN_FONT 3
#define SMALL_ITALIC_FONT 4

#define DRAW_END 0
#define DRAW_CHAR 1
#define DRAW_STROKE 2
#define DRAW_POINT 3

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

#define DRAW_HEIGHT 300
#define DRAW_WIDTH 300

#define DRAW_LEFT_MARGIN 120
#define DRAW_RIGHT_MARGIN 40

#define DRAW_YLABEL_MARGIN 12
#define DRAW_XLABEL_MARGIN 24

#define DRAW_AXIS_STROKE 0.5

#define DRAW_MAX (10 * DRAW_WIDTH)

struct draw_buf_t {
	double t, x, y; // x and y are functions of t
};

struct display {
	struct display *next;
	int type;
	int color;
	int len;
	int pad;
	double height;
	double width;
	double total_height;
	double total_width;
	double dx;
	double dy;
	uint8_t buf[0];
	double mem[0];
};

extern CGContextRef gcontext;

extern double document_height;	// for parent process
extern double document_width;

extern double total_height;	// for thread process
extern double total_width;

extern struct display *fence;

extern int running;

extern int draw_count;
extern struct draw_buf_t draw_buf[];

extern double tmin, tmax;
extern double xmin, xmax;
extern double ymin, ymax;

extern struct display *emit_display;
extern int emit_level;
extern int emit_index;
extern int emit_count;

#include "app-prototypes.h"
