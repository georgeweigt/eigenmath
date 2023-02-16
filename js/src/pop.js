function
pop()
{
	if (stack.length == 0)
		stopf("stack error");
	return stack.pop();
}
