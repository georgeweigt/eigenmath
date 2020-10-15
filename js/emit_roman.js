function
emit_roman(s)
{
	var u = {type:ROMAN, s:s};
	u.height = FONT_HEIGHT;
	u.width = s.length * FONT_WIDTH;
	return u;
}
