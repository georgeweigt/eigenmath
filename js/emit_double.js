function
emit_double(u, p, small_font) // p is a double
{
	var h, i, j, k, s, v;

	if (p.d == 0) {
		emit_roman_text(u, "0", small_font);
		return;
	}

	s = Math.abs(p.d).toPrecision(6);

	k = 0;

	while (isdigit(s.charAt(k)) || s.charAt(k) == ".")
		k++;

	// handle trailing zeroes

	i = s.indexOf(".");

	if (i == -1)
		emit_roman_text(u, s.substring(0, k), small_font);
	else {
		for (j = k - 1; j > i + 1; j--) {
			if (s.charAt(j) != "0")
				break;
		}
		emit_roman_text(u, s.substring(0, j + 1), small_font);
	}

	if (s.charAt(k) != "E" && s.charAt(k) != "e")
		return;

	k++;

	emit_glyph(u, "times", small_font);

	emit_roman_text(u, "10", small_font);

	v = {type:SUPERSCRIPT, a:[], small_font:small_font};

	if (s.charAt(k) == "+")
		k++;
	else if (s.charAt(k) == "-") {
		k++;
		emit_glyph(v, "minus", 1);
		emit_thin_space(v, 1);
	}

	while (s.charAt(k) == "0")
		k++; // skip leading zeroes

	emit_roman_text(v, s.substring(k, s.length), 1);

	h = u.a[u.a.length - 1].height; // height of neighbor

	emit_update_superscript(v, h);

	u.a.push(v);
}
