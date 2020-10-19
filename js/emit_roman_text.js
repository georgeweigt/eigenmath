function
emit_roman_text(u, s, small_font)
{
	var c, i, n, v;

	n = s.length;

	for (i = 0; i < n; i++) {

		c = s[i];

		if (c == '&')
			c = "&amp;";
		else if (c == '<')
			c = "&lt;";
		else if (c == '>')
			c = "&gt";

		v = {type:ROMAN, s:c, small_font:small_font};

		if (small_font) {
			v.height = SMALL_FONT_HEIGHT;
			v.depth = SMALL_FONT_DEPTH;
			v.width = SMALL_FONT_WIDTH;
		} else {
			v.height = FONT_HEIGHT;
			v.depth = FONT_DEPTH;
			v.width = FONT_WIDTH;
		}

		u.a.push(v);
	}
}
