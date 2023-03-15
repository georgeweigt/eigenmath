// like eval() except '=' is evaluated as '=='

void
evalp(void)
{
	struct atom *p1;
	p1 = pop();
	if (car(p1) == symbol(SETQ))
		eval_testeq(p1);
	else {
		push(p1);
		eval();
	}
}
