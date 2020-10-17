function
emit_fraction(u, p)
{
	var v = {type:FRACTION, num:{type:EXPR, a:[]}, den:{type:EXPR, a:[]}};

	if (car(p) == symbol(MULTIPLY)) {
		emit_numerators(v.num, p);
		emit_denominators(v.den, p);
	} else if (car(p) == symbol(POWER)) {
		push(p);
		reciprocate();
		p = pop();
		emit_roman(v.num, "1");
		emit_expr(v.den, p);
	} else {
		emit_roman(v.num, "?");
		emit_roman(v.den, "?");
	}

	emit_update_fraction(v);

	u.a.push(v);
}
