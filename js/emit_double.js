function
emit_double(u, p)
{
	var i, j, k, s, v;

	if (p.d == 0) {
		emit_roman(u, "0");
		return;
	}

	s = Math.abs(p.d).toPrecision(6) + 'T'; // terminator

	k = 0;

	while (isdigit(s[k]) || s[k] == '.')
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		emit_roman(u, s.substring(0, k));
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s[j] != '0')
				break;
		}
		emit_roman(u, s.substring(0, j + 1));
	}

	if (s[k] != 'E' && s[k] != 'e')
		return;

	k++;

	emit_infix_operator(u, "&times;");

	emit_roman(u, "10");

	v = {type:SUPERSCRIPT, a:[]};

	if (s[k] == '+')
		k++;
	else if (s[k] == '-') {
		k++;
		emit_minus_sign(v, u);
	}

	while (s[k] == '0')
		k++; // skip leading zeroes

	emit_roman(v, s.substring(k, s.length - 1));

	emit_update(v);

	u.a.push(v);
}
