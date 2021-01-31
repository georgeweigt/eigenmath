#include "defs.h"

void
eval(void)
{
	save();
	eval_nib();
	restore();
}

void
eval_nib(void)
{
	p1 = pop();

	if (iscons(p1) && iskeyword(car(p1))) {
		car(p1)->u.ksym.func(); // call through function pointer
		return;
	}

	if (iscons(p1) && isusersymbol(car(p1))) {
		eval_user_function();
		return;
	}

	if (iskeyword(p1)) {
		eval_keyword();
		return;
	}

	if (isusersymbol(p1)) {
		eval_user_symbol();
		return;
	}

	if (istensor(p1)) {
		eval_tensor();
		return;
	}

	push(p1); // rational, double, or string
}

// bare keyword

void
eval_keyword(void)
{
	push(p1);
	push_symbol(LAST); // default arg
	list(2);
	p1 = pop();
	car(p1)->u.ksym.func(); // call through function pointer
}

// evaluate symbol's binding

void
eval_user_symbol(void)
{
	p2 = get_binding(p1);

	if (p1 == p2 || p2 == symbol(NIL))
		push(p1);
	else {
		push(p2);
		eval();
	}
}

void
eval_binding(void)
{
	p1 = cadr(p1);
	p2 = get_binding(p1);
	if (p2 == symbol(NIL))
		p2 = p1;
	push(p2);
}

void
eval_clear(void)
{
	save_binding(symbol(TRACE));
	save_binding(symbol(TTY));

	clear_symbols();

	run_init_script();

	gc(); // garbage collection

	restore_binding(symbol(TTY));
	restore_binding(symbol(TRACE));

	push_symbol(NIL); // result
}

void
eval_do(void)
{
	push_integer(0);
	p1 = cdr(p1);
	while (iscons(p1)) {
		pop();
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}
}

// for example, eval(f,x,2)

void
eval_eval(void)
{
	push(cadr(p1));
	eval();
	p1 = cddr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		push(cadr(p1));
		eval();
		subst();
		p1 = cddr(p1);
	}
	eval();
}

void
eval_nil(void)
{
	push_symbol(NIL);
}

void
eval_number(void)
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
eval_quote(void)
{
	push(cadr(p1));
}

void
eval_sqrt(void)
{
	push(cadr(p1));
	eval();
	push_rational(1, 2);
	power();
}

void
eval_stop(void)
{
	stop("stop function");
}

void
eval_subst(void)
{
	push(cadddr(p1));
	eval();
	push(caddr(p1));
	eval();
	push(cadr(p1));
	eval();
	subst();
	eval(); // normalize
}

void
expand_expr(void)
{
	expanding++;
	eval();
	expanding--;
}
