function
setq_userfunc(p1)
{
	var h, p2, A, B, F, T;

	F = caadr(p1); // function name
	A = cdadr(p1); // function arg list
	B = caddr(p1); // function body

	if (!isusersymbol(F))
		stopf("function definition error");

	// convert args

	h = stack.length;
	p1 = A;

	while (iscons(p1)) {
		p2 = car(p1);
		if (!isusersymbol(p2))
			stopf("function definition error");
		push(dual(p2));
		p1 = cdr(p1);
	}

	list(stack.length - h);
	T = pop();

	set_binding(F, B);
	set_arglist(F, T);

	// convert body

	push(B);

	p1 = A;
	p2 = T;

	while (iscons(p1)) {
		push(car(p1));
		push(car(p2));
		subst();
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	B = pop();
	set_binding(ddual(F), B);
}
