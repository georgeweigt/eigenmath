function
emit_thin_space(u, small_font)
{
	var v = {type:SPACE, height:0, depth:0};

	if (small_font)
		v.width = SMALL_THIN_SPACE_WIDTH;
	else
		v.width = THIN_SPACE_WIDTH;

	u.a.push(v);
}
