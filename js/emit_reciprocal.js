function
emit_reciprocal(u, p, small_font)
{
	var num, den, v;

	push(p);
	reciprocate();
	p = pop();

	emit_roman_text(u, "1", small_font);
	num = u.a.pop();

	den = emit_new(p, small_font);

	v = {type:FRACTION, num:num, den:den, small_font:small_font};

	emit_update_fraction(v);

	u.a.push(v);
}
