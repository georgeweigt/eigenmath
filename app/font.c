#include "app.h"

CTFontRef
get_font_ref(int font_num)
{
	return font_ref_tab[font_num];
}

int
get_ascent(int font_num)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font_num);
	h = CTFontGetAscent(f);
	return ceil(h);
}

double
get_char_height(int font_num)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font_num);
	h = CTFontGetAscent(f);
	return h;
}

int
get_descent(int font_num)
{
	CTFontRef f;
	CGFloat d;
	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);
	return ceil(d);
}

double
get_char_depth(int font_num)
{
	CTFontRef f;
	CGFloat d;
	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);
	return d;
}

int
get_xheight(int font_num)
{
	CTFontRef f;
	CGFloat h;
	f = get_font_ref(font_num);
	h = CTFontGetXHeight(f);
	return ceil(h);
}

int
get_width(int font_num, int char_num)
{
	CTFontRef f;
	CFStringRef name;
	CGGlyph g;
	double w;

	f = get_font_ref(font_num);

	name = get_char_name(char_num);

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

int
text_width(int font_num, char *s)
{
	int w = 0;
	while (*s)
		w += get_width(font_num, *s++);
	return w;
}

void
get_height_width(int *h, int *w, int font_num, char *s)
{
	*h = get_ascent(font_num) + get_descent(font_num);
	*w = text_width(font_num, s);
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
