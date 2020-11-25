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
	return h;
}

int
get_descent(int font_num)
{
	CTFontRef f;
	CGFloat d;
	f = get_font_ref(font_num);
	d = CTFontGetDescent(f);
	return d;
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
