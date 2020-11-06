function
draw_labels()
{
	var p, u, x, y;

	push_double(ymax);
	p = pop();
	u = emit_line(p, 1);
	x = DRAW_LEFT_PAD - u.width - SMALL_FONT_SIZE;
	y = DRAW_TOP_PAD + u.height;
	emit_svg(u, x, y);

	push_double(ymin);
	p = pop();
	u = emit_line(p, 1);
	x = DRAW_LEFT_PAD - u.width - SMALL_FONT_SIZE;
	y = DRAW_TOP_PAD + DRAW_HEIGHT;
	emit_svg(u, x, y);

	push_double(xmin);
	p = pop();
	u = emit_line(p, 1);
	x = DRAW_LEFT_PAD - u.width / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + 2 * SMALL_FONT_SIZE;
	emit_svg(u, x, y);

	push_double(xmax);
	p = pop();
	u = emit_line(p, 1);
	x = DRAW_LEFT_PAD + DRAW_WIDTH - u.width / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + 2 * SMALL_FONT_SIZE;
	emit_svg(u, x, y);
}
