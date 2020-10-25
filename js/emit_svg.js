function
emit_svg(p, x, y)
{
	var dx, dy, i, n, size, w, x1, x2;

	switch (p.type) {

	case SPACE:
		break;

	case TEXT:
		emit_svg_text(p, x, y);
		break;

	case LINE:
		n = p.a.length;
		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}
		break;

	case PAREN:

		emit_svg_delims(p, x, y)

		x += emit_delim_width(p.small_font);

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}

		break;

	case SUPERSCRIPT:

		y += p.depth; // p.depth is negative

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}

		break;

	case SUBSCRIPT:

		if (p.small_font)
			y += SMALL_X_HEIGHT;
		else
			y += X_HEIGHT;

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}

		break;

	case FRACTION:

		// numerator

		dx = (p.width - p.num.width) / 2;

		if (dx < 0)
			dx = 0;

		if (p.small_font)
			dy = p.num.depth + SMALL_X_HEIGHT;
		else
			dy = p.num.depth + X_HEIGHT;

		emit_svg(p.num, x + dx, y - dy);

		// denominator

		dx = (p.width - p.den.width) / 2;

		if (dx < 0)
			dx = 0;

		if (p.small_font)
			dy = p.den.height - SMALL_X_HEIGHT;
		else
			dy = p.den.height - X_HEIGHT;

		emit_svg(p.den, x + dx, y + dy);

		// line

		if (p.small_font) {
			size = SMALL_FONT_SIZE;
			y -= SMALL_X_HEIGHT;
		} else {
			size = FONT_SIZE;
			y -= X_HEIGHT
		}

		w = 1/8 * roman_width['n'.charCodeAt(0)] * WIDTH_RATIO * size;

		x1 = x + w;
		x2 = x + p.width - w;

		emit_svg_line(x1, y, x2, y, 1.5);

		break;

	case TABLE:
		emit_svg_delims(p, x, y);
		emit_svg_table(p, x + emit_delim_width(0), y);
		break;
	}
}
