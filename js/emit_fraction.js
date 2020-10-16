function
emit_fraction(u, p)
{
	var v = {type:FRACTION, a:[]};

	if (isrational(p)) {
                emit_roman(v, Math.abs(p.a).toFixed(0));
                emit_roman(v, p.b.toFixed(0));
	} else if (car(p) == symbol(MULTIPLY)) {
		emit_numerators(v, p);
		emit_denominators(v, p);
	} else if (car(p) == symbol(POWER)) {
		push(p);
		reciprocate();
		p = pop();
		emit_roman(v, "1");
		v.a.push(emit_main(p));
	} else {
		emit_roman(v, "?");
		emit_roman(v, "?");
	}

	v.height = v.a[0].height + v.a[1].height + FRACTION_HEIGHT;
	v.width = Math.max(v.a[0].width, v.a[1].width);

	u.a.push(v);
}
