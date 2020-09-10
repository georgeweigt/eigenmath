function
pop()
{
	if (stack.length == 0)
		stop("stack error");
	return stack.pop();
}
