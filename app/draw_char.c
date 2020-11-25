#include "app.h"

void
draw_char(float x, float y, int font_num, int char_num)
{
	CTFontRef f;
	CGGlyph g;
	CGPoint p;
	CFStringRef name;

	y = app_total_h - y; // flip y coordinate

	p.x = roundf(x);
	p.y = roundf(y);

	f = get_font_ref(font_num);
	name = get_char_name(char_num);
	g = CTFontGetGlyphWithName(f, name);
	CTFontDrawGlyphs(f, &g, &p, 1, gcontext);
}
