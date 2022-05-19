// try rectangular form

function
simplify_pass3()
{
	var p1, p2;

	p1 = pop();

	// already simple?

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	if (car(p1) != symbol(ADD) || !findf(p1, imaginaryunit)) {
		push(p1);
		return;
	}

	push(p1);
	rect();
	p2 = pop();

	if (!iscons(p2)) {
		push(p2);
		return;
	}

	push(p1);
}
