function
emit_infix_operator(u, s)
{
	var v = {type:ROMAN, s:s, height:FONT_HEIGHT, depth:FONT_DEPTH, width:FONT_WIDTH};
	emit_thick_space(u);
	u.a.push(v);
	emit_thick_space(u);
}
