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

		dx = (p.width - p.num.width) / 2;

		if (dx < 0)
			dx = 0;

		if (p.small_font)
			dy = -SMALL_X_HEIGHT - SMALL_VSPACE - p.num.depth;
		else
			dy = -X_HEIGHT - VSPACE - p.num.depth;

		emit_svg(p.num, x + dx, y + dy);

		dx = (p.width - p.den.width) / 2;

		if (dx < 0)
			dx = 0;

		if (p.small_font)
			dy = -SMALL_X_HEIGHT + SMALL_VSPACE + p.den.height;
		else
			dy = -X_HEIGHT + VSPACE + p.den.height;

		emit_svg(p.den, x + dx, y + dy);

		if (p.small_font)
			size = SMALL_FONT_SIZE;
		else
			size = FONT_SIZE;

		w = 1/12 * roman_width_tab['m'.charCodeAt(0)] * WIDTH_RATIO * size;

		x1 = x + w;
		x2 = x + p.width - w;

		y = y - X_HEIGHT_RATIO * size;

		emit_svg_line(x1, y, x2, y, 1);

		break;

	case TABLE:
		emit_svg_delims(p, x, y);
		emit_svg_table(p, x + emit_delim_width(0), y);
		break;
	}
}
