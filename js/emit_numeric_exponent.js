function
emit_numeric_exponent(u, p, small_font) // p is rational or double, sign is not emitted
{
	var v = {type:SUPERSCRIPT, a:[], small_font:small_font};

	if (isrational(p)) {
		emit_rational(v, p, 1);
//		emit_roman_text(v, Math.abs(p.a).toFixed(0), 1);
//		if (p.b != 1) {
//			emit_roman_text(v, "/", 1);
//			emit_roman_text(v, p.b.toFixed(0), 1);
//		}
	} else
		emit_double(v, p, 1);

	emit_update_superscript(v, u.a[u.a.length - 1].height);

	u.a.push(v);
}
