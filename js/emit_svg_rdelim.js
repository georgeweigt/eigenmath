function
emit_svg_rdelim(u, x, y)
{
	var t, w;

	if (u.type == TABLE) {
		t = TABLE_DELIM_STROKE;
		w = emit_delim_width(0);
	} else if (u.small_font) {
		t = SMALL_DELIM_STROKE;
		w = emit_delim_width(1);
	} else {
		t = DELIM_STROKE;
		w = emit_delim_width(0);
	}

	var x1 = x + u.width - 0.25 * w;
	var x2 = x + u.width - 0.75 * w;

	var y1 = y - u.height + 0.5 * t;
	var y2 = y + u.depth - 0.5 * t;

	emit_svg_line(x1, y1, x1, y2, t); // stem
	emit_svg_line(x1, y1, x2, y1, t); // top segment
	emit_svg_line(x1, y2, x2, y2, t); // bottom segment
}
