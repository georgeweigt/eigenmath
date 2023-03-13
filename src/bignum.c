void
init_bignums(void)
{
	push_bignum(MPLUS, mint(0), mint(1));
	zero = pop();
	push_bignum(MPLUS, mint(1), mint(1));
	one = pop();
	push_bignum(MMINUS, mint(1), mint(1));
	minusone = pop();
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
	struct atom *p;

	p = pop();

	if (!issmallinteger(p))
		stopf("small integer expected");

	if (isrational(p)) {
		n = p->u.q.a[0];
		if (isnegativenumber(p))
			n = -n;
	} else
		n = (int) p->u.d;

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
	double a, b;
	struct atom *p;

	p = pop();

	if (isrational(p)) {
		a = bignum_float(p->u.q.a);
		b = bignum_float(p->u.q.b);
		if (isnegativenumber(p))
			a = -a;
		return a / b;
	}

	if (isdouble(p))
		return p->u.d;

	stopf("number expected");

	return 0.0;
}

int
isequaln(struct atom *p, int n)
{
	if (isrational(p))
		return p->sign == (n < 0 ? MMINUS : MPLUS) && MEQUAL(p->u.q.a, abs(n)) && MEQUAL(p->u.q.b, 1);
	else if (isdouble(p))
		return p->u.d == (double) n;
	else
		return 0;
}

int
isequalq(struct atom *p, int a, int b)
{
	if (isrational(p))
		return p->sign == (a < 0 ? MMINUS : MPLUS) && MEQUAL(p->u.q.a, abs(a)) && MEQUAL(p->u.q.b, b);
	else if (isdouble(p))
		return p->u.d == (double) a / b;
	else
		return 0;
}

int
cmpfunc(void)
{
	int t;
	struct atom *p1, *p2;
	p2 = pop();
	p1 = pop();
	t = cmp_numbers(p1, p2);
	return t;
}

int
cmp_numbers(struct atom *p1, struct atom *p2)
{
	double d1, d2;

	if (!isnum(p1) || !isnum(p2))
		stopf("compare");

	if (isrational(p1) && isrational(p2))
		return cmp_rationals(p1, p2);

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	if (d1 < d2)
		return -1;

	if (d1 > d2)
		return 1;

	return 0;
}

int
cmp_rationals(struct atom *a, struct atom *b)
{
	int t;
	uint32_t *ab, *ba;
	if (a->sign == MMINUS && b->sign == MPLUS)
		return -1;
	if (a->sign == MPLUS && b->sign == MMINUS)
		return 1;
	if (isinteger(a) && isinteger(b)) {
		if (a->sign == MMINUS)
			return mcmp(b->u.q.a, a->u.q.a);
		else
			return mcmp(a->u.q.a, b->u.q.a);
	}
	ab = mmul(a->u.q.a, b->u.q.b);
	ba = mmul(a->u.q.b, b->u.q.a);
	if (a->sign == MMINUS)
		t = mcmp(ba, ab);
	else
		t = mcmp(ab, ba);
	mfree(ab);
	mfree(ba);
	return t;
}

void
bignum_scan_integer(char *s)
{
	int sign;
	uint32_t *a;
	if (*s == '-')
		sign = MMINUS;
	else
		sign = MPLUS;
	if (*s == '+' || *s == '-')
		s++;
	a = mscan(s);
	if (a == NULL)
		stopf("parse error");
	push_bignum(sign, a, mint(1));
}

double
bignum_float(uint32_t *p)
{
	int i, n;
	double d;
	n = MLENGTH(p);
	d = 0.0;
	for (i = 0; i < n; i++)
		d += scalbn((double) p[i], 32 * i);
	return d;
}

void
bignum_factorial(int n)
{
	push_bignum(MPLUS, bignum_factorial_nib(n), mint(1));
}

uint32_t *
bignum_factorial_nib(int n)
{
	int i;
	uint32_t *a, *b, *t;
	if (n == 0 || n == 1)
		return mint(1);
	a = mint(2);
	b = mint(0);
	for (i = 3; i <= n; i++) {
		b[0] = (uint32_t) i;
		t = mmul(a, b);
		mfree(a);
		a = t;
	}
	mfree(b);
	return a;
}

void
msetbit(uint32_t *x, uint32_t k)
{
	x[k / 32] |= 1 << (k % 32);
}

void
mclrbit(uint32_t *x, uint32_t k)
{
	x[k / 32] &= ~(1 << (k % 32));
}

uint32_t *
mscan(char *s)
{
	uint32_t *a, *b, *t;
	a = mint(0);
	t = mint(0);
	while (*s) {
		if (*s < '0' || *s > '9') {
			mfree(a);
			a = NULL;
			break;
		}
		t[0] = 10;
		b = mmul(a, t);
		mfree(a);
		t[0] = *s - '0';
		a = madd(b, t);
		mfree(b);
		s++;
	}
	mfree(t);
	return a;
}

// convert bignum to string (returned value points to static buffer)

char *
mstr(uint32_t *u)
{
	int i, k, n, r;
	static char *buf;
	static int len;

	// estimate string length

	// note that 0xffffffff -> 000000004 294967295

	// hence space for 8 leading zeroes is required

	n = 10 * MLENGTH(u) + 10;

	n = 1000 * (n / 1000 + 1);

	if (n > len) {
		if (buf)
			free(buf);
		buf = alloc_mem(n);
		len = n;
	}

	u = mcopy(u);

	k = len - 1;
	buf[k] = '\0'; // string terminator

	for (;;) {
		r = mdivby1billion(u);
		for (i = 0; i < 9; i++) {
			buf[--k] = r % 10 + '0';
			r /= 10;
		}
		if (MZERO(u))
			break;
	}

	mfree(u);

	// remove leading zeroes

	while (buf[k] == '0' && buf[k + 1])
		k++;

	return buf + k;
}

// returns remainder, quotient returned in u

int
mdivby1billion(uint32_t *u)
{
	int i;
	uint64_t r = 0;
	for (i = MLENGTH(u) - 1; i >= 0; i--) {
		r = r << 32 | u[i];
		u[i] = (uint32_t) (r / 1000000000);
		r -= (uint64_t) 1000000000 * u[i];
	}
	mnorm(u);
	return (int) r;
}

// returns u + v

uint32_t *
madd(uint32_t *u, uint32_t *v)
{
	int i, nu, nv, nw;
	uint64_t t;
	uint32_t *w;
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	if (nu > nv)
		nw = nu + 1;
	else
		nw = nv + 1;
	w = mnew(nw);
	for (i = 0; i < nu; i++)
		w[i] = u[i];
	for (i = nu; i < nw; i++)
		w[i] = 0;
	t = 0;
	for (i = 0; i < nv; i++) {
		t += (uint64_t) w[i] + v[i];
		w[i] = (uint32_t) t;
		t >>= 32;
	}
	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = (uint32_t) t;
		t >>= 32;
	}
	mnorm(w);
	return w;
}

// returns u - v

uint32_t *
msub(uint32_t *u, uint32_t *v)
{
	int i, nu, nv, nw;
	uint64_t t;
	uint32_t *w;
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	if (nu > nv)
		nw = nu;
	else
		nw = nv;
	w = mnew(nw);
	for (i = 0; i < nu; i++)
		w[i] = u[i];
	for (i = nu; i < nw; i++)
		w[i] = 0;
	t = 0;
	for (i = 0; i < nv; i++) {
		t += (uint64_t) w[i] - v[i];
		w[i] = (uint32_t) t;
		t = (int64_t) t >> 32; // cast to extend sign
	}
	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = (uint32_t) t;
		t = (int64_t) t >> 32; // cast to extend sign
	}
	mnorm(w);
	return w;
}

// returns u * v

uint32_t *
mmul(uint32_t *u, uint32_t *v)
{
	int i, j, nu, nv, nw;
	uint64_t t;
	uint32_t *w;
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	nw = nu + nv;
	w = mnew(nw);
	for (i = 0; i < nw; i++)
		w[i] = 0;
	for (i = 0; i < nu; i++) {
		t = 0;
		for (j = 0; j < nv; j++) {
			t += (uint64_t) u[i] * v[j] + w[i + j];
			w[i + j] = (uint32_t) t;
			t >>= 32;
		}
		w[i + j] = (uint32_t) t;
	}
	mnorm(w);
	return w;
}

// returns floor(u / v)

uint32_t *
mdiv(uint32_t *u, uint32_t *v)
{
	int i, k, nu, nv;
	uint32_t *q, qhat, *w;
	uint64_t a, b, t;
	mnorm(u);
	mnorm(v);
	if (MLENGTH(v) == 1 && v[0] == 0)
		stopf("divide by zero"); // v = 0
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	k = nu - nv;
	if (k < 0) {
		q = mnew(1);
		q[0] = 0;
		return q; // u < v, return zero
	}
	u = mcopy(u);
	q = mnew(k + 1);
	w = mnew(nv + 1);
	b = v[nv - 1];
	do {
		q[k] = 0;
		while (nu >= nv + k) {
			// estimate 32-bit partial quotient
			a = u[nu - 1];
			if (nu > nv + k)
				a = a << 32 | u[nu - 2];
			if (a < b)
				break;
			qhat = (uint32_t) (a / (b + 1));
			if (qhat == 0)
				qhat = 1;
			// w = qhat * v
			t = 0;
			for (i = 0; i < nv; i++) {
				t += (uint64_t) qhat * v[i];
				w[i] = (uint32_t) t;
				t >>= 32;
			}
			w[nv] = (uint32_t) t;
			// u = u - w
			t = 0;
			for (i = k; i < nu; i++) {
				t += (uint64_t) u[i] - w[i - k];
				u[i] = (uint32_t) t;
				t = (int64_t) t >> 32; // cast to extend sign
			}
			if (t) {
				// u is negative, restore u
				t = 0;
				for (i = k; i < nu; i++) {
					t += (uint64_t) u[i] + w[i - k];
					u[i] = (uint32_t) t;
					t >>= 32;
				}
				break;
			}
			q[k] += qhat;
			mnorm(u);
			nu = MLENGTH(u);
		}
	} while (--k >= 0);
	mnorm(q);
	mfree(u);
	mfree(w);
	return q;
}

// returns u mod v

uint32_t *
mmod(uint32_t *u, uint32_t *v)
{
	int i, k, nu, nv;
	uint32_t qhat, *w;
	uint64_t a, b, t;
	mnorm(u);
	mnorm(v);
	if (MLENGTH(v) == 1 && v[0] == 0)
		stopf("divide by zero"); // v = 0
	u = mcopy(u);
	nu = MLENGTH(u);
	nv = MLENGTH(v);
	k = nu - nv;
	if (k < 0)
		return u; // u < v
	w = mnew(nv + 1);
	b = v[nv - 1];
	do {
		while (nu >= nv + k) {
			// estimate 32-bit partial quotient
			a = u[nu - 1];
			if (nu > nv + k)
				a = a << 32 | u[nu - 2];
			if (a < b)
				break;
			qhat = (uint32_t) (a / (b + 1));
			if (qhat == 0)
				qhat = 1;
			// w = qhat * v
			t = 0;
			for (i = 0; i < nv; i++) {
				t += (uint64_t) qhat * v[i];
				w[i] = (uint32_t) t;
				t >>= 32;
			}
			w[nv] = (uint32_t) t;
			// u = u - w
			t = 0;
			for (i = k; i < nu; i++) {
				t += (uint64_t) u[i] - w[i - k];
				u[i] = (uint32_t) t;
				t = (int64_t) t >> 32; // cast to extend sign
			}
			if (t) {
				// u is negative, restore u
				t = 0;
				for (i = k; i < nu; i++) {
					t += (uint64_t) u[i] + w[i - k];
					u[i] = (uint32_t) t;
					t >>= 32;
				}
				break;
			}
			mnorm(u);
			nu = MLENGTH(u);
		}
	} while (--k >= 0);
	mfree(w);
	return u;
}

// returns u ** v

uint32_t *
mpow(uint32_t *u, uint32_t *v)
{
	uint32_t *t, *w;
	u = mcopy(u);
	v = mcopy(v);
	// w = 1
	w = mnew(1);
	w[0] = 1;
	for (;;) {
		if (v[0] & 1) {
			// w = w * u
			t = mmul(w, u);
			mfree(w);
			w = t;
		}
		// v = v >> 1
		mshr(v);
		// v = 0?
		if (MLENGTH(v) == 1 && v[0] == 0)
			break;
		// u = u * u
		t = mmul(u, u);
		mfree(u);
		u = t;
	}
	mfree(u);
	mfree(v);
	return w;
}

// u = u >> 1

void
mshr(uint32_t *u)
{
	int i;
	for (i = 0; i < MLENGTH(u) - 1; i++) {
		u[i] >>= 1;
		if (u[i + 1] & 1)
			u[i] |= 0x80000000;
	}
	u[i] >>= 1;
	mnorm(u);
}

// compare u and v

int
mcmp(uint32_t *u, uint32_t *v)
{
	int i;
	mnorm(u);
	mnorm(v);
	if (MLENGTH(u) < MLENGTH(v))
		return -1;
	if (MLENGTH(u) > MLENGTH(v))
		return 1;
	for (i = MLENGTH(u) - 1; i >= 0; i--) {
		if (u[i] < v[i])
			return -1;
		if (u[i] > v[i])
			return 1;
	}
	return 0; // u = v
}

int
meq(uint32_t *u, uint32_t *v)
{
	int i;
	if (MLENGTH(u) != MLENGTH(v))
		return 0;
	for (i = 0; i < MLENGTH(u); i++)
		if (u[i] != v[i])
			return 0;
	return 1;
}

// convert unsigned to bignum

uint32_t *
mint(uint32_t n)
{
	uint32_t *p;
	p = mnew(1);
	p[0] = n;
	return p;
}

uint32_t *
mnew(int n)
{
	uint32_t *u;
	u = alloc_mem((n + 1) * sizeof (uint32_t));
	bignum_count++;
	*u = n;
	return u + 1;
}

void
mfree(uint32_t *u)
{
	free(u - 1);
	bignum_count--;
}

uint32_t *
mcopy(uint32_t *u)
{
	int i;
	uint32_t *v;
	v = mnew(MLENGTH(u));
	for (i = 0; i < MLENGTH(u); i++)
		v[i] = u[i];
	return v;
}

// remove leading zeroes

void
mnorm(uint32_t *u)
{
	while (MLENGTH(u) > 1 && u[MLENGTH(u) - 1] == 0)
		MLENGTH(u)--;
}

uint32_t *
mgcd(uint32_t *u, uint32_t *v)
{
	int i, k, n, sign;
	uint32_t *t;

	if (MZERO(u)) {
		t = mcopy(v);
		return t;
	}

	if (MZERO(v)) {
		t = mcopy(u);
		return t;
	}

	u = mcopy(u);
	v = mcopy(v);

	k = 0;

	while ((u[0] & 1) == 0 && (v[0] & 1) == 0) {
		mshr(u);
		mshr(v);
		k++;
	}

	if (u[0] & 1) {
		t = mcopy(v);
		sign = -1;
	} else {
		t = mcopy(u);
		sign = 1;
	}

	while (1) {

		while ((t[0] & 1) == 0)
			mshr(t);

		if (sign == 1) {
			mfree(u);
			u = mcopy(t);
		} else {
			mfree(v);
			v = mcopy(t);
		}

		mfree(t);

		if (mcmp(u, v) < 0) {
			t = msub(v, u);
			sign = -1;
		} else {
			t = msub(u, v);
			sign = 1;
		}

		if (MZERO(t)) {
			mfree(t);
			mfree(v);
			n = (k / 32) + 1;
			v = mnew(n);
			for (i = 0; i < n; i++)
				v[i] = 0;
			msetbit(v, k);
			t = mmul(u, v);
			mfree(u);
			mfree(v);
			return t;
		}
	}
}

// returns NULL if not perfect root, otherwise returns a^(1/n)

uint32_t *
mroot(uint32_t *a, uint32_t *n)
{
	int i, j, k;
	uint32_t *b, *c, m;

	if (MLENGTH(n) > 1 || n[0] == 0)
		return NULL;

	// k is bit length of a

	k = 32 * (MLENGTH(a) - 1);

	m = a[MLENGTH(a) - 1];

	while (m) {
		m >>= 1;
		k++;
	}

	if (k == 0)
		return mint(0);

	// initial guess of index of ms bit in result

	k = (k - 1) / n[0];

	j = k / 32 + 1; // k is bit index, not number of bits

	b = mnew(j);

	for (i = 0; i < j; i++)
		b[i] = 0;

	while (k >= 0) {
		msetbit(b, k);
		mnorm(b);
		c = mpow(b, n);
		switch (mcmp(c, a)) {
		case -1:
			break;
		case 0:
			mfree(c);
			return b;
		case 1:
			mclrbit(b, k);
			break;
		}
		mfree(c);
		k--;
	}

	mfree(b);

	return NULL;
}

int
bignum_issmallnum(uint32_t *N)
{
	return MLENGTH(N) == 1 && N[0] <= 0x7fffffff;
}

int
bignum_smallnum(uint32_t *N)
{
	return N[0] & 0x7fffffff;
}
