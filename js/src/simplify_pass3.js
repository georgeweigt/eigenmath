// try polar form

function
simplify_pass3()
{
	var p1, p2;

	p1 = pop();

	if (car(p1) != symbol(ADD) || isusersymbolsomewhere(p1) || !findf(p1, imaginaryunit)) {
		push(p1);
		return;
	}

	push(p1);
	polar();
	p2 = pop();

	if (!iscons(p2)) {
		push(p2);
		return;
	}

	push(p1);
}
