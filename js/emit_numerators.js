function
emit_numerators(u, p)
{
	var q, v;

	v = {type:LINE, a:[], level:u.level};

	p = cdr(p);
	q = car(p);

	while (iscons(p)) {

		q = car(p);

		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in denominator
		}

		if (v.a.length > 0)
			emit_medium_space(v);

		if (isrational(q)) {
			if (Math.abs(q.a) != 1)
				emit_roman_text(v, Math.abs(q.a).toFixed(0));
		} else if (car(q) == symbol(ADD))
			emit_subexpr(v, q);
		else
			emit_expr(v, q);

		p = cdr(p);
	}

	if (v.a.length == 0)
		emit_roman_text(v, "1"); // there were no numerators

	if (v.a.length == 1) {
		u.num = v.a[0];
		return;
	}

	emit_update(v);

	u.num = v;
}
