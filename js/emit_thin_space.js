function
emit_thin_space(u, small_font)
{
	var size, v, w;

	if (small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	w = 0.25 * roman_width["n".charCodeAt(0)] * WIDTH_RATIO * size;

	v = {type:SPACE, height:0, depth:0, width:w};

	u.a.push(v);
}
