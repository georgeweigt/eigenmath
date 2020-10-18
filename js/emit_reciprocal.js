function
emit_reciprocal(u, p, small_font)
{
	var num, den, v;

	push(p);
	reciprocate();
	p = pop();

	emit_roman_text(u, "1", small_font);
	num = u.a.pop();

	den = {type:EXPR, a:[]};

	emit_expr(den, p, small_font);

	if (den.a.length == 1)
		den = den.a[0];
	else
		emit_update(den);

	v = {type:FRACTION, num:num, den:den, small_font:small_font};

	emit_update_fraction(v);

	u.a.push(v);
}
