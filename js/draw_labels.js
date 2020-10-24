function
draw_labels()
{
	var p, x, y;

	push_double(ymax);
	p = pop();
	p = emit_line(p, 1);
	x = DRAW_LEFT_PAD - p.width - SMALL_FONT_SIZE;
	y = DRAW_TOP_PAD + p.height;
	emit_svg(p, x, y);

	push_double(ymin);
	p = pop();
	p = emit_line(p, 1);
	x = DRAW_LEFT_PAD - p.width - SMALL_FONT_SIZE;
	y = DRAW_TOP_PAD + DRAW_HEIGHT;
	emit_svg(p, x, y);

	push_double(xmin);
	p = pop();
	p = emit_line(p, 1);
	x = DRAW_LEFT_PAD - p.width / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + 2 * SMALL_FONT_SIZE;
	emit_svg(p, x, y);

	push_double(xmax);
	p = pop();
	p = emit_line(p, 1);
	x = DRAW_LEFT_PAD + DRAW_WIDTH - p.width / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + 2 * SMALL_FONT_SIZE;
	emit_svg(p, x, y);
}
