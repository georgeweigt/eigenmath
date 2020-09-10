function
eval_setq(p1)
{
	var p2;

	push_symbol(NIL);

	if (caadr(p1) == symbol(INDEX)) {
		setq_indexed();
		return;
	}

	if (iscons(cadr(p1))) {
		setq_userfunc();
		return;
	}

	if (!isusersymbol(cadr(p1)))
		stop("symbol expected");

	push(caddr(p1));
	evalf();
	p2 = pop();
	set_binding(cadr(p1), p2);
}
