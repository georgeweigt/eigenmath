function
simplify_pass2()
{
	var p1, p2;

	p1 = pop();

	push(p1);
	circexp();
	rationalize();
	evalf(); // to normalize
	p2 = pop();

	if (complexity(p2) < complexity(p1))
		p1 = p2;

	push(p1);
}
