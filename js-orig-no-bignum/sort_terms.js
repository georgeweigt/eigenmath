function
sort_terms(h)
{
	var t = stack.splice(h).sort(cmp_terms);
	stack = stack.concat(t);
}
