function
emit_double(u, p) // p is a double
{
	var i, j, k, s, v;

	if (p.d == 0) {
		emit_roman_text(u, "0");
		return;
	}

	s = Math.abs(p.d).toPrecision(6);

	k = 0;

	while (isdigit(s.charAt(k)) || s.charAt(k) == ".")
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		emit_roman_text(u, s.substring(0, k));
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s.charAt(j) != "0")
				break;
		}
		emit_roman_text(u, s.substring(0, j + 1));
	}

	if (s.charAt(k) != "E" && s.charAt(k) != "e")
		return;

	k++;

	emit_named_glyph(u, "times");

	emit_roman_text(u, "10");

	v = {type:SUPERSCRIPT, a:[], level:u.level + 1};

	// sign of exponent

	if (s.charAt(k) == "+")
		k++;
	else if (s.charAt(k) == "-") {
		emit_named_glyph(v, "minus");
		emit_thin_space(v);
		k++;
	}

	// skip leading zeroes in exponent

	while (k < s.length - 1 && s.charAt(k) == "0")
		k++;

	emit_roman_text(v, s.substring(k, s.length));

	emit_update_superscript(u, v);

	u.a.push(v);
}
