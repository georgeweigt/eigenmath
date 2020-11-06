function
emit_text(u, s, italic_font)
{
	var i, n, v;
	n = s.length;
	for (i = 0; i < n; i++) {
		v = {type:TEXT, s:s[i], small_font:u.small_font, italic_font:italic_font};
		emit_update_text(v);
		u.a.push(v);
	}
}
