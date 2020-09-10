function
subtract()
{
	if (isnum(stack[stack.length - 2]) && isnum(stack[stack.length - 1])) {
		var p2 = pop();
		var p1 = pop();
		subtract_numbers(p1, p2);
		return;
	}

	negate();
	add();
}
