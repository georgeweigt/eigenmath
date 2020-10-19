function
emit_reciprocal(u, p, small_font) // p = y^x where x is a negative number
{
	var num, den, v;

	num = {type:LINE, a:[]};
	den = {type:LINE, a:[]};

	emit_roman_text(num, "1", small_font);

	if (isminusone(caddr(p)))
		emit_expr(den, cadr(p), small_font);
	else {
		emit_base(den, cadr(p), small_font);
		emit_numeric_exponent(den, caddr(p)); // sign is not emitted
	}

	if (num.a.length == 1)
		num = num.a[0];
	else
		emit_update(num);

	if (den.a.length == 1)
		den = den.a[0];
	else
		emit_update(den);

	v = {type:FRACTION, num:num, den:den, small_font:small_font};

	emit_update_fraction(v);

	u.a.push(v);
}
