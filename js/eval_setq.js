function
eval_setq(p1)
{
	var p2;

	push_symbol(NIL); // result

	if (caadr(p1) == symbol(INDEX)) {
		setq_indexed(p1);
		return;
	}

	if (iscons(cadr(p1))) {
		setq_userfunc(p1);
		return;
	}

	if (!isusersymbol(cadr(p1)))
		stopf("symbol expected");

	push(caddr(p1));
	evalf();
	p2 = pop();
	set_binding(cadr(p1), p2);
}
