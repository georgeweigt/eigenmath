function
sort_expr(h)
{
	var t = stack.splice(h).sort(cmp_expr);
	stack = stack.concat(t);
}
