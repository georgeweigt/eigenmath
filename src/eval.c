#include "defs.h"

void
eval_and_print_result(int update)
{
	save();
	eval_and_print_result_nib(update);
	restore();
}

void
eval_and_print_result_nib(int update)
{
	p1 = pop();
	push(p1);
	eval();
	p2 = pop();

	// "draw", "for" and "setq" return "nil", there is no result to print

	if (p2 == symbol(NIL))
		return;

	if (update)
		binding[LAST] = p2;

	if (issymbol(p1) && !iskeyword(p1) && p1 != p2) // keyword like "float"
		prep_symbol_equals();

	if (iszero(binding[TTY])) {
		push(p2);
		cmdisplay();
	} else
		print(p2);
}

void
prep_symbol_equals(void)
{
	if (p1 == symbol(SYMBOL_I) && isimaginaryunit(p2))
		return;

	if (p1 == symbol(SYMBOL_J) && isimaginaryunit(p2))
		return;

	push_symbol(SETQ);
	push(p1);
	push(p2);
	list(3);
	p2 = pop();
}

void
eval(void)
{
	save();

	if (stop_flag)
		stop(NULL);

	p1 = pop();

	if (p1->k == CONS)
		eval_cons();
	else if (p1->k == SYM)
		eval_sym();
	else if (p1->k == TENSOR)
		eval_tensor();
	else
		push(p1); // rational, double, or string

	restore();
}

void
eval_sym(void)
{
	// bare keyword?

	if (iskeyword(p1)) {
		push(p1);
		push_symbol(LAST); // default arg
		list(2);
		eval();
		return;
	}

	// evaluate symbol's binding

	p2 = get_binding(p1);
	push(p2);
	if (p1 != p2)
		eval();
}

void (*functab[])(void) = {
	eval_abs,
	eval_add,
	eval_adj,
	eval_and,
	eval_arccos,
	eval_arccosh,
	eval_arcsin,
	eval_arcsinh,
	eval_arctan,
	eval_arctanh,
	eval_arg,
	eval_atomize,
	eval_besselj,
	eval_bessely,
	eval_binding,
	eval_binomial,
	eval_ceiling,
	eval_check,
	eval_choose,
	eval_circexp,
	eval_clear,
	eval_clock,
	eval_coeff,
	eval_cofactor,
	eval_conj,
	eval_contract,
	eval_cos,
	eval_cosh,
	eval_defint,
	eval_degree,
	eval_denominator,
	eval_derivative,
	eval_det,
	eval_dim,
	eval_divisors,
	eval_do,
	eval_dot,
	eval_draw,
	eval_eigen,
	eval_eigenval,
	eval_eigenvec,
	eval_erf,
	eval_erfc,
	eval_eval,
	eval_exit,
	eval_exp,
	eval_expand,
	eval_expcos,
	eval_expcosh,
	eval_expsin,
	eval_expsinh,
	eval_exptan,
	eval_exptanh,
	eval_factor,
	eval_factorial,
	eval_filter,
	eval_float,
	eval_floor,
	eval_for,
	eval_gcd,
	eval_hermite,
	eval_hilbert,
	eval_imag,
	eval_index,
	eval_inner,
	eval_integral,
	eval_inv,
	eval_isprime,
	eval_laguerre,
	eval_latex,
	eval_lcm,
	eval_leading,
	eval_legendre,
	eval_lisp,
	eval_log,
	eval_mag,
	eval_mathml,
	eval_mod,
	eval_multiply,
	eval_not,
	eval_nroots,
	eval_number,
	eval_numerator,
	eval_or,
	eval_outer,
	eval_polar,
	eval_power,
	eval_prime,
	eval_print,
	eval_product,
	eval_quote,
	eval_quotient,
	eval_rank,
	eval_rationalize,
	eval_real,
	eval_rect,
	eval_roots,
	eval_run,
	eval_setq,
	eval_sgn,
	eval_simplify,
	eval_sin,
	eval_sinh,
	eval_sqrt,
	eval_status,
	eval_stop,
	eval_string,
	eval_subst,
	eval_sum,
	eval_tan,
	eval_tanh,
	eval_taylor,
	eval_test,
	eval_testeq,
	eval_testge,
	eval_testgt,
	eval_testle,
	eval_testlt,
	eval_transpose,
	eval_unit,
	eval_zero,
};

void
eval_cons(void)
{
	int k;

	if (car(p1)->k != SYM)
		stop("eval_cons: function name is not a symbol");

	k = symnum(car(p1));

	if (k < MARK1)
		functab[k]();
	else
		eval_user_function();
}

void
eval_binding(void)
{
	push(get_binding(cadr(p1)));
}

void
eval_check(void)
{
	push(cadr(p1));
	evalp();
	p1 = pop();
	if (iszero(p1))
		stop("check");
	push_symbol(NIL); // no result is printed
}

void
eval_clear(void)
{
	clear_flag = 1;
	push_symbol(NIL);
}

void
eval_dim(void)
{
	int n;
	push(cadr(p1));
	eval();
	p2 = pop();
	if (iscons(cddr(p1))) {
		push(caddr(p1));
		eval();
		n = pop_integer();
	} else
		n = 1;
	if (!istensor(p2))
		push_integer(1); // dim of scalar is 1
	else if (n < 1 || n > p2->u.tensor->ndim)
		push(p1);
	else
		push_integer(p2->u.tensor->dim[n - 1]);
}

void
eval_divisors(void)
{
	push(cadr(p1));
	eval();
	divisors();
}

void
eval_do(void)
{
	push(car(p1));
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
	if (p1->k == RATIONAL || p1->k == DOUBLE)
		push_integer(1);
	else
		push_integer(0);
}

void
eval_print(void)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval_and_print_result(0);
		p1 = cdr(p1);
	}
	push_symbol(NIL);
}

void
eval_quote(void)
{
	push(cadr(p1));
}

void
eval_rank(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();
	if (istensor(p1))
		push_integer(p1->u.tensor->ndim);
	else
		push(zero);
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
eval_status(void)
{
	print_status();
	push_symbol(NIL);
}

void
eval_stop(void)
{
	stop(NULL);
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
eval_unit(void)
{
	int i, n;
	push(cadr(p1));
	eval();
	n = pop_integer();
	if (n < 2) {
		push(p1);
		return;
	}
	p1 = alloc_tensor(n * n);
	p1->u.tensor->ndim = 2;
	p1->u.tensor->dim[0] = n;
	p1->u.tensor->dim[1] = n;
	for (i = 0; i < n; i++)
		p1->u.tensor->elem[n * i + i] = one;
	push(p1);
}

// like eval() except "=" is evaluated as "=="

void
evalp(void)
{
	save();
	p1 = pop();
	if (car(p1) == symbol(SETQ))
		eval_testeq();
	else {
		push(p1);
		eval();
	}
	restore();
}

void
expand_expr(void)
{
	int t = expanding;
	expanding = 1;
	eval();
	expanding = t;
}
