function
emit_svg(u, x, y)
{
	var dx, i, n, x1, x2;

	switch (u.type) {

	case SPACE:
		break;

	case TEXT:
		emit_svg_text(u, x, y);
		break;

	case LINE:
		n = u.a.length;
		for (i = 0; i < n; i++) {
			emit_svg(u.a[i], x, y);
			x += u.a[i].width;
		}
		break;

	case PAREN:

		emit_svg_delims(u, x, y)

		x += emit_delim_width(u.small_font);

		n = u.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(u.a[i], x, y);
			x += u.a[i].width;
		}

		break;

	case SUPERSCRIPT:

		y += u.depth; // p.depth is negative

		n = u.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(u.a[i], x, y);
			x += u.a[i].width;
		}

		break;

	case SUBSCRIPT:

		if (u.small_font)
			y += SMALL_MINUS_HEIGHT;
		else
			y += MINUS_HEIGHT;

		n = u.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(u.a[i], x, y);
			x += u.a[i].width;
		}

		break;

	case FRACTION:

		// numerator

		dx = (u.width - u.num.width) / 2;

		if (dx < 0)
			dx = 0;

		emit_svg(u.num, x + dx, y - u.height + u.num.height);

		// denominator

		dx = (u.width - u.den.width) / 2;

		if (dx < 0)
			dx = 0;

		emit_svg(u.den, x + dx, y + u.depth - u.den.depth);

		// line

		x1 = x;
		x2 = x + u.width;

		if (u.small_font) {
			y -= SMALL_MINUS_HEIGHT;
			emit_svg_line(x1, y, x2, y, SMALL_HRULE_WIDTH);
		} else {
			y -= MINUS_HEIGHT;
			emit_svg_line(x1, y, x2, y, HRULE_WIDTH);
		}

		break;

	case TABLE:
		emit_svg_delims(u, x, y);
		emit_svg_table(u, x + emit_delim_width(0), y);
		break;
	}
}
