#include "app.h"

void
draw_char(double x, double y, int font_num, int char_num)
{
	CGPoint p;
	CTFontRef f;
	CFStringRef s;
	CGGlyph g;

	y = document_height - y; // flip y coordinate

//	x = round(x);
//	y = round(y);

	p.x = x;
	p.y = y;

	f = get_font_ref(font_num);
	s = get_char_name(char_num);
	g = CTFontGetGlyphWithName(f, s);

	CTFontDrawGlyphs(f, &g, &p, 1, gcontext);
}
