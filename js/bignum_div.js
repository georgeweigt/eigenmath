// floor(u / v)

function
bignum_div(u, v)
{
	var a, b, i, k, nu, nv, q = [], qhat, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nv == 1 && v[0] == 0)
		stopf("divide by zero");

	if (nu == 1 && nv == 1) {
		q[0] = Math.floor(u[0] / v[0]);
		return q;
	}

	k = nu - nv;

	if (k < 0) {
		q[0] = 0;
		return q; // u < v, return zero
	}

	for (i = 0; i <= k; i++)
		q[i] = 0;

	u = bignum_copy(u);

	b = v[nv - 1];

	do {
		while (nu >= nv + k) {

			// estimate partial quotient

			a = u[nu - 1];

			if (nu > nv + k)
				a = BIGM * a + u[nu - 2];

			if (a < b)
				break;

			qhat = Math.floor(a / (b + 1)) % BIGM;

			if (qhat == 0)
				qhat = 1;

			// w = qhat * v

			t = 0;

			for (i = 0; i < nv; i++) {
				t += qhat * v[i];
				w[i] = t % BIGM;
				t = Math.floor(t / BIGM);
			}

			w[nv] = t;

			// u = u - w

			t = 0;

			for (i = k; i < nu; i++) {
				t += u[i] - w[i - k];
				u[i] = t % BIGM;
				if (u[i] < 0)
					u[i] += BIGM;
				t = Math.floor(t / BIGM);
			}

			if (t) {
				// u is negative, restore u and break
				t = 0;
				for (i = k; i < nu; i++) {
					t += u[i] + w[i - k];
					u[i] = t % BIGM;
					t = Math.floor(t / BIGM);
				}
				break;
			}

			bignum_norm(u);
			nu = u.length;

			q[k] += qhat;
		}

	} while (--k >= 0);

	bignum_norm(q);

	return q;
}
