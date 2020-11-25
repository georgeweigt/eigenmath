#include "app.h"

void
draw_text(double x, double y, uint8_t *buf, int len)
{
	int i;
	double d, h;
	CTFontRef f;
	CGGlyph g;
	CFStringRef s;
	CGPoint p;

	h = get_char_height(DEFAULT_FONT);
	d = get_char_depth(DEFAULT_FONT, 'g'); // g has a descender

	y += (VPAD + VPAD - h - d) / 2.0 + h; // vertically center

	y = app_total_h - y;

	f = get_font_ref(DEFAULT_FONT);

	for (i = 0; i < len; i++) {

		p.x = (float) x;
		p.y = (float) y;

		s = get_char_name(buf[i]);
		g = CTFontGetGlyphWithName(f, s);
		CTFontDrawGlyphs(f, &g, &p, 1, gcontext);

		x += CTFontGetAdvancesForGlyphs(f, kCTFontOrientationHorizontal, &g, NULL, 1);
	}
}
