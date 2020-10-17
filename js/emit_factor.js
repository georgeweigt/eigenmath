function
emit_factor(u, p, small_font)
{
	if (isrational(p)) {
		emit_rational(u, p, small_font);
		return;
	}

	if (isdouble(p)) {
		emit_double(u, p, small_font);
		return;
	}

	if (issymbol(p)) {
		emit_symbol(u, p, small_font);
		return;
	}

	if (isstring(p)) {
		emit_string(u, p, small_font);
		return;
	}

	if (istensor(p)) {
		emit_tensor(u, p);
		return;
	}

	if (iscons(p)) {
		if (car(p) == symbol(POWER))
			emit_power(u, p, small_font);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			emit_subexpr(u, p, small_font);
		else
			emit_function(u, p, small_font);
		return;
	}
}
