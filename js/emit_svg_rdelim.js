function
emit_svg_rdelim(p, x, y)
{
	var t, w;

	if (p.small_font) {
		t = 1;
		w = SMALL_DELIM_WIDTH;
	} else {
		t = 2;
		w = DELIM_WIDTH;
	}

	var x1 = Math.round(x + p.width - w / 2);
	var x2 = x1 - w / 2;

	var y1 = Math.round(y - p.height + t);
	var y2 = Math.round(y + p.depth - t);

	emit_svg_line(x1, y1, x1, y2, t); // stem
	emit_svg_line(x1, y1, x2, y1, t); // top segment
	emit_svg_line(x1, y2, x2, y2, t); // bottom segment
}
