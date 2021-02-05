function
draw_labels()
{
	var p, x, y;

	push_double(ymax);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD - width(p) - DRAW_YLABEL_MARGIN;
	y = DRAW_TOP_PAD + height(p);
	draw_formula(x, y, p);

	push_double(ymin);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD - width(p) - DRAW_YLABEL_MARGIN;
	y = DRAW_TOP_PAD + DRAW_HEIGHT;
	draw_formula(x, y, p);

	push_double(tmin);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD - width(p) / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + DRAW_XLABEL_BASELINE;
	draw_formula(x, y, p);

	push_double(tmax);
	p = pop();
	emit_level = 1; // small font
	emit_list(p);
	p = pop();
	x = DRAW_LEFT_PAD + DRAW_WIDTH - width(p) / 2;
	y = DRAW_TOP_PAD + DRAW_HEIGHT + DRAW_XLABEL_BASELINE;
	draw_formula(x, y, p);
}
