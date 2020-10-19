function
emit_double(u, p, small_font) // p is a double
{
	var i, j, k, s, v;

	if (p.d == 0) {
		emit_roman_text(u, "0", small_font);
		return;
	}

	s = Math.abs(p.d).toPrecision(6) + '\0'; // terminator

	k = 0;

	while (isdigit(s[k]) || s[k] == '.')
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		emit_roman_text(u, s.substring(0, k), small_font);
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s[j] != '0')
				break;
		}
		emit_roman_text(u, s.substring(0, j + 1), small_font);
	}

	if (s[k] != 'E' && s[k] != 'e')
		return;

	k++;

	emit_infix_operator(u, "&times;", small_font);

	emit_roman_text(u, "10", small_font);

	v = {type:SUPERSCRIPT, a:[], small_font:small_font};

	if (s[k] == '+')
		k++;
	else if (s[k] == '-') {
		k++;
		emit_symbol_text(v, "&minus;", 1);
	}

	while (s[k] == '0')
		k++; // skip leading zeroes

	emit_roman_text(v, s.substring(k, s.length - 1), 1);

	emit_update(v);

	u.a.push(v);
}
