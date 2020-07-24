#include "defs.h"

void
eval_print(void)
{
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(car(p1));
		eval();
		print_result();
		p1 = cdr(p1);
	}
	push_symbol(NIL);
}

void
print_result(void)
{
	save();
	print_result_nib();
	restore();
}

void
print_result_nib(void)
{
	p2 = pop(); // result
	p1 = pop(); // input

	if (p2 == symbol(NIL))
		return;

	if (issymbol(p1))
		prep_symbol_equals();

	if (iszero(binding[TTY])) {
		push(p2);
		cmdisplay();
		return;
	}

	print(p2);
}

void
prep_symbol_equals(void)
{
	if (p1 == p2)
		return; // A = A

	if (iskeyword(p1))
		return; // keyword like "float"

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
eval_string(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();

	print_nib(p1);
	print_char('\0');

	push_string(outbuf);
}

void
print(struct atom *p)
{
	print_nib(p);
	print_char('\n');
	print_char('\0');
	printbuf(outbuf, BLACK);
}

void
print_nib(struct atom *p)
{
	outbuf_index = 0;
	if (car(p) == symbol(SETQ)) {
		print_expr(cadr(p));
		print_str(" = ");
		print_expr(caddr(p));
	} else
		print_expr(p);
}

void
print_subexpr(struct atom *p)
{
	print_char('(');
	print_expr(p);
	print_char(')');
}

void
print_expr(struct atom *p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		if (sign_of_term(car(p)) == '-')
			print_str("-");
		print_term(car(p));
		p = cdr(p);
		while (iscons(p)) {
			if (sign_of_term(car(p)) == '+')
				print_str(" + ");
			else
				print_str(" - ");
			print_term(car(p));
			p = cdr(p);
		}
	} else {
		if (sign_of_term(p) == '-')
			print_str("-");
		print_term(p);
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
		absval();
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
			print_factor(A);
			flag = 1;
		}
		while (iscons(p1)) {
			p2 = car(p1);
			if (is_denominator(p2))
				;
			else {
				if (flag)
					print_multiply_sign();
				print_factor(p2);
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
		print_factor(B);
		flag = 1;
	}

	while (iscons(p1)) {
		p2 = car(p1);
		if (is_denominator(p2)) {
			if (flag)
				print_multiply_sign();
			print_denom(p2, d);
			flag = 1;
		}
		p1 = cdr(p1);
	}

	if (d > 1)
		print_char(')');

	restore();
}

void
print_term(struct atom *p)
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

		print_factor(car(p));
		p = cdr(p);
		while (iscons(p)) {
			print_multiply_sign();
			print_factor(car(p));
			p = cdr(p);
		}
	} else
		print_factor(p);
}

// prints stuff after the divide symbol "/"

// d is the number of denominators

#undef BASE
#undef EXPO

#define BASE p1
#define EXPO p2

void
print_denom(struct atom *p, int d)
{
	save();

	BASE = cadr(p);
	EXPO = caddr(p);

	// i.e. 1 / (2^(1/3))

	if (d == 1 && !isminusone(EXPO))
		print_char('(');

	if (isfraction(BASE) || car(BASE) == symbol(ADD) || car(BASE) == symbol(MULTIPLY) || car(BASE) == symbol(POWER) || isnegativenumber(BASE)) {
			print_char('(');
			print_expr(BASE);
			print_char(')');
	} else
		print_expr(BASE);

	if (isminusone(EXPO)) {
		restore();
		return;
	}

	print_str("^");

	push(EXPO);
	negate();
	EXPO = pop();

	if (isfraction(EXPO) || car(EXPO) == symbol(ADD) || car(EXPO) == symbol(MULTIPLY) || car(EXPO) == symbol(POWER)) {
		print_char('(');
		print_expr(EXPO);
		print_char(')');
	} else
		print_expr(EXPO);

	if (d == 1)
		print_char(')');

	restore();
}

void
print_factor(struct atom *p)
{
	if (isnum(p)) {
		print_number(p);
		return;
	}

	if (isstr(p)) {
		print_str(p->u.str);
		return;
	}

	if (istensor(p)) {
		print_tensor(p);
		return;
	}

	if (car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY)) {
		print_str("(");
		print_expr(p);
		print_str(")");
		return;
	}

	if (car(p) == symbol(POWER)) {

		if (isimaginaryunit(p)) {
			if (isimaginaryunit(binding[SYMBOL_J])) {
				print_char('j');
				return;
			}
			if (isimaginaryunit(binding[SYMBOL_I])) {
				print_char('i');
				return;
			}
		}

		if (cadr(p) == symbol(EXP1)) {
			print_str("exp(");
			print_expr(caddr(p));
			print_str(")");
			return;
		}

		if (isminusone(caddr(p))) {
			print_str("1 / ");
			if (iscons(cadr(p))) {
				print_str("(");
				print_expr(cadr(p));
				print_str(")");
			} else
				print_expr(cadr(p));
			return;
		}

		if (caadr(p) == symbol(ADD) || caadr(p) == symbol(MULTIPLY) || caadr(p) == symbol(POWER) || isnegativenumber(cadr(p))) {
			print_str("(");
			print_expr(cadr(p));
			print_str(")");
		} else if (isnum(cadr(p)) && (lessp(cadr(p), zero) || isfraction(cadr(p)))) {
			print_str("(");
			print_factor(cadr(p));
			print_str(")");
		} else
			print_factor(cadr(p));
		print_str("^");
		if (iscons(caddr(p)) || isfraction(caddr(p)) || (isnum(caddr(p)) && lessp(caddr(p), zero))) {
			print_str("(");
			print_expr(caddr(p));
			print_str(")");
		} else
			print_factor(caddr(p));
		return;
	}

	if (car(p) == symbol(INDEX) && issymbol(cadr(p))) {
		print_index_function(p);
		return;
	}

	if (car(p) == symbol(FACTORIAL)) {
		print_factorial_function(p);
		return;
	}

	if (iscons(p)) {
		print_factor(car(p));
		p = cdr(p);
		print_str("(");
		if (iscons(p)) {
			print_expr(car(p));
			p = cdr(p);
			while (iscons(p)) {
				print_str(",");
				print_expr(car(p));
				p = cdr(p);
			}
		}
		print_str(")");
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
		print_subexpr(car(p));
	else
		print_expr(car(p));
	print_char('[');
	p = cdr(p);
	if (iscons(p)) {
		print_expr(car(p));
		p = cdr(p);
		while(iscons(p)) {
			print_char(',');
			print_expr(car(p));
			p = cdr(p);
		}
	}
	print_char(']');
}

void
print_factorial_function(struct atom *p)
{
	p = cadr(p);
	if (isposint(p) || issymbol(p))
		print_expr(p);
	else
		print_subexpr(p);
	print_char('!');
}

void
print_tensor(struct atom *p)
{
	int k = 0;
	print_tensor_inner(p, 0, &k);
}

void
print_tensor_inner(struct atom *p, int j, int *k)
{
	int i;
	print_str("(");
	for (i = 0; i < p->u.tensor->dim[j]; i++) {
		if (j + 1 == p->u.tensor->ndim) {
			print_expr(p->u.tensor->elem[*k]);
			*k = *k + 1;
		} else
			print_tensor_inner(p, j + 1, k);
		if (i + 1 < p->u.tensor->dim[j]) {
			print_str(",");
		}
	}
	print_str(")");
}

void
print_function_definition(struct atom *p)
{
	print_str(printname(p));
	print_arg_list(cadr(get_binding(p)));
	print_str("=");
	print_expr(caddr(get_binding(p)));
	print_str("\n");
}

void
print_arg_list(struct atom *p)
{
	print_str("(");
	if (iscons(p)) {
		print_str(printname(car(p)));
		p = cdr(p);
		while (iscons(p)) {
			print_str(",");
			print_str(printname(car(p)));
			p = cdr(p);
		}
	}
	print_str(")");
}

void
print_multiply_sign(void)
{
	print_str(" ");
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
print_number(struct atom *p)
{
	char *s;
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
		sprintf(tbuf, "%g", p->u.d);
		s = tbuf;
		if (*s == '+' || *s == '-')
			s++;
		if (isinf(p->u.d) || isnan(p->u.d)) {
			print_str(s);
			break;
		}
		if (strchr(tbuf, 'e') || strchr(tbuf, 'E'))
			print_char('(');
		while (*s && *s != 'e' && *s != 'E')
			print_char(*s++);
		if (!strchr(tbuf, '.'))
			print_str(".0");
		if (*s == 'e' || *s == 'E') {
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
		}
		if (strchr(tbuf, 'e') || strchr(tbuf, 'E'))
			print_char(')');
		break;
	default:
		break;
	}
}

void
eval_lisp(void)
{
	push(cadr(p1));
	eval();
	p1 = pop();

	outbuf_index = 0;
	print_lisp_nib(p1);
	print_char('\0');

	push_string(outbuf);
}

void
print_lisp(struct atom *p)
{
	outbuf_index = 0;
	print_lisp_nib(p);
	print_char('\n');
	print_char('\0');
	printbuf(outbuf, BLACK);
}

void
print_lisp_nib(struct atom *p)
{
	int i;
	char *s;
	switch (p->k) {
	case CONS:
		print_str("(");
		print_lisp_nib(car(p));
		p = cdr(p);
		while (iscons(p)) {
			print_str(" ");
			print_lisp_nib(car(p));
			p = cdr(p);
		}
		if (p != symbol(NIL)) {
			print_str(" . ");
			print_lisp_nib(p);
		}
		print_str(")");
		break;
	case STR:
		print_str("\"");
		print_str(p->u.str);
		print_str("\"");
		break;
	case RATIONAL:
		if (p->sign == MMINUS)
			print_char('-');
		s = mstr(p->u.q.a);
		print_str(s);
		s = mstr(p->u.q.b);
		if (strcmp(s, "1") == 0)
			break;
		print_char('/');
		print_str(s);
		break;
	case DOUBLE:
		sprintf(tbuf, "%g", p->u.d);
		if (isinf(p->u.d) || isnan(p->u.d)) {
			print_str(tbuf);
			break;
		}
		if (strchr(tbuf, 'e') || strchr(tbuf, 'E'))
			print_str("(* ");
		s = tbuf;
		while (*s && *s != 'e' && *s != 'E')
			print_char(*s++);
		if (!strchr(tbuf, '.'))
			print_str(".0");
		if (*s == 'e' || *s == 'E') {
			s++;
			print_str(" (^ 10 ");
			if (*s == '+')
				s++;
			else if (*s == '-')
				print_char(*s++);
			while (*s == '0')
				s++; // skip leading zeroes
			print_str(s);
			print_char(')');
		}
		if (strchr(tbuf, 'e') || strchr(tbuf, 'E'))
			print_char(')');
		break;
	case KSYM:
	case USYM:
		print_str(printname(p));
		break;
	case TENSOR:
		print_str("(tensor");
		sprintf(tbuf, " %d", p->u.tensor->ndim);
		print_str(tbuf);
		for (i = 0; i < p->u.tensor->ndim; i++) {
			sprintf(tbuf, " %d", p->u.tensor->dim[i]);
			print_str(tbuf);
		}
		for (i = 0; i < p->u.tensor->nelem; i++) {
			print_str(" ");
			print_lisp_nib(p->u.tensor->elem[i]);
		}
		print_str(")");
		break;
	default:
		print_str("ERROR");
		break;
	}
}

void
print_str(char *s)
{
	while (*s)
		print_char(*s++);
}

void
print_char(int c)
{
	if (outbuf_index == outbuf_length) {
		outbuf_length += 10000;
		outbuf = (char *) realloc(outbuf, outbuf_length);
		if (outbuf == NULL)
			malloc_kaput();
	}
	outbuf[outbuf_index++] = c;
}
