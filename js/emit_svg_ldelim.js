function
emit_svg_ldelim(u, x, y)
{
	var t, w;

	if (u.type == TABLE)
		t = TABLE_DELIM_STROKE;
	else if (u.level == 0)
		t = DELIM_STROKE;
	else
		t = SMALL_DELIM_STROKE;

	w = emit_delim_width(u);

	var x1 = x + 0.25 * w;
	var x2 = x + 0.75 * w;

	var y1 = y - u.height + 0.5 * t;
	var y2 = y + u.depth - 0.5 * t;

	emit_svg_line(x1, y1, x1, y2, t); // stem
	emit_svg_line(x1, y1, x2, y1, t); // top segment
	emit_svg_line(x1, y2, x2, y2, t); // bottom segment
}
