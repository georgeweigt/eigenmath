function
emit_fraction(u, p)
{
	var v = {type:FRACTION, num:{type:EXPR, a:[]}, den:{type:EXPR, a:[]}};

	if (isrational(p)) {
                emit_roman(v.num, Math.abs(p.a).toFixed(0));
                emit_roman(v.den, p.b.toFixed(0));
	} else if (car(p) == symbol(MULTIPLY)) {
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

	v.height = v.num.height + v.num.depth + FRAC_PAD + X_HEIGHT;
	v.depth = v.den.height + v.den.depth + FRAC_PAD - X_HEIGHT;

	v.width = Math.max(v.num.width, v.den.width);

	u.a.push(v);
}
