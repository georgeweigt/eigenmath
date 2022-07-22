int
equal(struct atom *p1, struct atom *p2)
{
	int i, n;
	double d;

	if (p1 == p2)
		return 1;

	if (istensor(p1) && istensor(p2)) {
		if (p1->u.tensor->ndim != p2->u.tensor->ndim)
			return 0;
		n = p1->u.tensor->ndim;
		for (i = 0; i < n; i++)
			if (p1->u.tensor->dim[i] != p2->u.tensor->dim[i])
				return 0;
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++)
			if (!equal(p1->u.tensor->elem[i], p2->u.tensor->elem[i]))
				return 0;
		return 1;
	}

	if (iscons(p1) && iscons(p2)) {
		while (iscons(p1) && iscons(p2)) {
			if (!equal(car(p1), car(p2)))
				return 0;
			p1 = cdr(p1);
			p2 = cdr(p2);
		}
		if (p1 == symbol(NIL) && p2 == symbol(NIL))
			return 1;
		else
			return 0;
	}

	if (isrational(p1) && isrational(p2)) {
		if (p1->sign != p2->sign)
			return 0;
		if (!meq(p1->u.q.a, p2->u.q.a))
			return 0;
		if (!meq(p1->u.q.b, p2->u.q.b))
			return 0;
		return 1;
	}

	if (isrational(p1) && isdouble(p2)) {
		push(p1);
		d = pop_double();
		if (d == p2->u.d)
			return 1;
		else
			return 0;
	}

	if (isdouble(p1) && isrational(p2)) {
		push(p2);
		d = pop_double();
		if (p1->u.d == d)
			return 1;
		else
			return 0;
	}

	if (isdouble(p1) && isdouble(p2)) {
		if (p1->u.d == p2->u.d)
			return 1;
		else
			return 0;
	}

	if (iskeyword(p1) && iskeyword(p2)) {
		if (strcmp(p1->u.ksym.name, p2->u.ksym.name) == 0)
			return 1;
		else
			return 0;
	}

	if (isstr(p1) && isstr(p2)) {
		if (strcmp(p1->u.str, p2->u.str) == 0)
			return 1;
		else
			return 0;
	}

	return 0;
}
