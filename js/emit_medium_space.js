function
emit_medium_space(u, small_font)
{
	var v = {type:SPACE, height:0, depth:0};

	if (small_font)
		v.width = SMALL_MEDIUM_SPACE;
	else
		v.width = MEDIUM_SPACE;

	u.a.push(v);
}
