function
emit_svg(p, x, y)
{
	var i, n;

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
	}
}
