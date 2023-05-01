function
sort(n)
{
	var t = stack.splice(stack.length - n).sort(cmp);
	stack = stack.concat(t);
}
