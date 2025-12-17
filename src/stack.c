void
push(struct atom *p)
{
	if (tos < 0 || tos >= STACKSIZE)
		stopf("stack error");
	stack[tos++] = p;
	if (tos > max_tos)
		max_tos = tos;
}

struct atom *
pop(void)
{
	if (tos < 1 || tos > STACKSIZE)
		stopf("stack error");
	return stack[--tos];
}

void
save_symbol(struct atom *p)
{
	push(p);
	push(get_binding(p));
	push(get_usrfunc(p));
}

void
restore_symbol(void)
{
	struct atom *p1, *p2, *p3;
	p3 = pop();
	p2 = pop();
	p1 = pop();
	set_symbol(p1, p2, p3);
}

void
dupl(void)
{
	struct atom *p1;
	p1 = pop();
	push(p1);
	push(p1);
}

void
swap(void)
{
	struct atom *p1, *p2;
	p1 = pop();
	p2 = pop();
	push(p1);
	push(p2);
}

void
push_integer(int n)
{
	switch (n) {
	case 0:
		push(zero);
		break;
	case 1:
		push(one);
		break;
	case -1:
		push(minusone);
		break;
	default:
		if (n < 0)
			push_bignum(MMINUS, mint(-n), mint(1));
		else
			push_bignum(MPLUS, mint(n), mint(1));
		break;
	}
}

void
push_rational(int a, int b)
{
	if (a < 0)
		push_bignum(MMINUS, mint(-a), mint(b));
	else
		push_bignum(MPLUS, mint(a), mint(b));
}

void
push_bignum(int sign, uint32_t *a, uint32_t *b)
{
	struct atom *p;

	// normalize zero

	if (MZERO(a)) {
		sign = MPLUS;
		if (!MEQUAL(b, 1)) {
			mfree(b);
			b = mint(1);
		}
	}

	p = alloc_atom();
	p->atomtype = RATIONAL;
	p->sign = sign;
	p->u.q.a = a;
	p->u.q.b = b;

	push(p);
}

int
pop_integer(void)
{
	int n;
	double d;
	uint32_t *a;
	struct atom *p;
	p = pop();
	if (!isnum(p))
		stopf("number expected, argument is not a number");
	if (isdouble(p)) {
		d = trunc(p->u.d);
		if (fabs(d) > 0x7fffffff)
			stopf("integer overflow");
		n = (int) d;
	} else {
		if (isfraction(p)) {
			a = mdiv(p->u.q.a, p->u.q.b);
			if (MLENGTH(a) > 1 || a[0] > 0x7fffffff) {
				mfree(a);
				stopf("integer overflow");
			}
			n = (int) a[0];
			mfree(a);
		} else {
			a = p->u.q.a;
			if (MLENGTH(a) > 1 || a[0] > 0x7fffffff)
				stopf("integer overflow");
			n = (int) a[0];
		}
		if (n > 0 && p->sign == MMINUS)
			n = -n;
	}
	return n;
}

void
push_double(double d)
{
	struct atom *p;
	p = alloc_atom();
	p->atomtype = DOUBLE;
	p->u.d = d;
	push(p);
}

double
pop_double(void)
{
	double a, b, d;
	struct atom *p;
	p = pop();
	if (!isnum(p))
		stopf("number expected, argument is not a number");
	if (isdouble(p))
		d = p->u.d;
	else {
		a = mfloat(p->u.q.a);
		b = mfloat(p->u.q.b);
		d = a / b;
		if (isnegativenumber(p))
			d = -d;
	}
	return d;
}

void
push_string(char *s)
{
	struct atom *p;
	p = alloc_str();
	s = strdup(s);
	if (s == NULL)
		exit(1);
	p->u.str = s;
	push(p);
}

// start from stack + h and remove n items

void
slice(int h, int n)
{
	int i, m;
	m = tos - h - n;
	if (m < 0)
		stopf("stack slice error");
	for (i = 0; i < m; i++)
		stack[h + i] = stack[h + n + i];
	tos -= n;
}
