void
eval(void)
{
	if (interrupt)
		kaput("interrupt");

	level++;

	if (level > max_level)
		max_level = level;

	if (level == 200)
		kaput("circular definition?");

	eval_nib();

	level--;
}

void
eval_nib(void)
{
	struct atom *p1;

	p1 = pop();

	if (iscons(p1) && iskeyword(car(p1))) {
		expanding++;
		car(p1)->u.ksym.func(p1); // call through function pointer
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
		p1 = pop();
		expanding++;
		car(p1)->u.ksym.func(p1); // call through function pointer
		expanding--;
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

void
eval_user_symbol(struct atom *p1)
{
	struct atom *p2;

	p2 = get_binding(p1);

	if (p1 == p2 || p2 == symbol(NIL))
		push(p1); // symbol evaluates to itself
	else {
		push(p2); // eval symbol binding
		eval();
	}
}

void
eval_nil(struct atom *p1)
{
	(void) p1; // silence compiler
	push_symbol(NIL);
}

void
eval_number(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = pop();

	if (isnum(p1))
		push_integer(1);
	else
		push_integer(0);
}

void
eval_stop(struct atom *p1)
{
	(void) p1; // silence compiler
	stopf("stop function");
}
