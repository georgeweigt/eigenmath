function
setq_usrfunc(p1)
{
	var A, B, C, F;

	F = caadr(p1); // function name
	A = cdadr(p1); // function args
	B = caddr(p1); // function body

	if (!isusersymbol(F))
		stopf("user symbol expected");

	if (lengthf(A) > 9)
		stopf("more than 9 arguments");

	push(B);
	convert_body(A);
	C = pop();

	set_symbol(F, B, C);
}
