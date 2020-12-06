function
draw_labels()
{
	var p, x, y;

	push_double(ymax);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD - width(p) - SMALL_FONT_SIZE;
	y = DRAW_TOP_PAD + height(p);
	draw_formula(x, y, p);

	push_double(ymin);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD - width(p) - SMALL_FONT_SIZE;
	y = DRAW_TOP_PAD + DRAW_HEIGHT;
	draw_formula(x, y, p);

	push_double(xmin);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD - width(p) / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + 2 * SMALL_FONT_SIZE;
	draw_formula(x, y, p);

	push_double(xmax);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD + DRAW_WIDTH - width(p) / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + 2 * SMALL_FONT_SIZE;
	draw_formula(x, y, p);
}
