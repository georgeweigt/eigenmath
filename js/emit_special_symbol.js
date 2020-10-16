function
emit_special_symbol(u, s)
{
	var v;

	if (s[0] >= 'A' && s[0] <= 'Z')
		v = {type:ROMAN, s: "&" + s + ";", height:FONT_HEIGHT, depth:FONT_DEPTH, width:FONT_WIDTH};
	else
		v = {type:ITALIC, s: "&" + s + ";", height:FONT_HEIGHT, depth:FONT_DEPTH, width:FONT_WIDTH};

	u.a.push(v);
}
