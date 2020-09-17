function
divide()
{
	var p1, p2;

	p2 = pop(); // divisor
	p1 = pop();

	if (isnum(p1) && isnum(p2) {
		divide_numbers(p1, p2);
		return;
	}

	push(p1);
	push(p2);
	reciprocate();
	multiply();
}
