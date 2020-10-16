function
emit_arglist(u, p)
{
	var v = {type:PAREN, a:[]};
	p = cdr(p);
	if (iscons(p)) {
		emit_expr(u, car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_roman(u, ",");
			emit_thin_space(u);
			emit_expr(u, car(p));
			p = cdr(p);
		}
	}
	emit_update(v);
	v.width += 2 * FONT_WIDTH; // for parens
	u.a.push(v);
}
