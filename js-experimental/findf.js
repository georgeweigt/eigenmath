function
findf(p, q) // is q in p?
{
	var i, n;

	if (equal(p, q))
		return 1;

	if (istensor(p)) {
		n = p.elem.length;
		for (i = 0; i < n; i++) {
			if (findf(p.elem[i], q))
				return 1;
		}
		return 0;
	}

	while (iscons(p)) {
		if (findf(car(p), q))
			return 1;
		p = cdr(p);
	}

	return 0;
}
