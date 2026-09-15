function
evalf()
{
	var p = pop();
	if (iskeyword(p)) {
		push(p);
		push_symbol(LAST); // default arg
		list(2);
		p = pop();
	}
	eval_level++;
	eval_nib(p);
	eval_level--;
}

function
eval_nib(p)
{
	if (eval_level > 1000)
		stopf("evaluation depth exceeded, possibly due to recursive function or circular symbol definition");

	if (iscons(p) && iskeyword(car(p))) {
		expanding++;
		car(p).func(p);
		expanding--;
		return;
	}

	if (iscons(p) && isusersymbol(car(p))) {
		eval_user_function(p);
		return;
	}

	if (isusersymbol(p)) {
		eval_user_symbol(p);
		return;
	}

	if (istensor(p)) {
		eval_tensor(p);
		return;
	}

	push(p); // rational, double, or string
}

// evaluate '=' as '=='

function
evalp()
{
	var p = pop();
	if (car(p) == symbol(SETQ)) {
		push_symbol(TESTEQ);
		push(cadr(p));
		push(caddr(p));
		list(3);
		p = pop();
	}
	push(p);
	evalf();
}
