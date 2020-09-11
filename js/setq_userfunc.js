function
setq_userfunc(p1)
{
	var A, B, F;

	F = caadr(p1);
	A = cdadr(p1);
	B = caddr(p1);

	if (!issymbol(F))
		stop("function name?");

	set_binding_and_arglist(F, B, A);
}
