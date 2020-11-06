function
emit_factor(u, p)
{
	if (isrational(p)) {
		emit_rational(u, p);
		return;
	}

	if (isdouble(p)) {
		emit_double(u, p);
		return;
	}

	if (issymbol(p)) {
		emit_symbol(u, p);
		return;
	}

	if (isstring(p)) {
		emit_string(u, p);
		return;
	}

	if (istensor(p)) {
		emit_tensor(u, p);
		return;
	}

	if (iscons(p)) {
		if (car(p) == symbol(POWER))
			emit_power(u, p);
		else if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY))
			emit_subexpr(u, p);
		else
			emit_function(u, p);
		return;
	}
}
