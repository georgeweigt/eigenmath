function
emit_subscript(u, s)
{
	var v = {type:SUBSCRIPT, a:[], small_font:1};

	if (s.length == 1)
		emit_italic(v, s, 1);
	else
		emit_special_symbol(v, s, 1);

	emit_update(v);

	if (u.small_font) {
		v.height -= SMALL_X_HEIGHT;
		v.depth += SMALL_X_HEIGHT;
	} else {
		v.height -= X_HEIGHT;
		v.depth += X_HEIGHT;
	}

	u.a.push(v);
}
