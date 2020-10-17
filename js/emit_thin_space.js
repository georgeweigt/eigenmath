function
emit_thin_space(u)
{
	var v = {type:SPACE, height:0, depth:0, small_font:u.small_font};

	if (u.small_font)
		v.width = SMALL_THIN_SPACE_WIDTH;
	else
		v.width = THIN_SPACE_WIDTH;

	u.a.push(v);
}
