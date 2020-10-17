function
emit_subscript(u, s)
{
	var v = {type:SUBSCRIPT, a:[], small_font:1};
	if (s.length == 1)
		emit_italic(v, s);
	else
		emit_special_symbol(v, s);
	emit_update(v);
	u.a.push(v);
}
