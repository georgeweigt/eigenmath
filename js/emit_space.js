function
emit_space(u, small_font)
{
	var v = {type:SPACE, height:0, depth:0};

	if (small_font)
		v.width = SMALL_FONT_WIDTH;
	else
		v.width = FONT_WIDTH;

	u.a.push(v);
}
