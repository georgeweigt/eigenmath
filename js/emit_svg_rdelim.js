function
emit_svg_rdelim(p, x, y)
{
	var d, h, s, w, x1, x2, y1, y2;

	if (p.small_font) {
		s = 2;
		w = SMALL_DELIM_WIDTH;
	} else {
		s = 3;
		w = DELIM_WIDTH;
	}

	h = p.height;
	d = p.depth;

	x1 = x + w - s / 2;
	x2 = x + w - s / 2;

	y1 = y - h;
	y2 = y + d;

	outbuf += "<line x1='" + x1 + "' y1='" + y1 + "' x2='" + x2 + "' y2='" + y2 + "' style='stroke:black;stroke-width:" + s + "'/>"

	x1 = x;
	x2 = x + w;

	y1 = y - h + s / 2;
	y2 = y - h + s / 2;

	outbuf += "<line x1='" + x1 + "' y1='" + y1 + "' x2='" + x2 + "' y2='" + y2 + "' style='stroke:black;stroke-width:" + s + "'/>"

	y1 = y + d - s / 2;
	y2 = y + d - s / 2;

	outbuf += "<line x1='" + x1 + "' y1='" + y1 + "' x2='" + x2 + "' y2='" + y2 + "' style='stroke:black;stroke-width:" + s + "'/>"
}
