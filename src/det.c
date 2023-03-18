void
eval_det(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	det();
}

void
det(void)
{
	int h, i, j, k, m, n;
	struct atom *p1, *p2;

	p1 = pop();

	if (!istensor(p1)) {
		push(p1);
		return;
	}

	if (p1->u.tensor->ndim != 2 || p1->u.tensor->dim[0] != p1->u.tensor->dim[1])
		stopf("det: square matrix expected");

	n = p1->u.tensor->dim[0];

	switch (n) {
	case 1:
		push(p1->u.tensor->elem[0]);
		return;
	case 2:
		push(p1->u.tensor->elem[0]);
		push(p1->u.tensor->elem[3]);
		multiply();
		push(p1->u.tensor->elem[1]);
		push(p1->u.tensor->elem[2]);
		multiply();
		subtract();
		return;
	case 3:
		push(p1->u.tensor->elem[0]);
		push(p1->u.tensor->elem[4]);
		push(p1->u.tensor->elem[8]);
		multiply_factors(3);
		push(p1->u.tensor->elem[1]);
		push(p1->u.tensor->elem[5]);
		push(p1->u.tensor->elem[6]);
		multiply_factors(3);
		push(p1->u.tensor->elem[2]);
		push(p1->u.tensor->elem[3]);
		push(p1->u.tensor->elem[7]);
		multiply_factors(3);
		push_integer(-1);
		push(p1->u.tensor->elem[2]);
		push(p1->u.tensor->elem[4]);
		push(p1->u.tensor->elem[6]);
		multiply_factors(4);
		push_integer(-1);
		push(p1->u.tensor->elem[1]);
		push(p1->u.tensor->elem[3]);
		push(p1->u.tensor->elem[8]);
		multiply_factors(4);
		push_integer(-1);
		push(p1->u.tensor->elem[0]);
		push(p1->u.tensor->elem[5]);
		push(p1->u.tensor->elem[7]);
		multiply_factors(4);
		add_terms(6);
		return;
	default:
		break;
	}

	p2 = alloc_matrix(n - 1, n - 1);

	h = tos;

	for (m = 0; m < n; m++) {
		if (iszero(p1->u.tensor->elem[m]))
			continue;
		k = 0;
		for (i = 1; i < n; i++)
			for (j = 0; j < n; j++)
				if (j != m)
					p2->u.tensor->elem[k++] = p1->u.tensor->elem[n * i + j];
		push(p2);
		det();
		push(p1->u.tensor->elem[m]);
		multiply();
		if (m % 2)
			negate();
	}

	n = tos - h;

	if (n == 0)
		push_integer(0);
	else
		add_terms(n);
}
