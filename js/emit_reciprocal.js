function
emit_reciprocal(u, p) // p = y^x where x is a negative number
{
	var num, den, v;

	num = {type:LINE, a:[], small_font:u.small_font};
	den = {type:LINE, a:[], small_font:u.small_font};

	emit_roman_text(num, "1");

	if (isminusone(caddr(p)))
		emit_expr(den, cadr(p));
	else {
		emit_base(den, cadr(p));
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

	v = {type:FRACTION, num:num, den:den, small_font:u.small_font};

	emit_update_fraction(v);

	u.a.push(v);
}
