function
evalf()
{
	eval_level++;
	eval_nib();
	eval_level--;
}

function
eval_nib()
{
	var p1;

	if (eval_level > 1000)
		stopf("evaluation depth exceeded, possibly due to recursive function or circular symbol definition");

	p1 = pop();

	if (iscons(p1) && iskeyword(car(p1))) {
		expanding++;
		car(p1).func(p1);
		expanding--;
		return;
	}

	if (iscons(p1) && isusersymbol(car(p1))) {
		eval_user_function(p1);
		return;
	}

	if (iskeyword(p1)) { // bare keyword
		push(p1);
		push_symbol(LAST); // default arg
		list(2);
		evalf();
		return;
	}

	if (isusersymbol(p1)) {
		eval_user_symbol(p1);
		return;
	}

	if (istensor(p1)) {
		eval_tensor(p1);
		return;
	}

	push(p1); // rational, double, or string
}

// evaluate '=' as '=='

function
evalp()
{
	var p1 = pop();
	if (car(p1) == symbol(SETQ)) {
		push_symbol(TESTEQ);
		push(cadr(p1));
		push(caddr(p1));
		list(3);
		p1 = pop();
	}
	push(p1);
	evalf();
}
