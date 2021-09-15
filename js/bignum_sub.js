// u is greater than or equal to v

function
bignum_sub(u, v)
{
	var i, nu, nv, nw, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nu == 1 && nv == 1) {
		w[0] = u[0] - v[0];
		if (w[0] < BIGM)
			return w;
	}

	if (nu > nv)
		nw = nu;
	else
		nw = nv;

	for (i = 0; i < nu; i++)
		w[i] = u[i];

	for (i = nu; i < nw; i++)
		w[i] = 0;

	t = 0;

	for (i = 0; i < nv; i++) {
		t += w[i] - v[i];
		w[i] = t % BIGM;
		if (w[i] < 0)
			w[i] += BIGM;
		t = Math.floor(t / BIGM);
	}

	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = t % BIGM;
		if (w[i] < 0)
			w[i] += BIGM;
		t = Math.floor(t / BIGM);
	}

	bignum_norm(w);

	return w;
}
