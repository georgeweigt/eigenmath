#include "defs.h"

void
eval(void)
{
	save();

	p1 = pop();

	if (p1->k == CONS)
		eval_cons();
	else if (p1->k == KSYM)
		eval_ksym();
	else if (p1->k == USYM)
		eval_usym();
	else if (p1->k == TENSOR)
		eval_tensor();
	else
		push(p1); // rational, double, or string

	restore();
}

void
eval_cons(void)
{
	if (car(p1)->k == KSYM)
		car(p1)->u.ksym.func();
	else if (car(p1)->k == USYM)
		eval_user_function();
	else
		push(p1); // not evaluated
}

// bare keyword

void
eval_ksym(void)
{
	push(p1);
	push_symbol(LAST); // default arg
	list(2);
	eval();
}

// evaluate symbol's binding

void
eval_usym(void)
{
	p2 = get_binding(p1);
	push(p2);
	if (p1 != p2)
		eval();
}

void
eval_binding(void)
{
	push(get_binding(cadr(p1)));
}

void
eval_clear(void)
{
	clear_flag = 1;
	push_symbol(NIL);
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
	int t = expanding;
	expanding = 1;
	eval();
	expanding = t;
}
