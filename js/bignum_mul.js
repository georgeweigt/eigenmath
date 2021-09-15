function
bignum_mul(u, v)
{
	var i, j, nu, nv, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nu == 1 && nv == 1) {
		w[0] = u[0] * v[0];
		if (w[0] >= BIGM) {
			w[1] = Math.floor(w[0] / BIGM);
			w[0] %= BIGM;
		}
		return w;
	}

	for (i = 0; i < nu; i++)
		w[i] = 0;

	for (j = 0; j < nv; j++) {
		t = 0;
		for (i = 0; i < nu; i++) {
			t += u[i] * v[j] + w[i + j];
			w[i + j] = t % BIGM;
			t = Math.floor(t / BIGM);
		}
		w[i + j] = t;
	}

	bignum_norm(w);

	return w;
}
