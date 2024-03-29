function
bignum_mul(u, v)
{
	var i, j, nu, nv, nw, t, w = [];

	nu = u.length;
	nv = v.length;

	nw = nu + nv;

	for (i = 0; i < nw; i++)
		w[i] = 0;

	for (i = 0; i < nu; i++) {
		t = 0;
		for (j = 0; j < nv; j++) {
			t += u[i] * v[j] + w[i + j];
			w[i + j] = t % BIGM;
			t = Math.floor(t / BIGM);
		}
		w[i + j] = t;
	}

	bignum_norm(w);

	return w;
}
