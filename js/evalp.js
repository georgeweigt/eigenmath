function
evalp()
{
	var p1 = pop();
	if (car(p1) == symbol(SETQ))
		eval_testeq(p1);
	else {
		push(p1);
		evalf();
	}
}
