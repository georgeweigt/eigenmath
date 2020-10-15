function
emit_fraction(p)
{
	var u = {type:FRACTION};

	if (isrational(p)) {
                u.a = emit_roman(Math.abs(p.a).toFixed(0));
                u.b = emit_roman(p.b.toFixed(0));
	} else if (car(p) == symbol(MULTIPLY)) {
		u.a = emit_numerators(p);
		u.b = emit_denominators(p);
	} else if (car(p) == symbol(POWER)) {
		push(p);
		reciprocate();
		p = pop();
		u.a = emit_roman("1");
		u.b = emit_expr(p);
	} else {
		u.a = emit_roman("?");
		u.b = emit_roman("?");
	}

	u.width = Math.max(u.a.width, u.b.width);
	u.height = u.a.height + u.b.height + FRACTION_HEIGHT;

	return u;
}
