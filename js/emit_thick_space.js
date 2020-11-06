function
emit_thick_space(u)
{
	var size, v, w;

	if (u.level == 0)
		size = FONT_SIZE;
	else
		size = SMALL_FONT_SIZE;

	w = roman_width["n".charCodeAt(0)] * WIDTH_RATIO * size;

	v = {type:SPACE, height:0, depth:0, width:w, level:u.level};

	u.a.push(v);
}
