#define NPRIME 10000
extern int primetab[];

void
factor_uint32(uint32_t n)
{
	int k, m;
	uint32_t d;

	for (k = 0; k < NPRIME; k++) {

		d = primetab[k];

		if (n / d < d)
			break; // n is 1 or prime

		m = 0;

		while (n % d == 0) {
			n /= d;
			m++;
		}

		if (m) {
			push_integer(d);
			push_integer(m);
		}
	}

	if (n > 1) {
		push_bignum(MPLUS, mint(n), mint(1)); // use push_bignum because n may be greater than 31 bits
		push_integer(1);
	}
}
