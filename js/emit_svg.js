function
emit_svg(p, x, y)
{
	var dx, dy, i, n, size, x1, x2;

	switch (p.type) {

	case SPACE:
		break;

	case TEXT:

//		emit_svg_line(x + p.width, y + p.depth, x + p.width, y - p.height); // for checking char widths

		outbuf += "<text style='text-anchor:middle;font-family:times;";

		if (p.small_font) {
			if (p.italic_font)
				outbuf += "font-size:14pt;font-style:italic;'";
			else
				outbuf += "font-size:14pt;'";
		} else {
			if (p.italic_font)
				outbuf += "font-size:20pt;font-style:italic;'";
			else
				outbuf += "font-size:20pt;'";
		}

		x = emit_round(x + p.width / 2);

		outbuf += " x='" + x + "' y='" + y + "'>" + p.s + "</text>";

		break;

	case LINE:

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}

		break;

	case PAREN:

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}

		break;

	case BRACK:

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

		x1 = emit_round(x + THIN_SPACE_RATIO * size);
		x2 = emit_round(x + p.width - THIN_SPACE_RATIO * size);

		y = emit_round(y - X_HEIGHT_RATIO * size);

		emit_svg_line(x1, y, x2, y);

		break;
	}
}
