function
emit_exponent(u, p)
{
	var v;

	if (isnum(p) && !isnegativenumber(p)) {
		emit_numeric_exponent(u, p); // sign is not emitted
		return;
	}

	v = {type:SUPERSCRIPT, a:[], level:u.level + 1};

	emit_expr(v, p);

	emit_update_superscript(u, v);

	u.a.push(v);
}
