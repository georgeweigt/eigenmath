#include "app.h"

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
