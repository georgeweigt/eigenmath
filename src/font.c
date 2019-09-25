#include "defs.h"
#include <ApplicationServices/ApplicationServices.h>

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

extern CFStringRef glyph_name[];

CGFontRef ftab[11];

struct text_metric text_metric[11];

void
init_font(void)
{
	int i, w;
	CGGlyph g;

//	ftab[0] = CGFontCreateWithFontName(CFSTR("Lucida Grande"));
	ftab[0] = CGFontCreateWithFontName(CFSTR("Courier"));

	ftab[1] = ftab[0];
	ftab[2] = ftab[0];

	ftab[3] = CGFontCreateWithFontName(CFSTR("Times New Roman"));
	ftab[4] = CGFontCreateWithFontName(CFSTR("Times New Roman Italic"));

	ftab[5] = CGFontCreateWithFontName(CFSTR("Times New Roman"));
	ftab[6] = CGFontCreateWithFontName(CFSTR("Times New Roman Italic"));

	ftab[7] = ftab[3];
	ftab[8] = ftab[4];

	ftab[9] = ftab[5];
	ftab[10] = ftab[6];

	text_metric[0].size = 16.0;
	text_metric[1].size = 16.0;
	text_metric[2].size = 16.0;
	text_metric[3].size = 20.0;
	text_metric[4].size = 20.0;
	text_metric[5].size = 20.0;
	text_metric[6].size = 20.0;
	text_metric[7].size = 16.0;
	text_metric[8].size = 16.0;
	text_metric[9].size = 16.0;
	text_metric[10].size = 16.0;

	for (i = 0; i < 11; i++) {
		text_metric[i].em = CGFontGetUnitsPerEm(ftab[i]);
		text_metric[i].ascent = ceil(1.0 * text_metric[i].size * (float) CGFontGetAscent(ftab[i]) / text_metric[i].em);
		text_metric[i].descent = ceil(-1.0 * text_metric[i].size * (float) CGFontGetDescent(ftab[i]) / text_metric[i].em);

		g = CGFontGetGlyphWithGlyphName(ftab[i], glyph_name['m' - 32]); // get glyph for 'm'
		CGFontGetGlyphAdvances(ftab[i], &g, 1, &w); // get width of 'm'
		text_metric[i].width = ceil(1.0 * text_metric[i].size * (float) w / text_metric[i].em);
		text_metric[i].xheight = ceil(1.0 * text_metric[i].size * (float) CGFontGetXHeight(ftab[i]) / text_metric[i].em);

	}
}

extern CGContextRef gcontext;

extern int app_total_h;
extern int app_total_w;

// table for converting symbols to glyph codes

#if 0
static int gtab[38] = {
	299,	// Gamma
	507,	// Delta
	300,	// Theta
	513,	// Lambda
	516,	// Xi
	518,	// Pi
	520,	// Sigma
	522,	// Upsilon
	301,	// Phi
	524,	// Psi
	525,	// Omega
	302,	// alpha
	533,	// beta
	534,	// gamma
	303,	// delta
	304,	// epsilon
	535,	// zeta
	536,	// eta
	537,	// theta
	538,	// iota
	539,	// kappa
	540,	// lambda
	541,	// mu
	542,	// nu
	543,	// xi
	155,	// pi
	545,	// rho
	305,	// sigma
	306,	// tau
	547,	// upsilon
	1483,	// phi (307 for alternate phi)
	548,	// chi
	549,	// psi
	550,	// omega
	238,	// multipication symbol
	152,	// partial derivative symbol
	237,	// minus

//	0,	// dim square
//	135,	// bullet
//	185,	// open diamond
	259,	// center dot
//	314,	// right arrow
//	377,	// right triangle
};
#endif

void
draw_text(int font, int x, int y, char *s, int len, int color)
{
	int c, i, w;
	CGGlyph g;
	y = app_total_h - (y + text_metric[font].ascent);
	CGContextSetFont(gcontext, ftab[font]);
	CGContextSetFontSize(gcontext, text_metric[font].size);
	switch (color) {
	case BLUE:
		CGContextSetRGBFillColor(gcontext, 0, 0, 1, 1);
		break;
	case RED:
		CGContextSetRGBFillColor(gcontext, 1, 0, 0, 1);
		break;
	}
	for (i = 0; i < len; i++) {
		CGContextSetTextPosition(gcontext, (float) x, (float) y);
		c = s[i];
		c = (c & 0xff) - 32;
		if (c < 0 || c >= 133)
			c = '?' - 32;
		g = CGFontGetGlyphWithGlyphName(ftab[font], glyph_name[c]);
		CGContextShowGlyphs(gcontext, &g, 1);
		CGFontGetGlyphAdvances(ftab[font], &g, 1, &w);
		w = ceil(text_metric[font].size * (float) w / text_metric[font].em);
		x += w;
	}
	if (color != BLACK)
		CGContextSetRGBFillColor(gcontext, 0, 0, 0, 1);
}

int
text_width(int font, char *s)
{
	int c, t = 0, w;
	CGGlyph g;
	while (*s) {
		c = *s++;
		c = (c & 0xff) - 32;
		if (c < 0 || c >= 133)
			c = '?' - 32;
		g = CGFontGetGlyphWithGlyphName(ftab[font], glyph_name[c]);
		CGFontGetGlyphAdvances(ftab[font], &g, 1, &w);
		t += ceil(text_metric[font].size * (float) w / text_metric[font].em);
	}
	return t;
}

void
get_height_width(int *h, int *w, int font, char *s)
{
	*h = text_metric[font].ascent + text_metric[font].descent;
	*w = text_width(font, s);
}

// From p. 85 of CGContext.pdf
//
// The default line width is 1 unit. When stroked, the line straddles the path,
// with half of the total width on either side.
//
// My note:
// That's why you have to add 0.5 to get a line that's one pixel wide.
// Half a pixel is drawn on either side.
//
// draw((0,0)) should land right in the middle of the cross-hairs.

void
draw_line(int x1, int y1, int x2, int y2)
{
	float fx1, fx2, fy1, fy2;

	// not offsetting by 0.5, results in 2-pixel width to match draw_point()

	fx1 = x1;
	fx2 = x2;
	fy1 = app_total_h - y1;
	fy2 = app_total_h - y2;

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);

	CGContextSetLineWidth(gcontext, 0.5);
	CGContextStrokePath(gcontext);
	CGContextSetLineWidth(gcontext, 1.0);
}

void
draw_left_bracket(int x, int y, int w, int h)
{
	float x1, x2, y1, y2;
	x1 = x + 0.5;
	x2 = x + w + 0.5;
	y1 = app_total_h - (y + 0.5);
	y2 = app_total_h - (y + h + 0.5);

	x1 += 2.0;
	x2 -= 2.0;

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, x2, y1);
	CGContextAddLineToPoint(gcontext, x1, y1);
	CGContextAddLineToPoint(gcontext, x1, y2);
	CGContextAddLineToPoint(gcontext, x2, y2);

	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
}

void
draw_right_bracket(int x, int y, int w, int h)
{
	float x1, x2, y1, y2;
	x1 = x + 0.5;
	x2 = x + w + 0.5;
	y1 = app_total_h - (y + 0.5);
	y2 = app_total_h - (y + h + 0.5);

	x1 += 2.0;
	x2 -= 2.0;

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, x1, y1);
	CGContextAddLineToPoint(gcontext, x2, y1);
	CGContextAddLineToPoint(gcontext, x2, y2);
	CGContextAddLineToPoint(gcontext, x1, y2);

	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
}

// Draw a little square at coordinates x + dx, y + dy

void
draw_point(int x, int dx, int y, int dy)
{
	float fx1, fx2, fy1, fy2;

	fx1 = x + dx - 0.5;
	fx2 = x + dx + 0.5;

	fy1 = app_total_h - (y + dy - 0.5);
	fy2 = app_total_h - (y + dy + 0.5);

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);
	CGContextAddLineToPoint(gcontext, fx1, fy2);
	CGContextClosePath(gcontext);

//	CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 1.0, 1.0);
	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
//	CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 0.0, 1.0);
}

void
draw_box(int x1, int y1, int x2, int y2)
{
	float fx1, fx2, fy1, fy2;

	fx1 = x1 - 0.5;
	fx2 = x2 + 0.5;

	fy1 = app_total_h - (y1 - 0.5);
	fy2 = app_total_h - (y2 + 0.5);

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);
	CGContextAddLineToPoint(gcontext, fx1, fy2);
	CGContextClosePath(gcontext);

	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
}

void
draw_hrule(int x, int y, int w)
{
	float fy;

	fy = app_total_h - (y + 0.5);

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, x, fy);
	CGContextAddLineToPoint(gcontext, x + w, fy);

	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
}

void
draw_selection_rect(float x, float y, float width, float height)
{
	float fx1, fx2, fy1, fy2;
	static CGFloat lengths[2] = {2.0, 2.0}; // for dashed line

	// both equal to 0.0 means there is no selection

	if (width == 0.0 && height == 0.0)
		return;

	// pixel alignment

	fx1 = floor(x);
	fx2 = ceil(x + width);

	fy1 = floor(y);
	fy2 = ceil(y + height);

	if (fx1 < 0)
		fx1 = 0;

	if (fx2 >= app_total_w)
		fx2 = app_total_w - 1;

	if (fy1 < 0)
		fy1 = 0;

	if (fy2 >= app_total_h)
		fy2 = app_total_h - 1;

	// this is required to get a line that is exactly 1 pixel wide

	fx1 += 0.5;
	fx2 += 0.5;

	fy1 += 0.5;
	fy2 += 0.5;

	// set up the path

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);
	CGContextAddLineToPoint(gcontext, fx1, fy2);
	CGContextClosePath(gcontext);

	// draw the rectangle

	CGContextSetLineDash(gcontext, 0.0, lengths, 2);
	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
	CGContextSetLineDash(gcontext, 0.0, NULL, 0);
}
