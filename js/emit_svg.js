function
emit_svg(p, x, y)
{
	var dx, dy, i, n, x1, x2;

	switch (p.type) {

	case SPACE:
		break;

	case ROMAN:

		if (p.small_font)
			outbuf += "<text style='font-family:monospace;font-size:14pt'";
		else
			outbuf += "<text style='font-family:monospace;font-size:20pt'";

		outbuf += " x='" + x + "' y='" + y + "'>" + p.s + "</text>";

		break;

	case ITALIC:

		if (p.small_font)
			outbuf += "<text style='font-family:monospace;font-size:14pt;font-style:italic'";
		else
			outbuf += "<text style='font-family:monospace;font-size:20pt;font-style:italic'";

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

		if (p.small_font)
			outbuf += "<text style='font-family:monospace;font-size:14pt'"
		else
			outbuf += "<text style='font-family:monospace;font-size:20pt'"

		outbuf += " x='" + x + "' y='" + y + "'>" + "(</text>";

		if (p.small_font)
			x += SMALL_FONT_WIDTH;
		else
			x += FONT_WIDTH;

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}

		if (p.small_font)
			outbuf += "<text style='font-family:monospace;font-size:14pt'"
		else
			outbuf += "<text style='font-family:monospace;font-size:20pt'"

		outbuf += " x='" + x + "' y='" + y + "'>" + ")</text>";

		break;

	case BRACK:

		if (p.small_font)
			outbuf += "<text style='font-family:monospace;font-size:14pt'"
		else
			outbuf += "<text style='font-family:monospace;font-size:20pt'"

		outbuf += " x='" + x + "' y='" + y + "'>" + "[</text>";

		if (p.small_font)
			x += SMALL_FONT_WIDTH;
		else
			x += FONT_WIDTH;

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.a[i].width;
		}

		if (p.small_font)
			outbuf += "<text style='font-family:monospace;font-size:14pt'"
		else
			outbuf += "<text style='font-family:monospace;font-size:20pt'"

		outbuf += " x='" + x + "' y='" + y + "'>" + "]</text>";

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
			dy = -SMALL_X_HEIGHT - SMALL_FRAC_PAD - p.num.depth;
		else
			dy = -X_HEIGHT - FRAC_PAD - p.num.depth;

		emit_svg(p.num, x + dx, y + dy);

		dx = (p.width - p.den.width) / 2;

		if (dx < 0)
			dx = 0;

		if (p.small_font)
			dy = -SMALL_X_HEIGHT + SMALL_FRAC_PAD + p.den.height;
		else
			dy = -X_HEIGHT + FRAC_PAD + p.den.height;

		emit_svg(p.den, x + dx, y + dy);

		if (p.small_font)
			y -= SMALL_X_HEIGHT;
		else
			y -= X_HEIGHT;

		if (p.small_font) {
			x1 = x + SMALL_FRAC_PAD / 2;
			x2 = x + p.width - SMALL_FRAC_PAD / 2;
		} else {
			x1 = x + FRAC_PAD / 2;
			x2 = x + p.width - FRAC_PAD / 2;
		}

		outbuf += "<line x1='" + x1 + "' y1='" + y + "' x2='" + x2 + "' y2='" + y + "' style='stroke:black' />"

		break;
	}
}
