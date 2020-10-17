function
emit_roman(u, s)
{
	var v = {type:ROMAN, s:s, small_font:u.small_font};

	if (u.small_font) {
		u.height = SMALL_FONT_HEIGHT;
		u.depth = SMALL_FONT_DEPTH;
		u.width = s.length * SMALL_FONT_WIDTH;
	} else {
		u.height = FONT_HEIGHT;
		u.depth = FONT_DEPTH;
		u.width = s.length * FONT_WIDTH;
	}

	u.a.push(v);
}
