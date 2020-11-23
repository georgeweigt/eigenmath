#include "app.h"

CTFontRef roman10;
CTFontRef roman7;

CTFontRef italic10;
CTFontRef italic7;

CTFontRef courier7;

void
init_font(void)
{
	roman10 = CTFontCreateWithName(CFSTR("Times New Roman"), 20.0, NULL);
	roman7 = CTFontCreateWithName(CFSTR("Times New Roman"), 16.0, NULL);

	italic10 = CTFontCreateWithName(CFSTR("Times New Roman Italic"), 20.0, NULL);
	italic7 = CTFontCreateWithName(CFSTR("Times New Roman Italic"), 16.0, NULL);

	courier7 = CTFontCreateWithName(CFSTR("Courier"), 16.0, NULL);
}

CTFontRef
get_font_ref(int font)
{
	CTFontRef f;

	switch (font)
	{
	case TIMES_FONT:
	case SYMBOL_FONT:
		f = roman10;
		break;

	case SMALL_TIMES_FONT:
	case SMALL_SYMBOL_FONT:
		f = roman7;
		break;

	case ITALIC_TIMES_FONT:
	case ITALIC_SYMBOL_FONT:
		f = italic10;
		break;

	case SMALL_ITALIC_TIMES_FONT:
	case SMALL_ITALIC_SYMBOL_FONT:
		f = italic7;
		break;

	default:
		f = courier7;
		break;
	}

	return f;
}

int
get_ascent(int font)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font);
	h = CTFontGetAscent(f);
	return ceil(h);
}

double
get_char_height(int font)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font);
	h = CTFontGetAscent(f);
	return h;
}

int
get_descent(int font)
{
	CTFontRef f;
	CGFloat d;
	f = get_font_ref(font);
	d = CTFontGetDescent(f);
	return ceil(d);
}

double
get_char_depth(int font)
{
	CTFontRef f;
	CGFloat d;
	f = get_font_ref(font);
	d = CTFontGetDescent(f);
	return d;
}

int
get_xheight(int font)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font);
	h = CTFontGetXHeight(f);
	return ceil(h);
}

int
get_width(int font, int c)
{
	CTFontRef f;
	CFStringRef name;
	CGGlyph g;
	double w;

	f = get_font_ref(font);

	name = get_char_name(c);

	g = CTFontGetGlyphWithName(f, name);
	w = CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);

	return ceil(w);
}

double
get_char_width(int font_num, int char_num)
{
	CTFontRef f;
	CFStringRef name;
	CGGlyph g;
	double w;

	f = get_font_ref(font_num);
	name = get_char_name(char_num);
	g = CTFontGetGlyphWithName(f, name);
	w = CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);

	return w;
}

void
draw_text(int font, int x, int y, uint8_t *s, int len, int color)
{
	int i;
	double w;
	CTFontRef f;
	CGGlyph g;
	CGPoint p;
	CFStringRef name;

	y = app_total_h - (y + get_ascent(font));

	switch (color) {
	case BLUE:
		CGContextSetRGBFillColor(gcontext, 0, 0, 1, 1);
		break;
	case RED:
		CGContextSetRGBFillColor(gcontext, 1, 0, 0, 1);
		break;
	}

	f = get_font_ref(font);

	for (i = 0; i < len; i++) {

		p.x = (float) x;
		p.y = (float) y;

		name = get_char_name(s[i]);

		g = CTFontGetGlyphWithName(f, name);
		CTFontDrawGlyphs(f, &g, &p, 1, gcontext);
		w = CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);
		x += ceil(w);
	}

	if (color != BLACK)
		CGContextSetRGBFillColor(gcontext, 0, 0, 0, 1);
}

int
text_width(int font, char *s)
{
	int w = 0;
	while (*s)
		w += get_width(font, *s++);
	return w;
}

void
get_height_width(int *h, int *w, int font, char *s)
{
	*h = get_ascent(font) + get_descent(font);
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
