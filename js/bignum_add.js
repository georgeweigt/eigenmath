function
bignum_add(u, v)
{
	var i, nu, nv, nw, t, w;

	w = bignum_int(0);

	nu = u.length;
	nv = v.length;

	if (nu == 1 && nv == 1) {
		w[0] = u[0] + v[0];
		if (w[0] >= BIGM) {
			w[0] -= BIGM;
			w[1] = 1;
		}
		return w;
	}

	if (nu > nv)
		nw = nu + 1;
	else
		nw = nv + 1;

	for (i = 0; i < nu; i++)
		w[i] = u[i];

	for (i = nu; i < nw; i++)
		w[i] = 0;

	t = 0;

	for (i = 0; i < nv; i++) {
		t += w[i] + v[i];
		w[i] = t % BIGM;
		t = Math.floor(t / BIGM);
	}

	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = t % BIGM;
		t = Math.floor(t / BIGM);
	}

	bignum_norm(w);

	return w;
}
