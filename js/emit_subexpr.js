function
emit_subexpr(p)
{
	emit_paren_begin();
	emit_expr(p);
	emit_paren_end();
}
