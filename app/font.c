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
get_font_ref(int font_num)
{
	CTFontRef f;

	switch (font_num)
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
