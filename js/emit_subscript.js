function
emit_subscript(u, s)
{
	var v = {type:SUBSCRIPT, a:[]};

	if (s.length == 1)
		emit_italic(v, s, 1);
	else
		emit_special_symbol(v, s, 1);

	emit_update(v);

	v.height -= SMALL_X_HEIGHT;
	v.depth += SMALL_X_HEIGHT;

	u.a.push(v);
}
