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

	case SUBEXPR:

		emit_svg_delims(u, x, y)

		x += emit_delim_width(u);

		n = u.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(u.a[i], x, y);
			x += u.a[i].width;
		}

		break;

	case SUPERSCRIPT:

		y += u.depth; // depth is negative

		n = u.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(u.a[i], x, y);
			x += u.a[i].width;
		}

		break;

	case SUBSCRIPT:

		if (u.level == 1)
			y += SUBSCRIPT_DEPTH;
		else
			y += SMALL_SUBSCRIPT_DEPTH;

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

		if (u.level == 0) {
			y -= MINUS_HEIGHT;
			emit_svg_line(x1, y, x2, y, FRAC_STROKE);
		} else {
			y -= SMALL_MINUS_HEIGHT;
			emit_svg_line(x1, y, x2, y, SMALL_FRAC_STROKE);
		}

		break;

	case TABLE:
		emit_svg_delims(u, x, y);
		emit_svg_table(u, x + emit_delim_width(u), y);
		break;
	}
}
