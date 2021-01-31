function
evalf()
{
	if (++evaldepth == 1000)
		stopf("evaluation depth");

	evalf_nib();

	--evaldepth;
}

function
evalf_nib()
{
	var p1;

	p1 = pop();

	if (iscons(p1) && iskeyword(car(p1))) {
		car(p1).func(p1);
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
		p1 = pop();
		car(p1).func(p1);
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
