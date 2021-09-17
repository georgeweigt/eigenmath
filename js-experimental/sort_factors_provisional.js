function
sort_factors_provisional(h)
{
	var t = stack.splice(h).sort(cmp_factors_provisional);
	stack = stack.concat(t);
}
