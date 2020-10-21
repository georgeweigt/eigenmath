function
emit_svg_ldelim(p, x, y)
{
	var d, h, t, w, x1, x2, y1, y2;

	if (p.small_font) {
		t = 1;
		w = SMALL_DELIM_WIDTH
	} else {
		t = 2;
		w = DELIM_WIDTH;
	}

	h = p.height;
	d = p.depth;

	x1 = x + w / 2;
	x2 = x + w / 2;

	y1 = y - h;
	y2 = y + d;

	emit_svg_line(x1, y1, x2, y2, t); // vertical line

	x1 = x + w / 2;
	x2 = x + w;

	y1 = y - h + t / 2;
	y2 = y - h + t / 2;

	emit_svg_line(x1, y1, x2, y2, t); // top segment

	y1 = y + d - t / 2;
	y2 = y + d - t / 2;

	emit_svg_line(x1, y1, x2, y2, t); // bottom segment
}
