function
emit_reciprocal(u, p, small_font) // p = y^x, x is a negative number
{
	var num, den, v;

	num = {a:[]};
	emit_roman_text(num, "1", small_font);
	num = num.a[0];

	if (isminusone(caddr(p)))
		den = emit_line(cadr(p), small_font);
	else {
		den = {type:LINE, a:[]};
		emit_base(den, cadr(p), small_font);
		emit_exponent(den, caddr(p));
		emit_update(den);
	}

	v = {type:FRACTION, num:num, den:den, small_font:small_font};

	emit_update_fraction(v);

	u.a.push(v);
}
