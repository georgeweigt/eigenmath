function
emit_double(p)
{
	var i, j, k, s;

	if (p.d == 0) {
		emit_roman("0");
		return;
	}

	s = Math.abs(p.d).toPrecision(6) + 'T'; // terminator

	k = 0;

	while (isdigit(s[k]) || s[k] == '.')
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		emit_roman(s.substring(0, k));
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s[j] != '0')
				break;
		}
		emit_roman(s.substring(0, j + 1));
	}

	if (s[k] != 'E' && s[k] != 'e')
		return;

	k++;

	emit_infix_times();

	emit_roman("10");

	emit_superscript_begin();

	if (s[k] == '+')
		k++;
	else if (s[k] == '-') {
		k++;
		emit_minus_sign();
	}

	while (s[k] == '0')
		k++; // skip leading zeroes

	emit_roman(s.substring(k, s.length - 1));

	emit_superscript_end();
}
