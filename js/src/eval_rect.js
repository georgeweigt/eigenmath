function
eval_rect(p1)
{
	push(cadr(p1));
	evalf();
	rect();
}

function
rect()
{
	var h, i, n, p1, p2, BASE, EXPO;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			rect();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		p1 = cdr(p1);
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	if (car(p1) != symbol(POWER)) {
		push(p1);
		return;
	}

	BASE = cadr(p1);
	EXPO = caddr(p1);

	// handle sum in exponent

	if (car(EXPO) == symbol(ADD)) {
		p1 = cdr(EXPO);
		h = stack.length;
		while (iscons(p1)) {
			push_symbol(POWER);
			push(BASE);
			push(car(p1));
			list(3);
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	// return mag(p1) * cos(arg(p1)) + i sin(arg(p1)))

	push(p1);
	mag();

	push(p1);
	arg();
	p2 = pop();

	push(p2);
	cosfunc();

	push(imaginaryunit);
	push(p2);
	sinfunc();
	multiply();

	add();

	multiply();
}
