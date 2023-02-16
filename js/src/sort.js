function
sort(n)
{
	var t = stack.splice(stack.length - n).sort(cmp_expr);
	stack = stack.concat(t);
}
