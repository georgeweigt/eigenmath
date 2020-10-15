function
emit_italic(s)
{
	var u = {type:ITALIC, s:s};
	u.height = FONT_HEIGHT;
	u.width = s.length * FONT_WIDTH;
	return u;
}
