function
emit_reciprocal(u, p, small_font) // p = y^x and x is a negative number
{
	var num, den, v;

	num = {type:LINE, a:[]};
	den = {type:LINE, a:[]};

	emit_roman_text(num, "1", small_font);

	if (isminusone(caddr(p)))
		emit_expr(den, cadr(p), small_font);
	else {
		emit_base(den, cadr(p), small_font);
		emit_exponent(den, caddr(p));
	}

	emit_update(num);
	emit_update(den);

	if (num.a.length == 1)
		num = num.a[0];

	if (den.a.length == 1)
		den = den.a[0];

	v = {type:FRACTION, num:num, den:den, small_font:small_font};

	emit_update_fraction(v);

	u.a.push(v);
}
