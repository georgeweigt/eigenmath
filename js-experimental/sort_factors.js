function
sort_factors(h)
{
	var t = stack.splice(h).sort(cmp_factors);
	stack = stack.concat(t);
}
