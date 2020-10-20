function
emit_svg(p, x, y)
{
	var dx, dy, i, n, x1, x2;

	switch (p.type) {

	case SPACE:
		break;

	case TEXT:
		if (p.small_font) {
			if (p.italic_font)
				outbuf += "<text style='font-family:times;font-size:14pt;font-style:italic'";
			else
				outbuf += "<text style='font-family:times;font-size:14pt'";
		} else {
			if (p.italic_font)
				outbuf += "<text style='font-family:times;font-size:20pt;font-style:italic'";
			else
				outbuf += "<text style='font-family:times;font-size:20pt'";
		}
		outbuf += " x='" + x + "' y='" + y + "'>" + p.s + "</text>";
//		if (p.s != ' ')
//			emit_svg_line(x + p.width, y + p.depth, x + p.width, y - p.height);
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
			y -= SMALL_X_HEIGHT;
		else
			y -= X_HEIGHT;

		if (p.small_font) {
			x1 = x + SMALL_THIN_SPACE;
			x2 = x + p.width - SMALL_THIN_SPACE;
		} else {
			x1 = x + THIN_SPACE;
			x2 = x + p.width - THIN_SPACE;
		}

		outbuf += "<line x1='" + x1 + "' y1='" + y + "' x2='" + x2 + "' y2='" + y + "' style='stroke:black' />"

		break;
	}
}
