#include "app.h"

void
draw_text(double x, double y, uint8_t *buf, int len)
{
	int i;
	CGPoint p;
	CTFontRef f;
	CFStringRef s;
	CGGlyph g;

	y = document_height - y;

	f = get_font_ref(DEFAULT_FONT);

	for (i = 0; i < len; i++) {

		p.x = x;
		p.y = y;

		s = get_char_name(buf[i]);
		g = CTFontGetGlyphWithName(f, s);

		CTFontDrawGlyphs(f, &g, &p, 1, gcontext);

		x += CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);
	}
}
