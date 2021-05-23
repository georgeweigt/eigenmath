function
power_tensor(BASE, EXPO)
{
	var i, j, k, n;

	if (!isnum(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (isrational(EXPO))
		n = EXPO.a / EXPO.b;
	else
		n = EXPO.d;

	if (n != Math.floor(n)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (n == 0) {
		if (!issquarematrix(BASE))
			stopf("square matrix expected");
		n = BASE.dim[0];
		BASE = alloc_tensor();
		BASE.dim[0] = n;
		BASE.dim[1] = n;
		k = 0;
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				if (i == j)
					BASE.elem[k++] = one;
				else
					BASE.elem[k++] = zero;
		push(BASE);
		return;
	}

	if (n < 0) {
		n = -n;
		push(BASE);
		inv();
		BASE = pop();
	}

	push(BASE);

	for (i = 1; i < n; i++) {
		push(BASE);
		hadamard();
	}
}
