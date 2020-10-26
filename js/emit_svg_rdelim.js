function
emit_svg_rdelim(p, x, y)
{
	var t, w;

	if (p.type == TABLE) {
		t = TABLE_DELIM_STROKE;
		w = emit_delim_width(0);
	} else if (p.small_font) {
		t = SMALL_DELIM_STROKE;
		w = emit_delim_width(1);
	} else {
		t = DELIM_STROKE;
		w = emit_delim_width(0);
	}

	var x1 = x + p.width - w / 2;
	var x2 = x + p.width - w;

	var y1 = y - p.height + 2 * t;
	var y2 = y + p.depth - 2 * t;

	emit_svg_line(x1, y1, x1, y2, t); // stem
	emit_svg_line(x1, y1, x2, y1, t); // top segment
	emit_svg_line(x1, y2, x2, y2, t); // bottom segment
}
