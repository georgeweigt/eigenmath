function
evalf()
{
	var p1;

	if (++evaldepth == 1000)
		stopf("circular eval");

	p1 = pop();

	if (iscons(p1) && issymbol(car(p1)))
		car(p1).func(p1);
	else if (iskeyword(p1)) {
		// bare keyword
		push(p1);
		push_symbol(LAST);
		list(2);
		p1 = pop();
		car(p1).func(p1);
	} else if (issymbol(p1))
		eval_symbol(p1); // keyword already handled
	else if (istensor(p1))
		eval_tensor(p1);
	else
		push(p1);

	evaldepth--;
}
