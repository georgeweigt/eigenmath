function
setq_usrfunc(p1)
{
	var A, B, C, F;

	F = caadr(p1); // function name
	A = cdadr(p1); // function args
	B = caddr(p1); // function body

	if (find_func_defn(B))
		stopf("func defn in func");

	if (!isusersymbol(F))
		stopf("user symbol expected");

	if (lengthf(A) > 9)
		stopf("more than 9 arguments");

	push(B);
	convert_body(A);
	C = pop();

	set_symbol(F, B, C);
}

function
convert_body(A)
{
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG1);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG2);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG3);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG4);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG5);
	subst();

	A = cdr(A);

	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG6);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG7);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG8);
	subst();

	A = cdr(A);
	if (!iscons(A))
		return;

	push(car(A));
	push_symbol(ARG9);
	subst();
}

function
find_func_defn(p1)
{
	if (!iscons(p1))
		return 0;

	if (car(p1) == symbol(SETQ) && caadr(p1) == symbol(INDEX))
		return 0; // component access

	if (car(p1) == symbol(SETQ) && iscons(cadr(p1)))
		return 1; // func defn

	while (iscons(p1)) {
		if (find_func_defn(car(p1)))
			return 1;
		p1 = cdr(p1);
	}

	return 0;
}
