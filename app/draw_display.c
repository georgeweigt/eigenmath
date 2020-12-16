#include "app.h"

// app calls check_display() then prep_display() then draw_display()

void
draw_display(double ymin, double ymax)
{
	double y;
	struct display *p;

	ymin = document_height - ymin; // convert from screen coordinates
	ymax = document_height - ymax;

	y = document_height;

	p = fence;

	while (p) {
		y -= p->height;
		draw_display_nib(p, y, ymin, ymax);
		p = p->next;
	}
}

void
draw_display_nib(struct display *p, double y, double ymin, double ymax)
{
	// clip to view rect

	if (y + p->height < ymin || y > ymax)
		return;

	switch (p->color) {
	case BLACK:
		CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 0.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 0.0, 1.0);
		break;
	case BLUE:
		CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 1.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 1.0, 1.0);
		break;
	case RED:
		CGContextSetRGBFillColor(gcontext, 1.0, 0.0, 0.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 1.0, 0.0, 0.0, 1.0);
		break;
	}

	switch (p->type) {

	case 0:
		draw_text(p->dx, y + p->dy, p->buf, p->len);
		break;

	case 1:
		draw_formula(p->dx, y + p->dy, p->mem);
		break;
	}
}

void
draw_text(double x, double y, uint8_t *buf, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		draw_char(x, y, TEXT_FONT, buf[i]);
		x += get_char_width(TEXT_FONT, buf[i]);
	}
}

// draws graphs too

void
draw_formula(double x, double y, float *p)
{
	for (;;) {

		switch ((int) *p) {

		case DRAW_CHAR:
			draw_char(x + p[1], y + p[2], p[3], p[4]);
			p += 5;
			break;

		case DRAW_STROKE:
			draw_stroke(x + p[1], y + p[2], x + p[3], y + p[4], p[5]);
			p += 6;
			break;

		case DRAW_POINT:
			draw_point(x + p[1], y + p[2]);
			p += 3;
			break;

		default:
			return;
		}
	}
}

void
draw_char(double x, double y, int font_num, int char_num)
{
	CGPoint p;
	CTFontRef f;
	CFStringRef s;
	CGGlyph g;

	y = document_height - y; // flip y coordinate

	p.x = x;
	p.y = y;

	f = get_font_ref(font_num);
	s = get_char_name(char_num);
	g = CTFontGetGlyphWithName(f, s);

	CTFontDrawGlyphs(f, &g, &p, 1, gcontext);
}

void
draw_stroke(double x1, double y1, double x2, double y2, double stroke_width)
{
	CGPoint p[2];

	y1 = document_height - y1;
	y2 = document_height - y2;

	p[0].x = x1;
	p[0].y = y1;

	p[1].x = x2;
	p[1].y = y2;

	CGContextSetLineWidth(gcontext, stroke_width);
	CGContextStrokeLineSegments(gcontext, p, 2);
}

#define R 2.0

void
draw_point(double x, double y)
{
	CGRect r;

	y = document_height - y;

	r.origin.x = x - R;
	r.origin.y = y - R;

	r.size.height = 2.0 * R;
	r.size.width = 2.0 * R;

	CGContextFillEllipseInRect(gcontext, r);
}
