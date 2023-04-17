// automatic variables not visible to the garbage collector are reclaimed

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
	struct atom *p;
	eval_level++;
	p = pop();
	fpush(p); // make visible to garbage collector
	evalf_nib(p);
	fpop();
	eval_level--;
}

void
evalf_nib(struct atom *p1)
{
	if (interrupt)
		stopf("interrupt");

	if (eval_level == 200)
		stopf("circular definition?");

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
