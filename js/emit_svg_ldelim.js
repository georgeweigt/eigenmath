function
emit_svg_ldelim(p, x, y)
{
	var t, w;

	if (p.small_font) {
		t = 1;
		w = emit_delim_width(1);
	} else {
		t = 2.5;
		w = emit_delim_width(0);
	}

	var x1 = x + w / 2;
	var x2 = x + w;

	var y1 = y - p.height + 2 * t;
	var y2 = y + p.depth - 2 * t;

	emit_svg_line(x1, y1, x1, y2, t); // stem
	emit_svg_line(x1, y1, x2, y1, t); // top segment
	emit_svg_line(x1, y2, x2, y2, t); // bottom segment
}
