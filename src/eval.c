// automatic struct atom pointers need to be visible to the garbage collector in order to be preserved

void
evalg(void)
{
	if (gc_level == eval_level && alloc_count > MAXBLOCKS * BLOCKSIZE / 10)
		gc();
	gc_level++;
	evalf();
	gc_level--;
}

// call evalf instead of evalg to evaluate without garbage collection

void
evalf(void)
{
	struct atom *p1;
	eval_level++;
	p1 = pop();
	fpush(p1); // make visible to garbage collector
	evalf_nib(p1);
	fpop();
	eval_level--;
}

void
evalf_nib(struct atom *p1)
{
	if (interrupt)
		kaput("interrupt");

	if (eval_level == 200)
		kaput("circular definition?");

	if (eval_level > max_eval_level)
		max_eval_level = eval_level;

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
		evalg();
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
		evalg();
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
	evalf();

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
