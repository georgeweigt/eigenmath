function
emit_svg(p, x, y)
{
	var i, n;

	switch (p.type) {

	case SPACE:

		break;

	case ROMAN:

		x += p.width / 2;

		outbuf += "<text text-anchor='middle'";

		if (p.small_font)
			outbuf += " style='font-family:times;font-size:14pt'";
		else
			outbuf += " style='font-family:times;font-size:20pt'";

		outbuf += " x='" + x + "' y='" + y + "'>" + p.s + "</text>";

		break;

	case ITALIC:

		x += p.width / 2;

		outbuf += "<text text-anchor='middle'";

		if (p.small_font)
			outbuf += " style='font-family:italic;font-size:14pt'";
		else
			outbuf += " style='font-family:italic;font-size:20pt'";

		outbuf += " x='" + x + "' y='" + y + "'>" + p.s + "</text>";

		break;

	case LINE:

		y += p.height;

		n = p.a.length;

		for (i = 0; i < n; i++) {
			emit_svg(p.a[i], x, y);
			x += p.width;
		}

		break;
	}
}
