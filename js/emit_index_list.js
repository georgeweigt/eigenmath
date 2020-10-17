function
emit_index_list(u, p)
{
	var v = {type:BRACK, a:[]};
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
	v.width += 2 * FONT_WIDTH;
	u.a.push(v);
}
