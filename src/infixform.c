#include "defs.h"

void
eval_infixform(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();

	outbuf_index = 0;
	infixform(p1);
	print_char('\0');

	push_string(outbuf);
}

void
print_infixform(struct atom *p)
{
	outbuf_index = 0;
	infixform(p);
	print_char('\n');
	print_char('\0');
	printbuf(outbuf, BLACK);
}

void
infixform(struct atom *p)
{
	if (car(p) == symbol(SETQ)) {
		infixform_expr(cadr(p));
		print_str(" = ");
		infixform_expr(caddr(p));
	} else
		infixform_expr(p);
}

void
infixform_subexpr(struct atom *p)
{
	print_char('(');
	infixform_expr(p);
	print_char(')');
}

void
infixform_expr(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		if (sign_of_term(car(p)) == '-')
			print_char('-');
		infixform_term(car(p));
		p = cdr(p);
		while (iscons(p)) {
			if (sign_of_term(car(p)) == '+')
				print_str(" + ");
			else
				print_str(" - ");
			infixform_term(car(p));
			p = cdr(p);
		}
	} else {
		if (sign_of_term(p) == '-')
			print_char('-');
		infixform_term(p);
	}
}

int
sign_of_term(struct atom *p)
{
	if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
		return '-';
	else
		return '+';
}

#undef A
#undef B

#define A p3
#define B p4

void
print_a_over_b(struct atom *p)
{
	int flag, n, d;
	struct atom *p1, *p2;

	save();

	// count numerators and denominators

	n = 0;
	d = 0;

	p1 = cdr(p);
	p2 = car(p1);

	if (isrational(p2)) {
		push(p2);
		numerator();
		absv();
		A = pop();
		push(p2);
		denominator();
		B = pop();
		if (!isplusone(A))
			n++;
		if (!isplusone(B))
			d++;
		p1 = cdr(p1);
	} else {
		A = one;
		B = one;
	}

	while (iscons(p1)) {
		p2 = car(p1);
		if (is_denominator(p2))
			d++;
		else
			n++;
		p1 = cdr(p1);
	}

	if (n == 0)
		print_char('1');
	else {
		flag = 0;
		p1 = cdr(p);
		if (isrational(car(p1)))
			p1 = cdr(p1);
		if (!isplusone(A)) {
			infixform_factor(A);
			flag = 1;
		}
		while (iscons(p1)) {
			p2 = car(p1);
			if (is_denominator(p2))
				;
			else {
				if (flag)
					infixform_multiply_sign();
				infixform_factor(p2);
				flag = 1;
			}
			p1 = cdr(p1);
		}
	}

	print_str(" / ");

	if (d > 1)
		print_char('(');

	flag = 0;
	p1 = cdr(p);

	if (isrational(car(p1)))
		p1 = cdr(p1);

	if (!isplusone(B)) {
		infixform_factor(B);
		flag = 1;
	}

	while (iscons(p1)) {
		p2 = car(p1);
		if (is_denominator(p2)) {
			if (flag)
				infixform_multiply_sign();
			infixform_denom(p2, d);
			flag = 1;
		}
		p1 = cdr(p1);
	}

	if (d > 1)
		print_char(')');

	restore();
}

void
infixform_term(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY) && any_denominators(p)) {
		print_a_over_b(p);
		return;
	}

	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);

		// coeff -1?

		if (isminusone(car(p)))
			p = cdr(p);

		infixform_factor(car(p));
		p = cdr(p);
		while (iscons(p)) {
			infixform_multiply_sign();
			infixform_factor(car(p));
			p = cdr(p);
		}
	} else
		infixform_factor(p);
}

// prints stuff after the divide symbol "/"

// d is the number of denominators

#undef BASE
#undef EXPO

#define BASE p1
#define EXPO p2

void
infixform_denom(struct atom *p, int d)
{
	save();

	BASE = cadr(p);
	EXPO = caddr(p);

	// i.e. 1 / (2^(1/3))

	if (d == 1 && !isminusone(EXPO))
		print_char('(');

	if (isfraction(BASE) || car(BASE) == symbol(ADD) || car(BASE) == symbol(MULTIPLY) || car(BASE) == symbol(POWER) || isnegativenumber(BASE)) {
			print_char('(');
			infixform_expr(BASE);
			print_char(')');
	} else
		infixform_expr(BASE);

	if (isminusone(EXPO)) {
		restore();
		return;
	}

	print_char('^');

	push(EXPO);
	negate();
	EXPO = pop();

	if (isfraction(EXPO) || car(EXPO) == symbol(ADD) || car(EXPO) == symbol(MULTIPLY) || car(EXPO) == symbol(POWER)) {
		print_char('(');
		infixform_expr(EXPO);
		print_char(')');
	} else
		infixform_expr(EXPO);

	if (d == 1)
		print_char(')');

	restore();
}

void
infixform_factor(struct atom *p)
{
	if (isnum(p)) {
		infixform_number(p);
		return;
	}

	if (isstr(p)) {
		print_str(p->u.str);
		return;
	}

	if (istensor(p)) {
		infixform_tensor(p);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		infixform_subexpr(p);
		return;
	}

	if (car(p) == symbol(POWER)) {

		if (isimaginaryunit(p)) {
			if (isimaginaryunit(get_binding(symbol(J_LOWER)))) {
				print_char('j');
				return;
			}
			if (isimaginaryunit(get_binding(symbol(I_LOWER)))) {
				print_char('i');
				return;
			}
		}

		if (cadr(p) == symbol(EXP1)) {
			print_str("exp(");
			infixform_expr(caddr(p));
			print_char(')');
			return;
		}

		if (isminusone(caddr(p))) {
			print_str("1 / ");
			if (iscons(cadr(p)))
				infixform_subexpr(cadr(p));
			else
				infixform_expr(cadr(p));
			return;
		}

		if (caadr(p) == symbol(ADD) || caadr(p) == symbol(MULTIPLY) || caadr(p) == symbol(POWER) || isnegativenumber(cadr(p)))
			infixform_subexpr(cadr(p));
		else if (isnum(cadr(p)) && (lessp(cadr(p), zero) || isfraction(cadr(p))))
			infixform_subexpr(cadr(p));
		else
			infixform_factor(cadr(p));

		print_char('^');

		if (iscons(caddr(p)) || isfraction(caddr(p)) || (isnum(caddr(p)) && lessp(caddr(p), zero)))
			infixform_subexpr(caddr(p));
		else
			infixform_factor(caddr(p));

		return;
	}

	if (car(p) == symbol(INDEX) && issymbol(cadr(p))) {
		print_index_function(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		infixform_factorial_function(p);
		return;
	}

	if (iscons(p)) {
		infixform_factor(car(p));
		p = cdr(p);
		print_char('(');
		if (iscons(p)) {
			infixform_expr(car(p));
			p = cdr(p);
			while (iscons(p)) {
				print_char(',');
				infixform_expr(car(p));
				p = cdr(p);
			}
		}
		print_char(')');
		return;
	}

	if (p == symbol(DERIVATIVE))
		print_char('d');
	else if (p == symbol(EXP1))
		print_str("exp(1)");
	else
		print_str(printname(p));
}

void
print_index_function(struct atom *p)
{
	p = cdr(p);
	if (caar(p) == symbol(ADD) || caar(p) == symbol(MULTIPLY) || caar(p) == symbol(POWER) || caar(p) == symbol(FACTORIAL))
		infixform_subexpr(car(p));
	else
		infixform_expr(car(p));
	print_char('[');
	p = cdr(p);
	if (iscons(p)) {
		infixform_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			print_char(',');
			infixform_expr(car(p));
			p = cdr(p);
		}
	}
	print_char(']');
}

void
infixform_factorial_function(struct atom *p)
{
	p = cadr(p);
	if (isposint(p) || issymbol(p))
		infixform_expr(p);
	else
		infixform_subexpr(p);
	print_char('!');
}

void
infixform_tensor(struct atom *p)
{
	int k = 0;
	infixform_tensor_inner(p, 0, &k);
}

void
infixform_tensor_inner(struct atom *p, int j, int *k)
{
	int i;
	print_char('(');
	for (i = 0; i < p->u.tensor->dim[j]; i++) {
		if (j + 1 == p->u.tensor->ndim) {
			infixform_expr(p->u.tensor->elem[*k]);
			*k = *k + 1;
		} else
			infixform_tensor_inner(p, j + 1, k);
		if (i + 1 < p->u.tensor->dim[j]) {
			print_char(',');
		}
	}
	print_char(')');
}

void
infixform_arg_list(struct atom *p)
{
	print_char('(');
	if (iscons(p)) {
		print_str(printname(car(p)));
		p = cdr(p);
		while (iscons(p)) {
			print_char(',');
			print_str(printname(car(p)));
			p = cdr(p);
		}
	}
	print_char(')');
}

void
infixform_multiply_sign(void)
{
	print_char(' ');
}

int
is_denominator(struct atom *p)
{
	if (car(p) == symbol(POWER) && cadr(p) != symbol(EXP1) && isnegativeterm(caddr(p)))
		return 1;
	else
		return 0;
}

int
any_denominators(struct atom *p)
{
	struct atom *q;
	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (is_denominator(q))
			return 1;
		p = cdr(p);
	}
	return 0;
}

// sign has already been printed

void
infixform_number(struct atom *p)
{
	char buf[24], *s;
	switch (p->k) {
	case RATIONAL:
		s = mstr(p->u.q.a);
		print_str(s);
		s = mstr(p->u.q.b);
		if (strcmp(s, "1") == 0)
			break;
		print_char('/');
		print_str(s);
		break;
	case DOUBLE:
		sprintf(buf, "%g", fabs(p->u.d));
		s = buf;
		while (*s && *s != 'E' && *s != 'e')
			print_char(*s++);
		if (!*s)
			break;
		s++;
		print_str(" 10^");
		if (*s == '-') {
			print_str("(-");
			s++;
			while (*s == '0')
				s++; // skip leading zeroes
			print_str(s);
			print_char(')');
		} else {
			if (*s == '+')
				s++;
			while (*s == '0')
				s++; // skip leading zeroes
			print_str(s);
		}
		break;
	default:
		break;
	}
}
