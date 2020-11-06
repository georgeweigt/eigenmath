function
emit_numeric_exponent(u, p) // p is rational or double, sign is not emitted
{
	var v = {type:SUPERSCRIPT, a:[], small_font:1};

	if (isrational(p)) {
		emit_roman_text(v, Math.abs(p.a).toFixed(0));
		if (p.b != 1) {
			emit_roman_text(v, "/");
			emit_roman_text(v, p.b.toFixed(0));
		}
	} else
		emit_double(v, p);

	emit_update_superscript(u, v);

	u.a.push(v);
}
