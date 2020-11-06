function
emit_subexpr(u, p)
{
	var v = {type:SUBEXPR, a:[], level:u.level};
	emit_expr(v, p);
	emit_update_subexpr(v);
	u.a.push(v);
}
