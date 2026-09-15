// call evalf instead of evalg to evaluate without garbage collection

void
evalf(void)
{
	fcount++;
	evalg();
	fcount--;
}

// all automatic variables must be visible to the garbage collector

// otherwise, use evalf

void
evalg(void)
{
	struct atom *p;
	if (fcount == 0 && alloc_count > MAXBLOCKS * BLOCKSIZE / 10)
		gc();
	p = pop();
	if (iskeyword(p)) {
		push(p);
		push_symbol(LAST); // default arg
		list(2);
		p = pop();
	}
	push(p); // make visible to garbage collector
	eval_level++;
	eval_nib(p);
	eval_level--;
	p = pop();
	pop(); // remove
	push(p);
}

void
eval_nib(struct atom *p)
{
	if (interrupt)
		stopf("interrupt");

	// this is needed to prevent seg fault (STACKSIZE is greater than process stack)

	if (eval_level > 1000)
		stopf("evaluation depth exceeded, possibly due to recursive function or circular symbol definition");

	if (eval_level > max_eval_level)
		max_eval_level = eval_level;

	if (iscons(p) && iskeyword(car(p))) {
		expanding++; // in case we are in noexpand()
		car(p)->u.ksym.func(p); // call through function pointer
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

void
evalp(void)
{
	struct atom *p;
	p = pop();
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
