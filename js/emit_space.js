function
emit_space(u, small_font)
{
	var v = {type:SPACE, height:0, depth:0};

	if (small_font)
		v.width = Math.round(WIDTH_RATIO * SMALL_FONT_SIZE);
	else
		v.width = Math.round(WIDTH_RATIO * FONT_SIZE);

	u.a.push(v);
}
