function
emit_text(u, s, small_font, italic_font)
{
	var c, i, n, v;

	n = s.length;

	for (i = 0; i < n; i++) {

		c = s[i];

		v = {type:TEXT, s:c, small_font:small_font, italic_font:italic_font};

		emit_update_text(v);

		if (c == '&')
			v.s = "&amp;";
		else if (c == '<')
			v.s = "&lt;";
		else if (c == '>')
			v.s = "&gt";

		u.a.push(v);
	}
}
