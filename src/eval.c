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

	// print string result in small font

	if (isstr(p2)) {
		printstr(p2->u.str);
		printstr("\n");
		return;
	}

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

// evaluate tos

void
eval(void)
{
	if (stop_flag)
		stop(NULL);
	save();
	p1 = pop();
	switch (p1->k) {
	case CONS:
		eval_cons();
		break;
	case RATIONAL:
		push(p1);
		break;
	case DOUBLE:
		push(p1);
		break;
	case STR:
		push(p1);
		break;
	case TENSOR:
		eval_tensor();
		break;
	case SYM:
		eval_sym();
		break;
	}
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

void
eval_cons(void)
{
	if (!issymbol(car(p1)))
		stop("cons?");

	switch (symnum(car(p1))) {
	case ABS:		eval_abs();		break;
	case ADD:		eval_add();		break;
	case ADJ:		eval_adj();		break;
	case AND:		eval_and();		break;
	case ARCCOS:		eval_arccos();		break;
	case ARCCOSH:		eval_arccosh();		break;
	case ARCSIN:		eval_arcsin();		break;
	case ARCSINH:		eval_arcsinh();		break;
	case ARCTAN:		eval_arctan();		break;
	case ARCTANH:		eval_arctanh();		break;
	case ARG:		eval_arg();		break;
	case ATOMIZE:		eval_atomize();		break;
	case BESSELJ:		eval_besselj();		break;
	case BESSELY:		eval_bessely();		break;
	case BINDING:		eval_binding();		break;
	case BINOMIAL:		eval_binomial();	break;
	case CEILING:		eval_ceiling();		break;
	case CHECK:		eval_check();		break;
	case CHOOSE:		eval_choose();		break;
	case CIRCEXP:		eval_circexp();		break;
	case CLEAR:		eval_clear();		break;
	case CLOCK:		eval_clock();		break;
	case COEFF:		eval_coeff();		break;
	case COFACTOR:		eval_cofactor();	break;
	case CONJ:		eval_conj();		break;
	case CONTRACT:		eval_contract();	break;
	case COS:		eval_cos();		break;
	case COSH:		eval_cosh();		break;
	case DEGREE:		eval_degree();		break;
	case DEFINT:		eval_defint();		break;
	case DENOMINATOR:	eval_denominator();	break;
	case DERIVATIVE:	eval_derivative();	break;
	case DET:		eval_det();		break;
	case DIM:		eval_dim();		break;
	case DIVISORS:		eval_divisors();	break;
	case DO:		eval_do();		break;
	case DOT:		eval_inner();		break;
	case DRAW:		eval_draw();		break;
	case EIGEN:		eval_eigen();		break;
	case EIGENVAL:		eval_eigenval();	break;
	case EIGENVEC:		eval_eigenvec();	break;
	case ERF:		eval_erf();		break;
	case ERFC:		eval_erfc();		break;
	case EVAL:		eval_eval();		break;
	case EXP:		eval_exp();		break;
	case EXPAND:		eval_expand();		break;
	case EXPCOS:		eval_expcos();		break;
	case EXPCOSH:		eval_expcosh();		break;
	case EXPSIN:		eval_expsin();		break;
	case EXPSINH:		eval_expsinh();		break;
	case EXPTAN:		eval_exptan();		break;
	case EXPTANH:		eval_exptanh();		break;
	case FACTOR:		eval_factor();		break;
	case FACTORIAL:		eval_factorial();	break;
	case FILTER:		eval_filter();		break;
	case FLOATF:		eval_float();		break;
	case FLOOR:		eval_floor();		break;
	case FOR:		eval_for();		break;
	case GCD:		eval_gcd();		break;
	case HERMITE:		eval_hermite();		break;
	case HILBERT:		eval_hilbert();		break;
	case IMAG:		eval_imag();		break;
	case INDEX:		eval_index();		break;
	case INNER:		eval_inner();		break;
	case INTEGRAL:		eval_integral();	break;
	case INV:		eval_inv();		break;
	case ISPRIME:		eval_isprime();		break;
	case LAGUERRE:		eval_laguerre();	break;
	case LCM:		eval_lcm();		break;
	case LEADING:		eval_leading();		break;
	case LEGENDRE:		eval_legendre();	break;
	case LISP:		eval_lisp();		break;
	case LOG:		eval_log();		break;
	case MAG:		eval_mag();		break;
	case MOD:		eval_mod();		break;
	case MULTIPLY:		eval_multiply();	break;
	case NOT:		eval_not();		break;
	case NROOTS:		eval_nroots();		break;
	case NUMBER:		eval_number();		break;
	case NUMERATOR:		eval_numerator();	break;
	case OR:		eval_or();		break;
	case OUTER:		eval_outer();		break;
	case POLAR:		eval_polar();		break;
	case POWER:		eval_power();		break;
	case PRIME:		eval_prime();		break;
	case PRINT:		eval_print();		break;
	case PRODUCT:		eval_product();		break;
	case QUOTE:		eval_quote();		break;
	case QUOTIENT:		eval_quotient();	break;
	case RANK:		eval_rank();		break;
	case RATIONALIZE:	eval_rationalize();	break;
	case REAL:		eval_real();		break;
	case RECTF:		eval_rect();		break;
	case ROOTS:		eval_roots();		break;
	case SETQ:		eval_setq();		break;
	case SGN:		eval_sgn();		break;
	case SIMPLIFY:		eval_simplify();	break;
	case SIN:		eval_sin();		break;
	case SINH:		eval_sinh();		break;
	case SQRT:		eval_sqrt();		break;
	case STATUS:		eval_status();		break;
	case STOP:		eval_stop();		break;
	case STRING:		eval_string();		break;
	case SUBST:		eval_subst();		break;
	case SUM:		eval_sum();		break;
	case TAN:		eval_tan();		break;
	case TANH:		eval_tanh();		break;
	case TAYLOR:		eval_taylor();		break;
	case TEST:		eval_test();		break;
	case TESTEQ:		eval_testeq();		break;
	case TESTGE:		eval_testge();		break;
	case TESTGT:		eval_testgt();		break;
	case TESTLE:		eval_testle();		break;
	case TESTLT:		eval_testlt();		break;
	case TRANSPOSE:		eval_transpose();	break;
	case UNIT:		eval_unit();		break;
	case ZERO:		eval_zero();		break;
	default:		eval_user_function();	break;
	}
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
